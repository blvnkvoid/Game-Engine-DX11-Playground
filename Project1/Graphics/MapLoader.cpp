#include "MapLoader.h"
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cfloat>

void MapLoader::SetMaterialLoader(
    SharedMaterialLoader& materialLoader)
{
    m_materialLoader = &materialLoader;
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

    OutputDebugStringA(
        ("BVH built: " +
            std::to_string(m_bvhNodes.size()) +
            " nodes, " +
            std::to_string(m_bvhIndices.size()) +
            " subsets\n").c_str()
    );
}

void MapLoader::DrawShadowBVHNode(
    int nodeIndex,
    ID3D11DeviceContext* context,
    const DirectX::BoundingFrustum& frustum)
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
        DrawShadowBVHNode(node.left, context, frustum);
        DrawShadowBVHNode(node.right, context, frustum);
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

        context->DrawIndexed(
            subset.indexCount,
            subset.startIndex,
            0
        );

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
    ID3D11DeviceContext* context,
    ID3D11Buffer* cbb,
    const SharedSceneData& engineSceneData,
    const DirectX::BoundingFrustum& frustum,
    ID3D11DepthStencilState* depthWriteOn)
{
    SharedSceneData sceneData = engineSceneData;


    m_stats.m_shadowDrawCallsCulled = 0;
    m_stats.m_shadowdrawCalls = 0;

    context->OMSetBlendState(
        nullptr,
        nullptr,
        0xffffffff
    );

    context->OMSetDepthStencilState(
        depthWriteOn,
        0
    );

    UINT stride = sizeof(SharedVertex);
    UINT offset = 0;

    context->IASetVertexBuffers(
        0,
        1,
        vertex_buffer.GetAddressOf(),
        &stride,
        &offset
    );

    context->IASetIndexBuffer(
        index_buffer.Get(),
        DXGI_FORMAT_R32_UINT,
        0
    );

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    context->VSSetConstantBuffers(
        0,
        1,
        &cbb
    );

    D3D11_MAPPED_SUBRESOURCE mapped{};


    context->Map(
        cbb,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapped
    );

    memcpy(
        mapped.pData,
        &sceneData,
        sizeof(sceneData)
    );

    context->Unmap(cbb, 0);

    static  bool m_enableFrustumCulling = true;

    static bool hWasPressed = false;
    bool hPressed = GetAsyncKeyState('H') & 0x8000;
    if (hPressed && !hWasPressed)
    {
        m_enableFrustumCulling = !m_enableFrustumCulling;

        OutputDebugStringA(
            m_enableFrustumCulling
            ? "Shadow Frustum culling: ON\n"
            : "Shadow Frustum culling: OFF\n"
        );
    }
    hWasPressed = hPressed;

    m_stats.culledIndices = 0;
    m_stats.renderedIndices = 0;
    m_stats.zeroIndexSubsets = 0;

    const auto start = std::chrono::high_resolution_clock::now();

    if (m_enableFrustumCulling && !m_bvhNodes.empty())
    {
        // H ON: BVH shadow culling
        DrawShadowBVHNode(
            0,
            context,
            frustum
        );
    }
    else
    {
        // H OFF: draw every shadow subset
        for (const auto& subset : m_mapSubsets)
        {
            if (subset.indexCount == 0)
            {
                m_stats.zeroIndexSubsets++;
                continue;
            }

            context->DrawIndexed(
                subset.indexCount,
                subset.startIndex,
                0
            );

            m_stats.m_shadowdrawCalls++;
            m_stats.renderedIndices += subset.indexCount;
        }
    }

    const auto end = std::chrono::high_resolution_clock::now();

    double shadowCpuMs =
        std::chrono::duration<double, std::milli>(end - start).count();

    m_stats.shadowCpuMs = shadowCpuMs;

}

void MapLoader::Draw(ID3D11DeviceContext* context,
    ID3D11Buffer* cbb,
    ID3D11Buffer* lampInfoBuffer,
    ID3D11ShaderResourceView* lampLightsSRV,
    const SharedSceneData& engineSceneData,
    const DirectX::BoundingFrustum& frustum,
    ID3D11DepthStencilState* depthWriteOn)
{
    SharedSceneData sceneData = engineSceneData;
    SharedSceneData drawData = sceneData;
    m_stats.m_drawCalls = 0;
    m_stats.m_culledDrawCalls = 0;


    context->OMSetBlendState(
        nullptr,
        nullptr,
        0xffffffff);

    context->OMSetDepthStencilState(
        depthWriteOn,
        0);

    UINT stride = sizeof(SharedVertex);
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());


    static bool lWasPressed = false;
    bool lPressed = (GetAsyncKeyState('L') & 0x8000) != 0;

    if (lPressed && !lWasPressed)
    {
        m_lightsEnabled = !m_lightsEnabled;
        OutputDebugStringA(
            m_lightsEnabled
            ? "Track lighting: ON\n"
            : "Track lighting: OFF\n");
    }

    m_stats.lightsEnabled = m_lightsEnabled;

    lWasPressed = lPressed;


    if (m_lightsEnabled)
    {
        context->PSSetConstantBuffers(1, 1, &lampInfoBuffer);
        context->PSSetShaderResources(1, 1, &lampLightsSRV);
    }
    else
    {
        ID3D11Buffer* nullCB = nullptr;
        ID3D11ShaderResourceView* nullSRV = nullptr;

        context->PSSetConstantBuffers(
            1,
            1,
            &nullCB);

        context->PSSetShaderResources(
            1,
            1,
            &nullSRV);
    }

    m_stats.lampCount =
        static_cast<uint32_t>(
           min(m_lampLights.size(), size_t{ 1200 }));
   
    static  bool m_enableFrustumCulling = true;

    static bool fWasPressed = false;
    bool fPressed = GetAsyncKeyState('F') & 0x8000;
    if (fPressed && !fWasPressed)
    {
        m_enableFrustumCulling = !m_enableFrustumCulling;

        OutputDebugStringA(
            m_enableFrustumCulling
            ? "Frustum culling: ON\n"
            : "Frustum culling: OFF\n"
        );
    }
    fWasPressed = fPressed;


        std::vector<uint32_t> visibleSubsets;

    if (m_enableFrustumCulling && !m_bvhNodes.empty())
    {
        CollectVisibleBVH(
            0,
            frustum,
            visibleSubsets
        );

        // Preserve original subset draw order.
        std::sort(
            visibleSubsets.begin(),
            visibleSubsets.end()
        );
    }
    else
    {
        // Original behavior when culling is disabled,
        // or when no BVH has been built.
        visibleSubsets.reserve(m_mapSubsets.size());

        for (uint32_t i = 0;
            i < m_mapSubsets.size();
            ++i)
        {
            visibleSubsets.push_back(i);
        }
    }

    for (uint32_t i : visibleSubsets)
    {
        const auto& subset = m_mapSubsets[i];



        assert(subset.resolvedMaterial != nullptr);

        const MaterialData& material =
            *subset.resolvedMaterial;

  

        drawData.material = material.gpuMaterial;

        drawData.material.diffuseColor = { 1,1,1 };
        drawData.material.ambientColor = { 0.3f,0.3f,0.3f };
        drawData.material.specularColor = { 0.1f,0.1f,0.1f };
        drawData.material.specularPower = 32.0f;
        drawData.material.d = 1.0f;

        drawData.material.isMapMaterial = 1.0f;

        if (subset.materialType != MaterialType::MATERIAL_DEFAULT)
        {
            drawData.material.materialType =
                static_cast<float>(subset.materialType);
        }

        ID3D11ShaderResourceView* diffuseSRV = nullptr;

        // 1. Explicit texture loaded by our material/MTL system
        if (material.mapTextures.diffuse)
        {
            diffuseSRV = material.mapTextures.diffuse.Get();

           /* std::string debug =
                "[DIFFUSE SOURCE: MTL] "
                "subset=" + std::to_string(i) +
                " | name=" + m_mapMaterialNames[i] +
                " | texture=" + material.diffuseTextureName +
                " | matIndex=" + std::to_string(subset.materialIndex) +
                "\n";*/

         //   OutputDebugStringA(debug.c_str());
        }

        // 2. Otherwise use the texture referenced by the FBX material
        else if (subset.materialIndex < m_materialSRVs.size())
        {
            diffuseSRV = m_materialSRVs[subset.materialIndex].Get();

           /* std::string debug =
                "[DIFFUSE SOURCE: FBX] "
                "subset=" + std::to_string(i) +
                " | name=" + m_mapMaterialNames[i] +
                " | MTL texture=" + material.diffuseTextureName +
                " | matIndex=" + std::to_string(subset.materialIndex) +
                " | SRV=" + (diffuseSRV ? "YES" : "NULL") +
                "\n";
                */
          // OutputDebugStringA(debug.c_str());
        }
        else
        {
          /*  std::string debug =
                "[DIFFUSE SOURCE: NONE] "
                "subset=" + std::to_string(i) +
                " | name=" + m_mapMaterialNames[i] +
                " | matIndex=" + std::to_string(subset.materialIndex) +
                "\n";

           // OutputDebugStringA(debug.c_str());*/
        }

        context->PSSetShaderResources(0, 1, &diffuseSRV);

        ID3D11ShaderResourceView* normalSRV = material.mapTextures.normal.Get();
        context->PSSetShaderResources(8, 1, &normalSRV);

        ID3D11ShaderResourceView* detailSRV = material.mapTextures.detail.Get();
        context->PSSetShaderResources(9, 1, &detailSRV);

        ID3D11ShaderResourceView* normalDetailSRV = material.mapTextures.normalDetail.Get();
        context->PSSetShaderResources(10, 1, &normalDetailSRV);

        ID3D11ShaderResourceView* mapsSRV = material.mapTextures.maps.Get();
        context->PSSetShaderResources(11, 1, &mapsSRV);


        D3D11_MAPPED_SUBRESOURCE mapped{};
        context->Map(
            cbb,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mapped);

        memcpy(mapped.pData,&drawData,sizeof(drawData));

        context->Unmap(cbb, 0);

        context->VSSetConstantBuffers(0, 1, &cbb);
        context->PSSetConstantBuffers(0, 1, &cbb);

        m_stats.m_drawCalls++;

        context->DrawIndexed(
            subset.indexCount,
            subset.startIndex,
            0);
    }

    // OutputDebugStringA(("Draw calls: " + std::to_string(m_drawCalls) + "\n").c_str());
}


void MapLoader::ResolveMaterials()
{
    assert(m_materialLoader != nullptr);

    for (size_t i = 0; i < m_mapSubsets.size(); ++i)
    {
        m_mapSubsets[i].resolvedMaterial =
            &m_materialLoader->ResolveMapMaterial(
                m_mapMaterialNames[i]
            );
    }
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
    Assimp::Importer importer;

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

    return true;
}