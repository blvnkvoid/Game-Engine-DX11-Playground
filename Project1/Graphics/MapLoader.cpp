#include "MapLoader.h"
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cfloat>
#include <map>
#include <tuple>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <functional>
#include <unordered_map>


void MapLoader::SetMaterialLoader(
    SharedMaterialLoader& materialLoader)
{
    m_materialLoader = &materialLoader;
}



float MapLoader::DistanceToChunk(
    const DirectX::XMFLOAT3 &position,
    const DirectX::BoundingBox &bounds)
{
    const float dx = max(
        std::abs(position.x - bounds.Center.x) -
            bounds.Extents.x,
        0.0f);

    const float dy = max(
        std::abs(position.y - bounds.Center.y) -
            bounds.Extents.y,
        0.0f);

    const float dz = max(
        std::abs(position.z - bounds.Center.z) -
            bounds.Extents.z,
        0.0f);

    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

bool MapLoader::UploadChunk(MapGeometryChunk &chunk)
{
    if (chunk.resident)
        return true;

    if (!m_batchDevice ||
        chunk.vertices.empty() ||
        chunk.indices.empty())
        return false;

    const size_t vertexBytes =
        chunk.vertices.size() * sizeof(SharedVertex);

    const size_t indexBytes =
        chunk.indices.size() * sizeof(UINT);

    if (vertexBytes > UINT_MAX ||
        indexBytes > UINT_MAX)
        return false;

    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = static_cast<UINT>(vertexBytes);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData{};
    vbData.pSysMem = chunk.vertices.data();

    HRESULT hr = m_batchDevice->CreateBuffer(
        &vbDesc,
        &vbData,
        chunk.vertexBuffer.GetAddressOf());

    if (FAILED(hr))
        return false;

    D3D11_BUFFER_DESC ibDesc{};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = static_cast<UINT>(indexBytes);
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData{};
    ibData.pSysMem = chunk.indices.data();

    hr = m_batchDevice->CreateBuffer(
        &ibDesc,
        &ibData,
        chunk.indexBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        chunk.Unload();
        return false;
    }

    chunk.resident = true;
    return true;
}

void MapLoader::UpdateChunkResidency(
    const DirectX::XMFLOAT3 &cameraPosition)
{
    constexpr float LOAD_RADIUS = 1200.0f;
    constexpr float UNLOAD_RADIUS = 1400.0f;

    // Conservative synchronous upload budget.
    // Later we can replace this with background preparation.
    constexpr uint32_t MAX_UPLOADS_PER_FRAME = 4;

    uint32_t uploads = 0;

    // Release distant geometry first.
    for (auto &chunk : m_geometryChunks)
    {
        if (!chunk.resident)
            continue;

        const float distance =
            DistanceToChunk(cameraPosition, chunk.bounds);

        if (distance > UNLOAD_RADIUS)
            chunk.Unload();
    }

    // Collect missing nearby chunks.
    struct PendingChunk
    {
        uint32_t index;
        float distance;
    };

    std::vector<PendingChunk> pending;

    for (uint32_t i = 0; i < m_geometryChunks.size(); ++i)
    {
        const auto &chunk = m_geometryChunks[i];

        if (chunk.resident)
            continue;

        const float distance =
            DistanceToChunk(cameraPosition, chunk.bounds);

        if (distance <= LOAD_RADIUS)
            pending.push_back({i, distance});
    }

    // Upload closest chunks first.
    std::sort(
        pending.begin(),
        pending.end(),
        [](const PendingChunk &a, const PendingChunk &b)
        {
            return a.distance < b.distance;
        });

    for (const auto &item : pending)
    {
        if (uploads >= MAX_UPLOADS_PER_FRAME)
            break;

        if (UploadChunk(m_geometryChunks[item.index]))
            ++uploads;
    }
}



void MapLoader::BuildGeometryChunks(
    const std::vector<UINT> &batchIndices)
{
    m_geometryChunks.clear();

    constexpr float CELL_SIZE = 128.0f;

    struct CellKey
    {
        int x, y, z;

        bool operator<(const CellKey &other) const
        {
            return std::tie(x, y, z) <
                   std::tie(other.x, other.y, other.z);
        }
    };

    // A cell can contain multiple material batches.
    std::map<CellKey, uint32_t> cellToChunk;

    // Global vertex index -> local vertex index,
    // maintained independently for every chunk.
    std::vector<std::unordered_map<UINT, UINT>> vertexRemaps;

    uint64_t copiedIndices = 0;

    for (uint32_t batchID = 0;
         batchID < m_renderBatches.size();
         ++batchID)
    {
        auto &batch = m_renderBatches[batchID];

        // The current batch bounds are derived from its
        // actual triangle vertices.
        //
        // We use the batch center to identify its cell.
        // This is a temporary association; the exact
        // spatial cell ID should eventually be retained
        // during BuildRenderBatches().
        const auto &center = batch.bounds.Center;

        CellKey key{
            batch.cellX,
            batch.cellY,
            batch.cellZ};

        auto found = cellToChunk.find(key);

        uint32_t chunkID;

        if (found == cellToChunk.end())
        {
            chunkID = static_cast<uint32_t>(
                m_geometryChunks.size());

            cellToChunk.emplace(key, chunkID);

            MapGeometryChunk chunk{};

            chunk.cellX = key.x;
            chunk.cellY = key.y;
            chunk.cellZ = key.z;

            m_geometryChunks.push_back(std::move(chunk));
            vertexRemaps.emplace_back();
        }
        else
        {
            chunkID = found->second;
        }

        MapGeometryChunk &chunk = m_geometryChunks[chunkID];
        auto &remap = vertexRemaps[chunkID];

        batch.chunkIndex = chunkID;
        batch.chunkStartIndex = static_cast<UINT>(chunk.indices.size());
        chunk.batchIndices.push_back(batchID);

        const size_t begin = batch.startIndex;
        const size_t end = begin + batch.indexCount;

        if (end > batchIndices.size())
        {
            throw std::runtime_error(
                "Chunk construction: invalid batch index range");
        }

        for (size_t i = begin; i < end; ++i)
        {
            const UINT globalIndex = batchIndices[i];

            if (globalIndex >= m_allVertices.size())
            {
                throw std::runtime_error(
                    "Chunk construction: invalid vertex index");
            }

            auto it = remap.find(globalIndex);

            UINT localIndex;

            if (it == remap.end())
            {
                localIndex = static_cast<UINT>(
                    chunk.vertices.size());

                chunk.vertices.push_back(
                    m_allVertices[globalIndex]);

                remap.emplace(globalIndex, localIndex);
            }
            else
            {
                localIndex = it->second;
            }

            chunk.indices.push_back(localIndex);
            ++copiedIndices;
        }
    }

    // Calculate actual chunk bounds from copied vertices.
    for (auto &chunk : m_geometryChunks)
    {
        if (chunk.vertices.empty())
            continue;

        DirectX::XMFLOAT3 minP{
            FLT_MAX, FLT_MAX, FLT_MAX};

        DirectX::XMFLOAT3 maxP{
            -FLT_MAX, -FLT_MAX, -FLT_MAX};

        for (const auto &vertex : chunk.vertices)
        {
            const auto &p = vertex.pos;

            minP.x = min(minP.x, p.x);
            minP.y = min(minP.y, p.y);
            minP.z = min(minP.z, p.z);

            maxP.x = max(maxP.x, p.x);
            maxP.y = max(maxP.y, p.y);
            maxP.z = max(maxP.z, p.z);
        }

        DirectX::BoundingBox::CreateFromPoints(
            chunk.bounds,
            DirectX::XMLoadFloat3(&minP),
            DirectX::XMLoadFloat3(&maxP));
    }

    if (copiedIndices != batchIndices.size())
    {
        throw std::runtime_error(
            "Chunk construction: index count mismatch");
    }

    uint64_t chunkVertices = 0;
    uint64_t chunkIndices = 0;

    for (const auto &chunk : m_geometryChunks)
    {
        chunkVertices += chunk.vertices.size();
        chunkIndices += chunk.indices.size();
    }

    std::string report =
        "Geometry chunks:\n"
        "Chunks: " +
        std::to_string(m_geometryChunks.size()) +
        "\nChunk vertices: " +
        std::to_string(chunkVertices) +
        "\nChunk indices: " +
        std::to_string(chunkIndices) +
        "\nChunk triangles: " +
        std::to_string(chunkIndices / 3) +
        "\n";

    OutputDebugStringA(report.c_str());
}



void MapLoader::BuildRenderBatches()
{
    m_renderBatches.clear();
    m_batchBVHNodes.clear();
    m_batchBVHIndices.clear();
    m_batchIndexBuffer.Reset();

    if (!m_batchDevice || m_allIndices.empty() || m_allVertices.empty())
        return;

    constexpr float CELL_SIZE = 128.0f;
    constexpr UINT MAX_BATCH_INDICES = 150000;

    // ---------------------------------------------------------
    // Material identity
    // ---------------------------------------------------------

    struct MaterialKey
    {
        ID3D11ShaderResourceView *diffuse = nullptr;
        ID3D11ShaderResourceView *normal = nullptr;
        ID3D11ShaderResourceView *detail = nullptr;
        ID3D11ShaderResourceView *normalDetail = nullptr;
        ID3D11ShaderResourceView *maps = nullptr;

        float materialType = 0.0f;
        float hasDiffuseTexture = 0.0f;
        float hasDetailTexture = 0.0f;
        float hasDiffuseColor = 0.0f;

        bool operator<(const MaterialKey &other) const
        {
            const std::less<ID3D11ShaderResourceView *> lessPtr;

            auto comparePtr = [&](auto *a, auto *b)
            {
                if (lessPtr(a, b))
                    return -1;
                if (lessPtr(b, a))
                    return 1;
                return 0;
            };

            int result = comparePtr(diffuse, other.diffuse);
            if (result != 0)
                return result < 0;

            result = comparePtr(normal, other.normal);
            if (result != 0)
                return result < 0;

            result = comparePtr(detail, other.detail);
            if (result != 0)
                return result < 0;

            result = comparePtr(normalDetail, other.normalDetail);
            if (result != 0)
                return result < 0;

            result = comparePtr(maps, other.maps);
            if (result != 0)
                return result < 0;

            return std::tie(
                       materialType,
                       hasDiffuseTexture,
                       hasDetailTexture,
                       hasDiffuseColor) <
                   std::tie(
                       other.materialType,
                       other.hasDiffuseTexture,
                       other.hasDetailTexture,
                       other.hasDiffuseColor);
        }
    };

    struct BatchKey
    {
        int x = 0;
        int y = 0;
        int z = 0;

        MaterialKey material;

        bool operator<(const BatchKey &other) const
        {
            return std::tie(x, y, z, material) <
                   std::tie(other.x, other.y, other.z,
                            other.material);
        }
    };

    // ---------------------------------------------------------
    // Triangle groups
    // ---------------------------------------------------------

    struct TriangleGroup
    {
        std::vector<UINT> indices;
        uint32_t representativeSubset = 0;
    };

    std::map<BatchKey, TriangleGroup> groups;

    uint64_t inputTriangles = 0;

    // ---------------------------------------------------------
    // Partition triangles into spatial cells
    // ---------------------------------------------------------

    for (uint32_t s = 0; s < m_mapSubsets.size(); ++s)
    {
        const auto &subset = m_mapSubsets[s];

        if (subset.indexCount == 0 || !subset.resolvedMaterial)
            continue;

        const MaterialData &material = *subset.resolvedMaterial;
        const SharedMaterial &gpu = material.gpuMaterial;

        ID3D11ShaderResourceView *diffuseSRV = nullptr;

        if (material.mapTextures.diffuse)
        {
            diffuseSRV = material.mapTextures.diffuse.Get();
        }
        else if (subset.materialIndex < m_materialSRVs.size())
        {
            diffuseSRV =
                m_materialSRVs[subset.materialIndex].Get();
        }

        MaterialKey materialKey{};

        materialKey.diffuse = diffuseSRV;
        materialKey.normal = material.mapTextures.normal.Get();
        materialKey.detail = material.mapTextures.detail.Get();
        materialKey.normalDetail =
            material.mapTextures.normalDetail.Get();
        materialKey.maps = material.mapTextures.maps.Get();

        materialKey.materialType =
            subset.materialType != MaterialType::MATERIAL_DEFAULT
                ? static_cast<float>(subset.materialType)
                : gpu.materialType;

        materialKey.hasDiffuseTexture = gpu.hasDiffuseTexture;
        materialKey.hasDetailTexture = gpu.hasDetailTexture;
        materialKey.hasDiffuseColor = gpu.hasDiffuseColor;

        const size_t begin = subset.startIndex;
        const size_t end = begin + subset.indexCount;

        if (end > m_allIndices.size())
        {
            throw std::runtime_error(
                "Spatial batching: invalid subset index range");
        }

        if (subset.indexCount % 3 != 0)
        {
            throw std::runtime_error(
                "Spatial batching: non-triangle index count");
        }

        for (size_t i = begin; i < end; i += 3)
        {
            const UINT i0 = m_allIndices[i];
            const UINT i1 = m_allIndices[i + 1];
            const UINT i2 = m_allIndices[i + 2];

            if (i0 >= m_allVertices.size() ||
                i1 >= m_allVertices.size() ||
                i2 >= m_allVertices.size())
            {
                throw std::runtime_error(
                    "Spatial batching: invalid vertex index");
            }

            const auto &p0 = m_allVertices[i0].pos;
            const auto &p1 = m_allVertices[i1].pos;
            const auto &p2 = m_allVertices[i2].pos;

            // Triangle centroid, not mesh center.
            const float cx = (p0.x + p1.x + p2.x) / 3.0f;
            const float cy = (p0.y + p1.y + p2.y) / 3.0f;
            const float cz = (p0.z + p1.z + p2.z) / 3.0f;

            BatchKey key{};

            key.x = static_cast<int>(std::floor(cx / CELL_SIZE));
            key.y = static_cast<int>(std::floor(cy / CELL_SIZE));
            key.z = static_cast<int>(std::floor(cz / CELL_SIZE));

            key.material = materialKey;

            auto result = groups.try_emplace(key);
            TriangleGroup &group = result.first->second;

            if (result.second)
                group.representativeSubset = s;

            group.indices.push_back(i0);
            group.indices.push_back(i1);
            group.indices.push_back(i2);

            ++inputTriangles;
        }
    }

    // ---------------------------------------------------------
    // Build contiguous index ranges and tight batch bounds
    // ---------------------------------------------------------

    std::vector<UINT> batchIndices;
    batchIndices.reserve(m_allIndices.size());

    for (const auto &entry : groups)
    {
        const TriangleGroup &group = entry.second;

        if (group.indices.empty())
            continue;

        const MapMeshSubset &representative =
            m_mapSubsets[group.representativeSubset];

        size_t cursor = 0;

        while (cursor < group.indices.size())
        {
            const size_t remaining = group.indices.size() - cursor;

            const size_t count = min(
                remaining,
                static_cast<size_t>(MAX_BATCH_INDICES));

            MapRenderBatch batch{};

            batch.cellX = entry.first.x;
            batch.cellY = entry.first.y;
            batch.cellZ = entry.first.z;

            batch.startIndex =
                static_cast<UINT>(batchIndices.size());

            batch.indexCount = static_cast<UINT>(count);

            batch.resolvedMaterial =
                representative.resolvedMaterial;

            batch.materialIndex =
                representative.materialIndex;

            batch.materialType =
                representative.materialType;

            DirectX::XMFLOAT3 minP{
                FLT_MAX, FLT_MAX, FLT_MAX};

            DirectX::XMFLOAT3 maxP{
                -FLT_MAX, -FLT_MAX, -FLT_MAX};

            // Copy indices and calculate bounds from actual
            // vertices, including triangle boundary crossings.
            for (size_t j = cursor; j < cursor + count; ++j)
            {
                const UINT vertexIndex = group.indices[j];

                batchIndices.push_back(vertexIndex);

                const auto &p = m_allVertices[vertexIndex].pos;

                minP.x = min(minP.x, p.x);
                minP.y = min(minP.y, p.y);
                minP.z = min(minP.z, p.z);

                maxP.x = max(maxP.x, p.x);
                maxP.y = max(maxP.y, p.y);
                maxP.z = max(maxP.z, p.z);
            }

            DirectX::BoundingBox::CreateFromPoints(
                batch.bounds,
                DirectX::XMLoadFloat3(&minP),
                DirectX::XMLoadFloat3(&maxP));

            m_renderBatches.push_back(batch);

            cursor += count;
        }
    }

    if (batchIndices.empty())
        return;

    BuildGeometryChunks(batchIndices);

    // ---------------------------------------------------------
    // Validate geometry preservation
    // ---------------------------------------------------------

    if (batchIndices.size() != inputTriangles * 3)
    {
        throw std::runtime_error(
            "Spatial batching: triangle count mismatch");
    }

    // ---------------------------------------------------------
    // Upload static batch index buffer
    // ---------------------------------------------------------

    const size_t bytes =
        batchIndices.size() * sizeof(UINT);

   /* if (bytes > std::numeric_limits<UINT>::std::max())
    {
        throw std::runtime_error(
            "Spatial batching: index buffer exceeds UINT limit");
    }

    */

    D3D11_BUFFER_DESC desc{};

    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = static_cast<UINT>(bytes);
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA data{};
    data.pSysMem = batchIndices.data();

    HRESULT hr = m_batchDevice->CreateBuffer(
        &desc,
        &data,
        m_batchIndexBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        throw std::runtime_error(
            "Spatial batching: CreateBuffer failed");
    }

    // ---------------------------------------------------------
    // Build existing BVH
    // ---------------------------------------------------------

    BuildBatchBVH();

    // ---------------------------------------------------------
    // Diagnostics
    // ---------------------------------------------------------

    std::string report =
        "Spatial batching:\n"
        "Input triangles: " +
        std::to_string(inputTriangles) +
        "\nOutput triangles: " +
        std::to_string(batchIndices.size() / 3) +
        "\nSpatial/material groups: " +
        std::to_string(groups.size()) +
        "\nRender batches: " +
        std::to_string(m_renderBatches.size()) +
        "\n";

    OutputDebugStringA(report.c_str());
}



void MapLoader::DrawShadowSubset(
    ID3D11DeviceContext *context,
    const MapMeshSubset &subset,
    ID3D11PixelShader *shadowAlphaPS)
{
    if (subset.materialType == MaterialType::MATERIAL_TREE)
    {
        ID3D11ShaderResourceView *diffuseSRV = nullptr;

        if (subset.resolvedMaterial &&
            subset.resolvedMaterial->mapTextures.diffuse)
        {
            diffuseSRV =
                subset.resolvedMaterial->mapTextures.diffuse.Get();
        }
        else if (subset.materialIndex < m_materialSRVs.size())
        {
            diffuseSRV =
                m_materialSRVs[subset.materialIndex].Get();
        }

        if (diffuseSRV)
        {
            context->PSSetShader(shadowAlphaPS, nullptr, 0);
            context->PSSetShaderResources(0, 1, &diffuseSRV);
            context->PSSetSamplers(
                0, 1, m_samplerState.GetAddressOf());
        }
        else
        {
            // No alpha texture: preserve opaque shadow behavior.
            context->PSSetShader(nullptr, nullptr, 0);
        }
    }
    else
    {
        // Existing fast depth-only path.
        context->PSSetShader(nullptr, nullptr, 0);
    }

    context->DrawIndexed(
        subset.indexCount,
        subset.startIndex,
        0);
}


void MapLoader::BuildBatchBVH()
{
    m_batchBVHNodes.clear();
    m_batchBVHIndices.clear();

    for (uint32_t i = 0; i < m_renderBatches.size(); ++i)
        m_batchBVHIndices.push_back(i);

    if (m_batchBVHIndices.empty())
        return;

    m_batchBVHNodes.reserve(m_batchBVHIndices.size() * 2);

    BuildBatchBVHNode(
        0,
        static_cast<uint32_t>(m_batchBVHIndices.size()));
}

int MapLoader::BuildBatchBVHNode(
    uint32_t first,
    uint32_t count)
{
    const int nodeIndex =
        static_cast<int>(m_batchBVHNodes.size());

    m_batchBVHNodes.emplace_back();

    DirectX::XMFLOAT3 minP{FLT_MAX, FLT_MAX, FLT_MAX};
    DirectX::XMFLOAT3 maxP{-FLT_MAX, -FLT_MAX, -FLT_MAX};

    uint64_t totalIndices = 0;

    for (uint32_t i = first; i < first + count; ++i)
    {
        const auto &batch =
            m_renderBatches[m_batchBVHIndices[i]];

        const auto &c = batch.bounds.Center;
        const auto &e = batch.bounds.Extents;

        totalIndices += batch.indexCount;

        minP.x = min(minP.x, c.x - e.x);
        minP.y = min(minP.y, c.y - e.y);
        minP.z = min(minP.z, c.z - e.z);

        maxP.x = max(maxP.x, c.x + e.x);
        maxP.y = max(maxP.y, c.y + e.y);
        maxP.z = max(maxP.z, c.z + e.z);
    }

    BVHNode &node = m_batchBVHNodes[nodeIndex];

    DirectX::BoundingBox::CreateFromPoints(
        node.bounds,
        DirectX::XMLoadFloat3(&minP),
        DirectX::XMLoadFloat3(&maxP));

    node.first = first;
    node.count = count;
    node.totalIndices = totalIndices;

    constexpr uint32_t LEAF_SIZE = 8;

    if (count <= LEAF_SIZE)
        return nodeIndex;

    const float sx = maxP.x - minP.x;
    const float sy = maxP.y - minP.y;
    const float sz = maxP.z - minP.z;

    int axis = 0;

    if (sy > sx && sy > sz)
        axis = 1;
    else if (sz > sx)
        axis = 2;

    const uint32_t middle = first + count / 2;

    auto centerAxis = [&](uint32_t index)
    {
        const auto &c = m_renderBatches[index].bounds.Center;

        if (axis == 0)
            return c.x;
        if (axis == 1)
            return c.y;
        return c.z;
    };

    std::nth_element(
        m_batchBVHIndices.begin() + first,
        m_batchBVHIndices.begin() + middle,
        m_batchBVHIndices.begin() + first + count,
        [&](uint32_t a, uint32_t b)
        {
            return centerAxis(a) < centerAxis(b);
        });

    const int left =
        BuildBatchBVHNode(first, middle - first);

    const int right =
        BuildBatchBVHNode(
            middle,
            count - (middle - first));

    // Reacquire reference after recursive vector growth.
    m_batchBVHNodes[nodeIndex].left = left;
    m_batchBVHNodes[nodeIndex].right = right;

    return nodeIndex;
}

void MapLoader::CollectVisibleBatchBVH(
    int nodeIndex,
    const DirectX::BoundingFrustum &frustum,
    std::vector<uint32_t> &visible)
{
    const BVHNode &node = m_batchBVHNodes[nodeIndex];

    if (!frustum.Intersects(node.bounds))
    {
        m_stats.m_culledDrawCalls += node.count;
        return;
    }

    if (!node.IsLeaf())
    {
        CollectVisibleBatchBVH(node.left, frustum, visible);
        CollectVisibleBatchBVH(node.right, frustum, visible);
        return;
    }

    for (uint32_t i = node.first;
         i < node.first + node.count;
         ++i)
    {
        const uint32_t batchIndex = m_batchBVHIndices[i];
        const auto &batch = m_renderBatches[batchIndex];

        if (!frustum.Intersects(batch.bounds))
        {
            ++m_stats.m_culledDrawCalls;
            continue;
        }

        visible.push_back(batchIndex);
    }
}



void MapLoader::CollectVisibleBVH(
    int nodeIndex,
    const DirectX::BoundingFrustum& frustum,
    std::vector<uint32_t>& visible)
{
    const BVHNode& node = m_bvhNodes[nodeIndex];

    if (!frustum.Intersects(node.bounds))
    {
        m_stats.m_culledDrawCalls += node.count;
        return;
    }

    if (!node.IsLeaf())
    {
        CollectVisibleBVH(node.left, frustum, visible);
        CollectVisibleBVH(node.right, frustum, visible);
        return;
    }

    for (uint32_t j = node.first;
        j < node.first + node.count;
        ++j)
    {
        const uint32_t subsetIndex = m_bvhIndices[j];
        const auto& subset = m_mapSubsets[subsetIndex];

        if (!frustum.Intersects(subset.bounds))
        {
            m_stats.m_culledDrawCalls++;
            continue;
        }

        visible.push_back(subsetIndex);
    }
}

void MapLoader::BuildBVH()
{
    m_bvhNodes.clear();
    m_bvhIndices.clear();

    for (uint32_t i = 0; i < m_mapSubsets.size(); ++i)
    {
        if (m_mapSubsets[i].indexCount > 0)
            m_bvhIndices.push_back(i);
    }

    if (m_bvhIndices.empty())
        return;

    m_bvhNodes.reserve(m_bvhIndices.size() * 2);

    BuildBVHNode(
        0,
        static_cast<uint32_t>(m_bvhIndices.size())
    );

    //OutputDebugStringA(("BVH built: " + std::to_string(m_bvhNodes.size()) + " nodes, " + std::to_string(m_bvhIndices.size()) + " subsets\n").c_str());
}

void MapLoader::DrawShadowBVHNode(
    int nodeIndex,
    ID3D11DeviceContext* context,
    const DirectX::BoundingFrustum& frustum,
    ID3D11PixelShader *shadowAlphaPS)
{
    const BVHNode& node = m_bvhNodes[nodeIndex];

    // Entire branch rejected.
    if (!frustum.Intersects(node.bounds))
    {
        m_stats.m_shadowDrawCallsCulled += node.count;
        m_stats.culledIndices += node.totalIndices;
        return;
    }

    // Internal node.
    if (!node.IsLeaf())
    {
        DrawShadowBVHNode(node.left, context, frustum, shadowAlphaPS);
        DrawShadowBVHNode(node.right, context, frustum, shadowAlphaPS);
        return;
    }

    // Leaf: test individual subsets.
    for (uint32_t i = node.first;
        i < node.first + node.count;
        ++i)
    {
        const auto& subset =
            m_mapSubsets[m_bvhIndices[i]];

        if (!frustum.Intersects(subset.bounds))
        {
            m_stats.m_shadowDrawCallsCulled++;
            m_stats.culledIndices += subset.indexCount;
            continue;
        }

     DrawShadowSubset(
            context,
            subset,
            shadowAlphaPS);
        

        m_stats.m_shadowdrawCalls++;
        m_stats.renderedIndices += subset.indexCount;
    }
}

int MapLoader::BuildBVHNode(uint32_t first, uint32_t count)
{
    const int nodeIndex =
        static_cast<int>(m_bvhNodes.size());

    m_bvhNodes.emplace_back();

    DirectX::XMFLOAT3 minP{
        FLT_MAX, FLT_MAX, FLT_MAX
    };

    DirectX::XMFLOAT3 maxP{
        -FLT_MAX, -FLT_MAX, -FLT_MAX
    };
    uint64_t totalIndices = 0;

    for (uint32_t i = first; i < first + count; ++i)
    {
        const auto& subset =
            m_mapSubsets[m_bvhIndices[i]];

        const auto& bounds = subset.bounds;

        totalIndices += subset.indexCount;
        minP.x = min(minP.x, bounds.Center.x - bounds.Extents.x);
        minP.y = min(minP.y, bounds.Center.y - bounds.Extents.y);
        minP.z = min(minP.z, bounds.Center.z - bounds.Extents.z);

        maxP.x = max(maxP.x, bounds.Center.x + bounds.Extents.x);
        maxP.y = max(maxP.y, bounds.Center.y + bounds.Extents.y);
        maxP.z = max(maxP.z, bounds.Center.z + bounds.Extents.z);
    }

    DirectX::BoundingBox nodeBounds;

    DirectX::BoundingBox::CreateFromPoints(
        nodeBounds,
        DirectX::XMLoadFloat3(&minP),
        DirectX::XMLoadFloat3(&maxP)
    );

    m_bvhNodes[nodeIndex].bounds = nodeBounds;
    m_bvhNodes[nodeIndex].first = first;
    m_bvhNodes[nodeIndex].count = count;
    m_bvhNodes[nodeIndex].totalIndices = totalIndices;

    constexpr uint32_t leafSize = 16;

    if (count <= leafSize)
        return nodeIndex;

    const float sizeX = maxP.x - minP.x;
    const float sizeY = maxP.y - minP.y;
    const float sizeZ = maxP.z - minP.z;

    int axis = 0;

    if (sizeY > sizeX && sizeY > sizeZ)
        axis = 1;
    else if (sizeZ > sizeX)
        axis = 2;

    const uint32_t middle = first + count / 2;

    auto centerAxis = [&](uint32_t subsetIndex)
    {
        const auto& c = m_mapSubsets[subsetIndex].bounds.Center;

        if (axis == 0) return c.x;
        if (axis == 1) return c.y;
        return c.z;
    };

    std::nth_element(
        m_bvhIndices.begin() + first,
        m_bvhIndices.begin() + middle,
        m_bvhIndices.begin() + first + count,
        [&](uint32_t a, uint32_t b)
        {
            return centerAxis(a) < centerAxis(b);
        }
    );

    const int left =
        BuildBVHNode(first, middle - first);

    const int right =
        BuildBVHNode(middle, count - (middle - first));

    m_bvhNodes[nodeIndex].left = left;
    m_bvhNodes[nodeIndex].right = right;

    return nodeIndex;
}


void MapLoader::UpdateVisibleLights(
    ID3D11DeviceContext* context,
    ID3D11Buffer* lampStructuredBuffer,
    ID3D11Buffer* lampInfoBuffer,
    const DirectX::BoundingFrustum& frustum,
    Camera* cam
   )
{
    m_visibleLampLights.clear();

    const float maxLightDistance = 1000.0f;
    const float maxLightDistanceSq = maxLightDistance * maxLightDistance;

    

    for (const auto& light : m_lampLights)
    {
        XMVECTOR lightPos = XMLoadFloat3(&light.position);
        XMVECTOR cameraPos = XMLoadFloat3(&cam->position);

        XMVECTOR delta = XMVectorSubtract(lightPos, cameraPos);
        float distanceSq = XMVectorGetX(XMVector3LengthSq(delta));


        if (distanceSq < maxLightDistanceSq)
        {
            const DirectX::BoundingSphere bounds(
                light.position,
                light.radius);     

             if (frustum.Intersects(bounds))
            {
                m_visibleLampLights.push_back(light);
            }
        }
    }

    const size_t activeCount =
        m_lightsEnabled
        ? min(m_visibleLampLights.size(), size_t{ 1200 })
        : 0;

    m_stats.activelampCount =
        static_cast<uint32_t>(activeCount);

    if (lampStructuredBuffer && activeCount > 0)
    {
        D3D11_MAPPED_SUBRESOURCE mapped{};

        if (SUCCEEDED(context->Map(
            lampStructuredBuffer,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mapped)))
        {
            memcpy(
                mapped.pData,
                m_visibleLampLights.data(),
                sizeof(LampLight) * activeCount);

            context->Unmap(lampStructuredBuffer, 0);
        }
    }

    if (lampInfoBuffer)
    {
        LampInfo info{};
        info.lampCount = static_cast<int>(activeCount);

        D3D11_MAPPED_SUBRESOURCE mapped{};

        if (SUCCEEDED(context->Map(
            lampInfoBuffer,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mapped)))
        {
            memcpy(mapped.pData, &info, sizeof(info));
            context->Unmap(lampInfoBuffer, 0);
        }
    }
}


void MapLoader::DrawShadow(
    ID3D11DeviceContext *context,
    ID3D11Buffer *cbb,
    const SharedSceneData &engineSceneData,
    const DirectX::BoundingFrustum &frustum,
    ID3D11DepthStencilState *depthWriteOn,
    ID3D11PixelShader *shadowAlphaPS)
{
    if (m_geometryChunks.empty() || m_renderBatches.empty())
        return;

    SharedSceneData sceneData = engineSceneData;

    m_stats.m_shadowDrawCallsCulled = 0;
    m_stats.m_shadowdrawCalls = 0;
    m_stats.culledIndices = 0;
    m_stats.renderedIndices = 0;
    m_stats.zeroIndexSubsets = 0;

    context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    context->OMSetDepthStencilState(depthWriteOn, 0);

    UINT stride = sizeof(SharedVertex);
    UINT offset = 0;

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->VSSetConstantBuffers(0, 1, &cbb);

    D3D11_MAPPED_SUBRESOURCE mapped{};

    HRESULT hr = context->Map(
        cbb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

    if (FAILED(hr))
        return;

    memcpy(mapped.pData, &sceneData, sizeof(sceneData));
    context->Unmap(cbb, 0);

    static bool enableFrustumCulling = true;
    static bool hWasPressed = false;

    const bool hPressed =
        (GetAsyncKeyState('H') & 0x8000) != 0;

    if (hPressed && !hWasPressed)
    {
        enableFrustumCulling = !enableFrustumCulling;

        OutputDebugStringA(
            enableFrustumCulling
                ? "Shadow Frustum culling: ON\n"
                : "Shadow Frustum culling: OFF\n");
    }

    hWasPressed = hPressed;

    const auto start =
        std::chrono::high_resolution_clock::now();

    std::vector<uint32_t> visibleBatches;

    if (enableFrustumCulling && !m_batchBVHNodes.empty())
    {
        CollectVisibleBatchBVH(
            0, frustum, visibleBatches);
    }
    else
    {
        visibleBatches.reserve(m_renderBatches.size());

        for (uint32_t i = 0; i < m_renderBatches.size(); ++i)
            visibleBatches.push_back(i);
    }

    for (uint32_t i : visibleBatches)
    {
        const auto &batch = m_renderBatches[i];

        if (batch.indexCount == 0)
        {
            ++m_stats.zeroIndexSubsets;
            continue;
        }

        if (batch.chunkIndex >= m_geometryChunks.size())
            continue;

        const auto &chunk = m_geometryChunks[batch.chunkIndex];

        if (!chunk.resident ||
            !chunk.vertexBuffer ||
            !chunk.indexBuffer)
            continue;

        ID3D11Buffer *vb = chunk.vertexBuffer.Get();

        context->IASetVertexBuffers(
            0, 1, &vb, &stride, &offset);

        context->IASetIndexBuffer(
            chunk.indexBuffer.Get(),
            DXGI_FORMAT_R32_UINT,
            0);

        ID3D11ShaderResourceView *diffuseSRV = nullptr;

        if (batch.materialType == MaterialType::MATERIAL_TREE)
        {
            if (batch.resolvedMaterial &&
                batch.resolvedMaterial->mapTextures.diffuse)
            {
                diffuseSRV =
                    batch.resolvedMaterial->mapTextures.diffuse.Get();
            }
            else if (batch.materialIndex < m_materialSRVs.size())
            {
                diffuseSRV =
                    m_materialSRVs[batch.materialIndex].Get();
            }
        }

        if (batch.materialType == MaterialType::MATERIAL_TREE &&
            diffuseSRV &&
            shadowAlphaPS)
        {
            context->PSSetShader(shadowAlphaPS, nullptr, 0);
            context->PSSetShaderResources(0, 1, &diffuseSRV);
            context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
        }
        else
        {
            context->PSSetShader(nullptr, nullptr, 0);
        }

        context->DrawIndexed(
            batch.indexCount,
            batch.chunkStartIndex,
            0);

        ++m_stats.m_shadowdrawCalls;
        m_stats.renderedIndices += batch.indexCount;
    }

    const auto end =
        std::chrono::high_resolution_clock::now();

    m_stats.shadowCpuMs =
        std::chrono::duration<double, std::milli>(
            end - start)
            .count();
}



void MapLoader::Draw(
    ID3D11DeviceContext *context,
    ID3D11Buffer *cbb,
    ID3D11Buffer *lampInfoBuffer,
    ID3D11ShaderResourceView *lampLightsSRV,
    const SharedSceneData &engineSceneData,
    const DirectX::BoundingFrustum &frustum,
    ID3D11DepthStencilState *depthWriteOn,
    const DirectX::XMFLOAT3 &cameraPosition)
{
    if (m_geometryChunks.empty() || m_renderBatches.empty())
        return;

    UpdateChunkResidency(cameraPosition);

    SharedSceneData drawData = engineSceneData;

    m_stats.m_drawCalls = 0;
    m_stats.m_culledDrawCalls = 0;

    context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    context->OMSetDepthStencilState(depthWriteOn, 0);

    UINT stride = sizeof(SharedVertex);
    UINT offset = 0;


    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->PSSetSamplers(
        0, 1, m_samplerState.GetAddressOf());

    context->VSSetConstantBuffers(0, 1, &cbb);
    context->PSSetConstantBuffers(0, 1, &cbb);

    // Track lighting toggle.
    static bool lWasPressed = false;
    const bool lPressed =
        (GetAsyncKeyState('L') & 0x8000) != 0;

    if (lPressed && !lWasPressed)
        m_lightsEnabled = !m_lightsEnabled;

    lWasPressed = lPressed;
    m_stats.lightsEnabled = m_lightsEnabled;

    if (m_lightsEnabled)
    {
        context->PSSetConstantBuffers(1, 1, &lampInfoBuffer);
        context->PSSetShaderResources(1, 1, &lampLightsSRV);
    }
    else
    {
        ID3D11Buffer *nullCB = nullptr;
        ID3D11ShaderResourceView *nullSRV = nullptr;

        context->PSSetConstantBuffers(1, 1, &nullCB);
        context->PSSetShaderResources(1, 1, &nullSRV);
    }

    m_stats.lampCount = static_cast<uint32_t>(
        min(m_lampLights.size(), size_t{1200}));

    // Frustum toggle.
    static bool enableFrustumCulling = true;
    static bool fWasPressed = false;

    const bool fPressed =
        (GetAsyncKeyState('F') & 0x8000) != 0;

    if (fPressed && !fWasPressed)
        enableFrustumCulling = !enableFrustumCulling;

    fWasPressed = fPressed;

    std::vector<uint32_t> visibleBatches;

    if (enableFrustumCulling && !m_batchBVHNodes.empty())
    {
        CollectVisibleBatchBVH(
            0, frustum, visibleBatches);

        std::sort(
            visibleBatches.begin(),
            visibleBatches.end());
    }
    else
    {
        visibleBatches.reserve(m_renderBatches.size());

        for (uint32_t i = 0; i < m_renderBatches.size(); ++i)
            visibleBatches.push_back(i);
    }

    
    //   context->PSSetShader(nullptr, nullptr, 0);

    // Bind the actual material resources for every batch.
    // No stale SRVs inherited from previous passes.
    for (uint32_t i : visibleBatches)
    {
        const auto &batch = m_renderBatches[i];

        if (batch.chunkIndex >= m_geometryChunks.size())
            continue;

        const auto &chunk =
            m_geometryChunks[batch.chunkIndex];

        if (!chunk.resident)
            continue;

        ID3D11Buffer *vb = chunk.vertexBuffer.Get();

        context->IASetVertexBuffers(
            0, 1, &vb, &stride, &offset);

        context->IASetIndexBuffer(
            chunk.indexBuffer.Get(),
            DXGI_FORMAT_R32_UINT,
            0);


        const MaterialData &material = *batch.resolvedMaterial;

        drawData.material = material.gpuMaterial;

        drawData.material.diffuseColor = {1, 1, 1};
        drawData.material.ambientColor = {0.3f, 0.3f, 0.3f};
        drawData.material.specularColor = {0.1f, 0.1f, 0.1f};
        drawData.material.specularPower = 32.0f;
        drawData.material.d = 1.0f;
        drawData.material.isMapMaterial = 1.0f;

        if (batch.materialType != MaterialType::MATERIAL_DEFAULT)
        {
            drawData.material.materialType =
                static_cast<float>(batch.materialType);
        }

        ID3D11ShaderResourceView *diffuseSRV = nullptr;

        if (material.mapTextures.diffuse)
        {
            diffuseSRV = material.mapTextures.diffuse.Get();
        }
        else if (batch.materialIndex < m_materialSRVs.size())
        {
            diffuseSRV =
                m_materialSRVs[batch.materialIndex].Get();
        }

        context->PSSetShaderResources(0, 1, &diffuseSRV);

        ID3D11ShaderResourceView *additionalSRVs[4] =
            {
                material.mapTextures.normal.Get(),
                material.mapTextures.detail.Get(),
                material.mapTextures.normalDetail.Get(),
                material.mapTextures.maps.Get()};

        context->PSSetShaderResources(
            8, 4, additionalSRVs);

        D3D11_MAPPED_SUBRESOURCE mapped{};

        HRESULT hr = context->Map(
            cbb,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mapped);

        if (FAILED(hr))
            continue;

        memcpy(mapped.pData, &drawData, sizeof(drawData));
        context->Unmap(cbb, 0);


        context->DrawIndexed(
            batch.indexCount,
            batch.chunkStartIndex,
            0);

        ++m_stats.m_drawCalls;
    }

    uint64_t visibleIndices = 0;
    uint64_t visibleTriangles = 0;

    for (uint32_t i : visibleBatches)
    {
        const auto &batch = m_renderBatches[i];

        if (!batch.resolvedMaterial || batch.indexCount == 0)
            continue;

        visibleIndices += batch.indexCount;
        visibleTriangles += batch.indexCount / 3;
    }

    static int logCounter = 0;

    if (++logCounter >= 300)
    {
        logCounter = 0;

        char buffer[256];

        sprintf_s(
            buffer,
            "Visible batches: %zu | Indices: %llu | Triangles: %llu\n",
            visibleBatches.size(),
            static_cast<unsigned long long>(visibleIndices),
            static_cast<unsigned long long>(visibleTriangles));

        OutputDebugStringA(buffer);
    }
}




void MapLoader::ResolveMaterials()
{
    assert(m_materialLoader != nullptr);

    for (size_t i = 0; i < m_mapSubsets.size(); ++i)
    {
        m_mapSubsets[i].resolvedMaterial =
            &m_materialLoader->ResolveMapMaterial(
                m_mapMaterialNames[i]);
    }

    BuildRenderBatches();
}


void MapLoader::ProcessNode(
    const aiNode* node,
    const aiScene* scene,
    const aiMatrix4x4& parentTransform)
{
    const aiMatrix4x4 worldTransform =
        parentTransform * node->mTransformation;

    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        const aiMesh* mesh =
            scene->mMeshes[node->mMeshes[i]];

        std::string meshName = mesh->mName.C_Str();

        std::transform(
            meshName.begin(),
            meshName.end(),
            meshName.begin(),
            [](unsigned char c)
            {
                return static_cast<char>(std::tolower(c));
            });

        if (meshName.find("bulb") != std::string::npos)
        {
            const DirectX::XMFLOAT3 localCenter =
                CalculateMeshCenter(mesh);

            const aiVector3D localPosition(
                localCenter.x,
                localCenter.y,
                localCenter.z);

            const aiVector3D worldPosition =
                worldTransform * localPosition;

            LampLight light{};

            light.position =
            {
                worldPosition.x,
                worldPosition.y,
                worldPosition.z
            };

            std::string msg =
                "Lamp world: " +
                std::to_string(worldPosition.x) + ", " +
                std::to_string(worldPosition.y) + ", " +
                std::to_string(worldPosition.z) + "\n";

            OutputDebugStringA(msg.c_str());

            light.radius = 30.0f;

            m_lampLights.push_back(light);

            const auto& p = m_lampLights.back().position;

            std::string msg2 =
                "Stored: " +
                std::to_string(p.x) + ", " +
                std::to_string(p.y) + ", " +
                std::to_string(p.z) + "\n";

            //OutputDebugStringA(msg2.c_str());
        }
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(
            node->mChildren[i],
            scene,
            worldTransform);
    }
}

void MapLoader::FindGridNodes(
    aiNode* node,
    const aiMatrix4x4& parentTransform)
{
    if (!node)
        return;

    // Resolve the node into world space.
    const aiMatrix4x4 worldTransform =
        parentTransform * node->mTransformation;

    std::string name = node->mName.C_Str();
    std::string lower = name;

    std::transform(
        lower.begin(),
        lower.end(),
        lower.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(std::tolower(c));
        });


    if (lower.find("grid") != std::string::npos || lower.find("timing") != std::string::npos)
    {
        aiVector3D scaling;
        aiQuaternion rotation;
        aiVector3D position;

        worldTransform.Decompose(
            scaling,
            rotation,
            position);

        OutputDebugStringA(
            ("FOUND GRID NODE: " + name +
                " POS=(" +
                std::to_string(position.x) + ", " +
                std::to_string(position.y) + ", " +
                std::to_string(position.z) + ")"
                " ROT=(" +
                std::to_string(rotation.x) + ", " +
                std::to_string(rotation.y) + ", " +
                std::to_string(rotation.z) + ", " +
                std::to_string(rotation.w) + ")" +
                "\n").c_str());

        m_markers.push_back(
            {
                name,

                position.x,
                position.y,
                position.z,

                rotation.x,
                rotation.y,
                rotation.z,
                rotation.w
            });
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        FindGridNodes(
            node->mChildren[i],
            worldTransform);
    }
}

DirectX::XMFLOAT3 MapLoader::CalculateMeshCenter(const aiMesh* mesh)
{
    DirectX::XMFLOAT3 center{ 0.0f, 0.0f, 0.0f };

    if (!mesh || mesh->mNumVertices == 0)
        return center;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        center.x += mesh->mVertices[i].x;
        center.y += mesh->mVertices[i].y;
        center.z += mesh->mVertices[i].z;
    }

    const float inverseCount = 1.0f / static_cast<float>(mesh->mNumVertices);

    center.x *= inverseCount;
    center.y *= inverseCount;
    center.z *= inverseCount;
    return center;
}



bool MapLoader::LoadWorld(const std::string& filename,
    ID3D11Device* device,
    ID3D11DeviceContext* context)
{

    m_batchDevice = device;

    m_renderBatches.clear();
    m_batchBVHNodes.clear();
    m_batchBVHIndices.clear();
    m_batchIndexBuffer.Reset();


    Assimp::Importer importer;
    const auto start = std::chrono::steady_clock::now();
    const aiScene* scene = importer.ReadFile(
        filename,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FlipUVs |
        aiProcess_MakeLeftHanded |
        aiProcess_GenNormals
    );

    if (!scene || !scene->HasMeshes())
        return false;

    OutputDebugStringA(
        ("Assimp materials: " +
         std::to_string(scene->mNumMaterials) + "\n")
            .c_str());

    m_markers.clear();
    aiMatrix4x4 identity;

    FindGridNodes(scene->mRootNode, identity);

    m_allVertices.clear();
    m_allIndices.clear();
    m_collisionIndices.clear();
    m_mapSubsets.clear();
    m_mapMaterialNames.clear();
     
    size_t lastSlash = filename.find_last_of("\\/");
    std::string directory =
        (lastSlash != std::string::npos) ?  
        filename.substr(0, lastSlash + 1) : "";


    m_stats.m_totalVertices = 0;
    m_stats.m_totalTriangles = 0;

    m_stats.m_meshes = scene->mNumMeshes;

    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        aiString texPath;
        ID3D11ShaderResourceView* srv = nullptr;

        if (scene->mMaterials[i]->GetTexture(
            aiTextureType_DIFFUSE,
            0,
            &texPath) == AI_SUCCESS)
        {
            std::string textureName = texPath.C_Str();

            std::string file =
                textureName.substr(
                    textureName.find_last_of("/\\") + 1);

            std::wstring wpath(
                directory.begin(),
                directory.end());

            wpath += std::wstring(
                file.begin(),
                file.end());

            if (m_texMgr)
                srv = m_texMgr->GetTexture(
                    wpath,
                    context);
        }

        m_materialSRVs.push_back(srv);
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];


        m_stats.m_totalVertices += mesh->mNumVertices;
        m_stats.m_totalTriangles += mesh->mNumFaces; // assuming triangulation

        UINT baseVertex = (UINT)m_allVertices.size();

        MapMeshSubset  subset{};
        subset.startIndex = (UINT)m_allIndices.size();
        subset.indexCount = 0;

        std::string materialName;

        if (mesh->mMaterialIndex < scene->mNumMaterials)
        {
            aiMaterial* aiMat =
                scene->mMaterials[mesh->mMaterialIndex];


            subset.materialIndex = mesh->mMaterialIndex;

            aiString aiMatName;

            if (aiMat->Get(AI_MATKEY_NAME, aiMatName) == AI_SUCCESS)
            {
                materialName = aiMatName.C_Str();
            }
        }

        std::string meshName = mesh->mName.C_Str();

        std::string n = meshName;
        std::transform(n.begin(), n.end(), n.begin(), ::tolower);


        // ---------------------------------------------------------
        // Map material classification
        // ---------------------------------------------------------

        bool noCollision = false;

        if (n.find("nocoll_") != std::string::npos)
        {
            noCollision = true;
        }

        if (n.find("road") != std::string::npos ||
            n.find("asphalt") != std::string::npos ||
            n.find("grass") != std::string::npos)
        {
            subset.materialType =
                MaterialType::MATERIAL_ASPHALT;
        }


        if (n.find("tree") != std::string::npos ||
            n.find("kstree") != std::string::npos ||
            n.find("bush") != std::string::npos)
        {
            subset.materialType =
                MaterialType::MATERIAL_TREE;
        }


        if (n.find("bulb") != std::string::npos ||
            n.find("tunnel_lamps") != std::string::npos ||
            n.find("lightemitter") != std::string::npos ||
            n.find("streetlamp_sub1") != std::string::npos)
        {
            subset.materialType =
                MaterialType::MATERIAL_LAMP;
        }

        if (meshName == "GRID")
        {
            aiVector3D minP(FLT_MAX, FLT_MAX, FLT_MAX);
            aiVector3D maxP(-FLT_MAX, -FLT_MAX, -FLT_MAX);

            for (unsigned int v = 0; v < mesh->mNumVertices; v++)
            {
                aiVector3D p = mesh->mVertices[v];

                minP.x = min(minP.x, p.x);
                minP.y = min(minP.y, p.y);
                minP.z = min(minP.z, p.z);

                maxP.x = max(maxP.x, p.x);
                maxP.y = max(maxP.y, p.y);
                maxP.z = max(maxP.z, p.z);
            }

            aiVector3D center;
            center.x = (minP.x + maxP.x) * 0.5f;
            center.y = (minP.y + maxP.y) * 0.5f;
            center.z = (minP.z + maxP.z) * 0.5f;

            std::string msg =
                "GRID mesh center: X=" + std::to_string(center.x) +
                " Y=" + std::to_string(center.y) +
                " Z=" + std::to_string(center.z) + "\n";

            //OutputDebugStringA(msg.c_str());
        }


        aiVector3D minP(FLT_MAX, FLT_MAX, FLT_MAX);
        aiVector3D maxP(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        if (mesh->mNumVertices > 0)
        {
            for (unsigned int v = 0; v < mesh->mNumVertices; v++)
            {
                SharedVertex vert{};
                aiVector3D p = mesh->mVertices[v];

                minP.x = min(minP.x, p.x);
                minP.y = min(minP.y, p.y);
                minP.z = min(minP.z, p.z);

                maxP.x = max(maxP.x, p.x);
                maxP.y = max(maxP.y, p.y);
                maxP.z = max(maxP.z, p.z);


                vert.pos = { p.x, p.y, p.z };

                if (mesh->HasNormals())
                {
                    aiVector3D n = mesh->mNormals[v];
                    vert.normal = { n.x, n.y, n.z };
                }
                else
                    vert.normal = { 0,1,0 };

                if (mesh->HasTextureCoords(0))
                    vert.texCoord = { mesh->mTextureCoords[0][v].x,
                                      mesh->mTextureCoords[0][v].y };
                else
                    vert.texCoord = { 0,0 };

                vert.color = { 1,1,1,1 };
                m_allVertices.push_back(vert);
            }

            DirectX::XMFLOAT3 minPoint =
            {
                minP.x,
                minP.y,
                minP.z
            };

            DirectX::XMFLOAT3 maxPoint =
            {
                maxP.x,
                maxP.y,
                maxP.z
            };

            DirectX::BoundingBox::CreateFromPoints(
                subset.bounds,
                DirectX::XMLoadFloat3(&minPoint),
                DirectX::XMLoadFloat3(&maxPoint)
            );


            std::string msg =
                "Bounds for " + meshName +
                " | Center: " +
                std::to_string(subset.bounds.Center.x) + ", " +
                std::to_string(subset.bounds.Center.y) + ", " +
                std::to_string(subset.bounds.Center.z) +
                " | Extents: " +
                std::to_string(subset.bounds.Extents.x) + ", " +
                std::to_string(subset.bounds.Extents.y) + ", " +
                std::to_string(subset.bounds.Extents.z) +
                "\n";

            //  OutputDebugStringA(msg.c_str());
        }

        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            const aiFace& face = mesh->mFaces[f];
            if (face.mNumIndices != 3) continue;

            UINT i0 = face.mIndices[0] + baseVertex;
            UINT i1 = face.mIndices[1] + baseVertex;
            UINT i2 = face.mIndices[2] + baseVertex;

            // Rendering — ALL meshes
            m_allIndices.push_back(i0);
            m_allIndices.push_back(i1);
            m_allIndices.push_back(i2);

            // Collision — only unmarked meshes
            if (!noCollision)
            {
                m_collisionIndices.push_back(i0);
                m_collisionIndices.push_back(i1);
                m_collisionIndices.push_back(i2);
            }

            subset.indexCount += 3;
        }

        m_mapSubsets.push_back(subset);
        m_mapMaterialNames.push_back(materialName);    
    }

    BuildBVH();
    m_lampLights.clear();

    ProcessNode(
        scene->mRootNode,
        scene,
        aiMatrix4x4()
    );


    size_t litSectionCount = 0;
    size_t elsewhereCount = 0;

    for (const auto& light : m_lampLights)
    {
        const auto& p = light.position;

        if (p.x >= -2600.0f && p.x <= 5000.0f &&
            p.z >= -3500.0f && p.z <= 4100.0f)
        {
            ++litSectionCount;
        }
        else
        {
            ++elsewhereCount;
        }
    }

    OutputDebugStringA(
        ("Working straight lamps: " + std::to_string(litSectionCount) + "\n").c_str()
    );

    OutputDebugStringA(
        ("All other lamps: " + std::to_string(elsewhereCount) + "\n").c_str()
    );
    OutputDebugStringA(
        ("Total lamps: " + std::to_string(m_lampLights.size()) + "\n").c_str()
    );

    index_count = (UINT)m_allIndices.size();

    if (m_allVertices.empty() || m_allIndices.empty())
        return false;

    D3D11_BUFFER_DESC vbd{};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = (UINT)(sizeof(SharedVertex) * m_allVertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vdata{ m_allVertices.data() };
    device->CreateBuffer(&vbd, &vdata, vertex_buffer.GetAddressOf());

    D3D11_BUFFER_DESC ibd{};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = (UINT)(sizeof(UINT) * m_allIndices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA idata{ m_allIndices.data() };
    device->CreateBuffer(&ibd, &idata, index_buffer.GetAddressOf());

    // sampler
    D3D11_SAMPLER_DESC samp{};
    samp.Filter = D3D11_FILTER_ANISOTROPIC;
    samp.MaxAnisotropy = 16;
    samp.AddressU = samp.AddressV = samp.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samp.MinLOD = 0;
    samp.MaxLOD = D3D11_FLOAT32_MAX;

    device->CreateSamplerState(&samp, m_samplerState.GetAddressOf());

    const auto end = std::chrono::steady_clock::now();

    double function = std::chrono::duration<double, std::milli>(end - start).count();

    OutputDebugStringA(("Whole func: " + std::to_string(function) + " ms\n").c_str());

    OutputDebugStringA(
        ("Assimp faces: " +
         std::to_string(m_stats.m_totalTriangles) +
         "\nGenerated indices: " +
         std::to_string(m_allIndices.size()) +
         "\n")
            .c_str());


    return true;
}