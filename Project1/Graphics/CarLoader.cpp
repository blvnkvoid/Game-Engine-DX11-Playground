#include "CarLoader.h"
#pragma warning(push)
#pragma warning(disable : 26812) // Prefer 'enum class' over 'enum'
#pragma warning(pop)
#include <DirectXMath.h>    // Added this for XMFLOAT types


CarLoader::CarLoader() {
    index_count = 0;
    vertex_buffer = nullptr;
    index_buffer = nullptr;
}

struct VertexKey {
    int v, vt, vn;
    bool operator<(const VertexKey& other) const {
        if (v != other.v) return v < other.v;
        if (vt != other.vt) return vt < other.vt;
        return vn < other.vn;
    }
};

void CarLoader::SetMaterialLoader(
    SharedMaterialLoader& materialLoader)
{
    m_materialLoader = &materialLoader;
}


void CarLoader::SetMaterialLibraryPath(
    const std::string& path)
{
    m_materialLibraryPath = path;
}

void CarLoader::CheckCarMaterial(size_t index)
{
    if (!m_materialLoader)
        return;

    if (index >= m_carMaterialNames.size())
        return;

    const std::string& materialName =
        m_carMaterialNames[index];

    const MaterialData& material =
        m_materialLoader->ResolveCarMaterial(
            materialName
        );

    // breakpoint
}

bool CarLoader::LoadOBJ(const std::string& objFile, ID3D11Device* device) {
   
        
    std::vector<XMFLOAT3> temp_positions;
    std::vector<XMFLOAT2> temp_texcoords;
    std::vector<XMFLOAT3> temp_normals;

    std::map<VertexKey, unsigned int> uniqueVertices;
    std::vector<SharedVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> faceIndices;

    std::ifstream file(objFile);
    if (!file.is_open()) return false;
                

    std::string line;

    MeshSubset startSubset;
    startSubset.startIndex = 0;

    
    m_carSubsets.push_back(startSubset);


    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

  
        // 1. MATERIAL CHANGE
        if (prefix == "usemtl") {   

            if (!m_carSubsets.empty())
            {
                m_carSubsets.back().indexCount =
                    (unsigned int)indices.size() -
                    m_carSubsets.back().startIndex;

                if (m_carSubsets.back().indexCount == 0)
                {
                    m_carSubsets.pop_back();

                    if (!m_carMaterialNames.empty())
                        m_carMaterialNames.pop_back();
                }
            }

            MeshSubset newSubset;
            newSubset.startIndex = (unsigned int)indices.size();


            std::string matName;
            ss >> matName;



            m_carSubsets.push_back(newSubset);
            m_carMaterialNames.push_back(matName);
        }
        // 2. GEOMETRY DATA
        else if (prefix == "v") {
            XMFLOAT3 pos; ss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        }
        else if (prefix == "vt") {
            XMFLOAT2 tex; ss >> tex.x >> tex.y;
           //tex.x = 1.0f - tex.x;
           tex.y = 1.0f - tex.y; // Flip for DX
            temp_texcoords.push_back(tex);
        }
        else if (prefix == "vn") {
            XMFLOAT3 norm; ss >> norm.x >> norm.y >> norm.z;
            temp_normals.push_back(norm);
        }
        // 3. FACE PARSING
        else if (prefix == "f") {
            faceIndices.clear(); // Clear local bucket for THIS line
            std::string vertexStr;

            while (ss >> vertexStr) {
                int vIdx = 0, tIdx = 0, nIdx = 0;
                size_t firstSlash = vertexStr.find('/');
                size_t lastSlash = vertexStr.find_last_of('/');

                vIdx = std::stoi(vertexStr.substr(0, firstSlash));
                if (vIdx < 0) vIdx = (int)temp_positions.size() + vIdx + 1;

                if (firstSlash != std::string::npos && lastSlash != std::string::npos && (lastSlash - firstSlash) > 1) {
                    std::string tStr = vertexStr.substr(firstSlash + 1, lastSlash - (firstSlash + 1));
                    if (!tStr.empty()) tIdx = std::stoi(tStr);
                }

                if (lastSlash != std::string::npos && lastSlash > firstSlash) {
                    std::string nStr = vertexStr.substr(lastSlash + 1);
                    if (!nStr.empty()) nIdx = std::stoi(nStr);
                }
                VertexKey vk = { vIdx, tIdx, nIdx };

                if (uniqueVertices.count(vk) == 0) {
                    SharedVertex v;
                    v.pos = temp_positions[vIdx - 1];
                    v.pos.z = -v.pos.z;
                    v.texCoord = (tIdx > 0) ? temp_texcoords[tIdx - 1] : XMFLOAT2(0, 0);
                    v.normal = (nIdx > 0) ? temp_normals[nIdx - 1] : XMFLOAT3(0, 1, 0);
                    v.normal.z = -v.normal.z;
                    v.color = XMFLOAT4(1, 1, 1, 1);
                    v.tangent = XMFLOAT4(0, 0, 0, 1);
                    uniqueVertices[vk] = (unsigned int)vertices.size();
                    vertices.push_back(v);
                }
                faceIndices.push_back(uniqueVertices[vk]);
            }

            // Triangulate
            if (faceIndices.size() >= 3) {
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[2]);
                indices.push_back(faceIndices[1]);
                if (faceIndices.size() == 4) {
                    indices.push_back(faceIndices[0]);
                    indices.push_back(faceIndices[3]);
                    indices.push_back(faceIndices[2]);
                }
            }
        }
    }

    // Finalize the last subset
    if (!m_carSubsets.empty()) {
        m_carSubsets.back().indexCount = (unsigned int)indices.size() - m_carSubsets.back().startIndex;
    }

    if (vertices.empty()) return false;

    // TODO: minPos/maxPos currently unused - old auto-center/scale code?
    XMFLOAT3 minPos = { FLT_MAX, FLT_MAX, FLT_MAX };
    XMFLOAT3 maxPos = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (const auto& v : vertices) {
        minPos.x = min(minPos.x, v.pos.x); minPos.y = min(minPos.y, v.pos.y); minPos.z = min(minPos.z, v.pos.z);
        maxPos.x = max(maxPos.x, v.pos.x); maxPos.y = max(maxPos.y, v.pos.y); maxPos.z = max(maxPos.z, v.pos.z);
    }


    // --- GENERATE TANGENTS ---
    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        SharedVertex& v0 = vertices[indices[i]];
        SharedVertex& v1 = vertices[indices[i + 1]];
        SharedVertex& v2 = vertices[indices[i + 2]];

        XMVECTOR p0 = XMLoadFloat3(&v0.pos);
        XMVECTOR p1 = XMLoadFloat3(&v1.pos);
        XMVECTOR p2 = XMLoadFloat3(&v2.pos);

        XMVECTOR edge1 = XMVectorSubtract(p1, p0);
        XMVECTOR edge2 = XMVectorSubtract(p2, p0);

        float du1 = v1.texCoord.x - v0.texCoord.x;
        float dv1 = v1.texCoord.y - v0.texCoord.y;

        float du2 = v2.texCoord.x - v0.texCoord.x;
        float dv2 = v2.texCoord.y - v0.texCoord.y;

        float determinant = du1 * dv2 - du2 * dv1;

        // Degenerate UV triangle - can't calculate a useful tangent
        if (fabsf(determinant) < 1e-8f)
            continue;

        float r = 1.0f / determinant;

        XMVECTOR tangent =
            XMVectorScale(
                XMVectorSubtract(
                    XMVectorScale(edge1, dv2),
                    XMVectorScale(edge2, dv1)
                ),
                r
            );

        //tangent = XMVector3Normalize(tangent);

        XMFLOAT3 t;
        XMStoreFloat3(&t, tangent);

        v0.tangent.x += t.x;
        v0.tangent.y += t.y;
        v0.tangent.z += t.z;

        v1.tangent.x += t.x;
        v1.tangent.y += t.y;
        v1.tangent.z += t.z;

        v2.tangent.x += t.x;
        v2.tangent.y += t.y;
        v2.tangent.z += t.z;
    }


    for (auto& v : vertices)
    {
        XMVECTOR N = XMLoadFloat3(&v.normal);

        XMVECTOR T = XMVectorSet(
            v.tangent.x,
            v.tangent.y,
            v.tangent.z,
            0.0f);

        // Gram-Schmidt:
        // remove any part of T pointing along N
        float dotNT = XMVectorGetX(XMVector3Dot(N, T));

        T = XMVectorSubtract(
            T,
            XMVectorScale(N, dotNT)
        );

        if (XMVectorGetX(XMVector3LengthSq(T)) > 1e-8f)
        {
            T = XMVector3Normalize(T);

            XMFLOAT3 result;
            XMStoreFloat3(&result, T);

            v.tangent.x = result.x;
            v.tangent.y = result.y;
            v.tangent.z = result.z;
        }
        else
        {
            // Safe fallback
            v.tangent.x = 1.0f;
            v.tangent.y = 0.0f;
            v.tangent.z = 0.0f;
        }

        v.tangent.w = 1.0f;
    }

    // --- CREATE BUFFERS (Using final, scaled data) ---
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = (UINT)(sizeof(SharedVertex) * vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vData = { vertices.data() };
    device->CreateBuffer(&vbd, &vData, vertex_buffer.GetAddressOf());

    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = (UINT)(sizeof(unsigned int) * indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA iData = { indices.data() };
    device->CreateBuffer(&ibd, &iData, index_buffer.GetAddressOf());

    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.ByteWidth = sizeof(SharedSceneData);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    device->CreateBuffer(&cbd, nullptr, m_constantBuffer.GetAddressOf());

    this->index_count = (UINT)indices.size();  

    return true;
}

void CarLoader::CreateDefaultMapsTexture(ID3D11Device* device)
{
    const uint32_t pixel = 0xFFFFFFFF; // RGBA = 1,1,1,1

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = 1;
    textureDesc.Height = 1;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = &pixel;
    initialData.SysMemPitch = sizeof(uint32_t);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

    HRESULT hr = device->CreateTexture2D(
        &textureDesc,
        &initialData,
        &texture
    );

    if (FAILED(hr))
        return;

    hr = device->CreateShaderResourceView(
        texture.Get(),
        nullptr,
        &m_mapsTextureRV
    );

    if (FAILED(hr))
        m_mapsTextureRV.Reset();
}


void CarLoader::CreateFlatNormalTexture(ID3D11Device* device)
{
    const unsigned char pixel[4] = { 128, 128, 255, 255 };

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = 1;
    desc.Height = 1;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA data{};
    data.pSysMem = pixel;
    data.SysMemPitch = 4;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

    HRESULT hr = device->CreateTexture2D(
        &desc,
        &data,
        texture.GetAddressOf());

    if (FAILED(hr))
        return;

    hr = device->CreateShaderResourceView(
        texture.Get(),
        nullptr,
        m_flatNormalRV.GetAddressOf());

    if (FAILED(hr))
        m_flatNormalRV.Reset();
}


void CarLoader::BindAndDraw(
    ID3D11DeviceContext* context,
    UINT stride,
    DirectX::XMMATRIX world,
    DirectX::XMMATRIX view,
    DirectX::XMMATRIX projection,
    ID3D11DepthStencilState* depthWriteOn,
    ID3D11DepthStencilState* depthWriteOff,
    ID3D11BlendState* alphaBlendState,
    ID3D11Buffer* lampInfoBuffer,
    ID3D11ShaderResourceView* lampLightsSRV,    
    const SharedSceneData& sceneData)
{
    UINT offset = 0;

    SharedSceneData drawData = sceneData;

    context->IASetVertexBuffers(
        0, 1,
        vertex_buffer.GetAddressOf(),
        &stride,
        &offset);

    context->IASetIndexBuffer(
        index_buffer.Get(),
        DXGI_FORMAT_R32_UINT,
        0);

    context->PSSetConstantBuffers(
        1, 1,
        &lampInfoBuffer);

    context->PSSetShaderResources(
        1, 1,
        &lampLightsSRV);

    for (size_t i = 0; i < m_carSubsets.size(); i++)
    {

        const MeshSubset& subset = m_carSubsets[i];

        const MaterialData& material =
            m_materialLoader->ResolveCarMaterial(
                m_carMaterialNames[i]
            );


        drawData.material = material.gpuMaterial;
        drawData.world = XMMatrixTranspose(world);
        drawData.view = XMMatrixTranspose(view);
        drawData.projection = XMMatrixTranspose(projection);

        bool isGlass =
            static_cast<int>(material.gpuMaterial.materialType) ==
            static_cast<int>(MaterialType::MATERIAL_GLASS);

        if (isGlass)
        {
            float blendFactor[4] = { 0, 0, 0, 0 };

            context->OMSetBlendState(
                alphaBlendState,
                blendFactor,
                0xffffffff);

            context->OMSetDepthStencilState(
                depthWriteOff,
                0);
        }
        else
        {
            context->OMSetBlendState(
                nullptr,
                nullptr,
                0xffffffff);

            context->OMSetDepthStencilState(
                depthWriteOn,
                0);
        }


        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT hr = context->Map(
            m_constantBuffer.Get(),
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mappedResource);



        if (SUCCEEDED(hr))
        {
            memcpy(
                mappedResource.pData,
                &drawData,
                sizeof(SharedSceneData));

            context->Unmap(m_constantBuffer.Get(), 0);
        }

        context->VSSetConstantBuffers(
            0, 1,
            m_constantBuffer.GetAddressOf());

        context->PSSetConstantBuffers(
            0, 1,
            m_constantBuffer.GetAddressOf());

        ID3D11ShaderResourceView* diffuseSRV = material.carTextures.diffuse ? material.carTextures.diffuse.Get() : m_textureRV.Get();
        context->PSSetShaderResources(0, 1, &diffuseSRV);


        ID3D11ShaderResourceView* normalSRV =
            material.carTextures.normal
            ? material.carTextures.normal.Get()
            : m_flatNormalRV.Get();

        context->PSSetShaderResources(3, 1, &normalSRV);


        ID3D11ShaderResourceView* detailSRV =
            material.carTextures.detail
            ? material.carTextures.detail.Get()
            : m_textureRV.Get();

        context->PSSetShaderResources(4, 1, &detailSRV);


        ID3D11ShaderResourceView* normalDetailSRV =
            material.carTextures.normalDetail
            ? material.carTextures.normalDetail.Get()
            : m_flatNormalRV.Get();

        context->PSSetShaderResources(5, 1, &normalDetailSRV);


        ID3D11ShaderResourceView* mapsSRV =
            material.carTextures.maps
            ? material.carTextures.maps.Get()
            : m_mapsTextureRV.Get();

        context->PSSetShaderResources(6, 1, &mapsSRV);

        context->DrawIndexed(
            subset.indexCount,
            subset.startIndex,
            0);

    }
    context->OMSetDepthStencilState(depthWriteOn, 0);
}