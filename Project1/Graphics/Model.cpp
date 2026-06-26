#include "Model.h"
#pragma warning(push)
#pragma warning(disable : 26812) // Prefer 'enum class' over 'enum'
#include "WICTextureLoader.h"
#pragma warning(pop)
#include "MaterialLoader.h"
#include "TextureManager.h"
#include <DirectXMath.h>    // Added this for XMFLOAT types
#include <DDSTextureLoader.h>


Model::Model() {
    index_count = 0;
    m_textureRV = nullptr;
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

bool Model::LoadOBJ(const std::string& mtlFile, const std::string& objFile, ID3D11Device* device) {
   
        
    std::vector<XMFLOAT3> temp_positions;
    std::vector<XMFLOAT2> temp_texcoords;
    std::vector<XMFLOAT3> temp_normals;

    std::map<VertexKey, unsigned int> uniqueVertices;
    std::vector<SharedVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> faceIndices;

    MaterialLoader ML;
    ML.LoadMaterial(mtlFile, "", this->m_materialLib);
    std::ifstream file(objFile);
    if (!file.is_open()) return false;

    std::string line;


    MeshSubset startSubset;
    startSubset.startIndex = 0;

    startSubset.material.diffuseColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
    startSubset.material.ambientColor = XMFLOAT3(0.3f, 0.3f, 0.3f);
    startSubset.material.specularColor = XMFLOAT3(0.1f, 0.1f, 0.1f);
    startSubset.material.specularPower = 32.0f;
    startSubset.material.d = 1.0f;

    
    m_subsets.push_back(startSubset);


    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

  
        // 1. MATERIAL CHANGE
        if (prefix == "usemtl") {   



            if (!m_subsets.empty()) {
                m_subsets.back().indexCount = (unsigned int)indices.size() - m_subsets.back().startIndex;
            }

            if (m_subsets.back().indexCount == 0) {
                m_subsets.pop_back();
            }

            MeshSubset newSubset;
            newSubset.startIndex = (unsigned int)indices.size();

            std::string matName;
            ss >> matName;

            newSubset.materialName = matName;


            if (m_materialLib.find(matName) != m_materialLib.end())
            {
                MaterialData& mat = m_materialLib[matName];

                newSubset.material = mat.gpuMaterial;
                newSubset.diffuseTextureName = mat.diffuseTextureName;

                if (!mat.diffuseTextureName.empty())
                {
                    std::wstring texPath =
                        L"Cars/" + std::wstring(
                            mat.diffuseTextureName.begin(),
                            mat.diffuseTextureName.end());

                    OutputDebugStringA(
                        ("Material: " + matName +
                            " Texture: " +
                            (mat.diffuseTextureName.empty() ? "<none>" : mat.diffuseTextureName) +
                            "\n").c_str());

                    //newSubset.diffuseTexture = textureManager->GetTexture(texPath, context);
                }
            }
            else
            {
                OutputDebugStringA(("Missing material: " + matName + "\n").c_str());

                newSubset.material.diffuseColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
                newSubset.material.ambientColor = XMFLOAT3(0.3f, 0.3f, 0.3f);
                newSubset.material.specularColor = XMFLOAT3(0.1f, 0.1f, 0.1f);
                newSubset.material.specularPower = 32.0f;
                newSubset.material.d = 1.0f;
            }

            m_subsets.push_back(newSubset);
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
    if (!m_subsets.empty()) {
        m_subsets.back().indexCount = (unsigned int)indices.size() - m_subsets.back().startIndex;
    }

    if (vertices.empty()) return false;

    // --- AUTO-CENTER & SCALE (Now on the CPU vertices before GPU upload) ---
    XMFLOAT3 minPos = { FLT_MAX, FLT_MAX, FLT_MAX };
    XMFLOAT3 maxPos = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (const auto& v : vertices) {
        minPos.x = min(minPos.x, v.pos.x); minPos.y = min(minPos.y, v.pos.y); minPos.z = min(minPos.z, v.pos.z);
        maxPos.x = max(maxPos.x, v.pos.x); maxPos.y = max(maxPos.y, v.pos.y); maxPos.z = max(maxPos.z, v.pos.z);
    }

  /**  XMFLOAT3 center = {(minPos.x + maxPos.x) / 2.0f, (minPos.y + maxPos.y) / 2.0f, (minPos.z + maxPos.z) / 2.0f};
    float maxDimension = max(maxPos.x - minPos.x, max(maxPos.y - minPos.y, maxPos.z - minPos.z));

    if (maxDimension < 0.0001f) maxDimension = 1.0f;


    for (auto& v : vertices) {
        v.pos.x = (v.pos.x - center.x) / maxDimension;
        v.pos.y = (v.pos.y - center.y) / maxDimension;
        v.pos.z = (v.pos.z - center.z) / maxDimension;
    }
    */
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
    cbd.Usage = D3D11_USAGE_DYNAMIC; // We update this every frame! [cite: 2026-01-03]
    cbd.ByteWidth = sizeof(SharedSceneData); // Use your SharedSceneData struct [cite: 2026-01-03]
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    device->CreateBuffer(&cbd, nullptr, m_constantBuffer.GetAddressOf());

    this->index_count = (UINT)indices.size();




    
    return true;
}

void Model::ResolveMaterialTextures(TextureManager* textureManager, ID3D11DeviceContext* context,const std::wstring& textureFolder)
{
    for (auto& subset : m_subsets)
    {
        if (subset.diffuseTextureName.empty())
            continue;

        std::wstring textureName(
            subset.diffuseTextureName.begin(),
            subset.diffuseTextureName.end());

        std::wstring fullPath =
            textureFolder + textureName;

        subset.diffuseTexture =
            textureManager->GetTexture(fullPath, context);

        OutputDebugStringW(
            (L"Resolved texture: " + fullPath + L"\n").c_str());
    }
}

bool Model::LoadTexture(ID3D11Device* device, std::wstring filename) {
    m_textureRV.Reset();

    HRESULT hr = E_FAIL;

    if (filename.find(L".dds") != std::wstring::npos) {
        hr = DirectX::CreateDDSTextureFromFile(device, filename.c_str(), nullptr, m_textureRV.GetAddressOf());
    }
    else {
        hr = DirectX::CreateWICTextureFromFileEx(
            device, filename.c_str(), 0,
            D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
            0, D3D11_RESOURCE_MISC_GENERATE_MIPS, DirectX::WIC_LOADER_DEFAULT, nullptr, m_textureRV.GetAddressOf()
        );
    }

    //std::cout << "SUCCESS: Texture loaded at " << m_textureRV.Get() << std::endl;
    return true;
}

void Model::BindTexture(ID3D11DeviceContext* context) {
    if (m_textureRV) {
        // '0' is the slot (register t0 in your shader)
        // '1' is the number of textures
        context->PSSetShaderResources(0, 1, m_textureRV.GetAddressOf());
    }
}

void Model::BindAndDraw(
    ID3D11DeviceContext* context,
    UINT stride,
    DirectX::XMMATRIX world,
    DirectX::XMMATRIX view,
    DirectX::XMMATRIX projection,
    Camera* cam)
{
    UINT offset = 0;

    context->IASetVertexBuffers(
        0, 1,
        vertex_buffer.GetAddressOf(),
        &stride,
        &offset);

    context->IASetIndexBuffer(
        index_buffer.Get(),
        DXGI_FORMAT_R32_UINT,
        0);

    for (const auto& subset : m_subsets)
    {
        SharedSceneData sceneData = {};

        sceneData.material = subset.material;

        sceneData.world = XMMatrixTranspose(world);
        sceneData.view = XMMatrixTranspose(view);
        sceneData.projection = XMMatrixTranspose(projection);

        sceneData.lightDirection = XMFLOAT4(0.5f, -1.0f, 0.5f, 0.0f);
        sceneData.lightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

        DirectX::XMFLOAT3 pos = cam->GetPosition();
        XMVECTOR posVec = XMLoadFloat3(&pos);

        XMStoreFloat4(&sceneData.cameraPosition, posVec);
        XMStoreFloat4(&sceneData.cameraDirection, cam->GetForwardVector());

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
                &sceneData,
                sizeof(SharedSceneData));

            context->Unmap(m_constantBuffer.Get(), 0);
        }

        context->VSSetConstantBuffers(
            0, 1,
            m_constantBuffer.GetAddressOf());

        context->PSSetConstantBuffers(
            0, 1,
            m_constantBuffer.GetAddressOf());

        ID3D11ShaderResourceView* srv =
            subset.diffuseTexture
            ? subset.diffuseTexture
            : m_textureRV.Get();

        context->PSSetShaderResources(
            0, 1,
            &srv);

        context->DrawIndexed(
            subset.indexCount,
            subset.startIndex,
            0);
    }
}