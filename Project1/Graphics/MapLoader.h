#pragma once

#pragma warning(push)
#pragma warning(disable : 26812)
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma warning(pop)

#include "../SharedTypes.h"
#include "TextureManager.h"



class MapLoader {
public:
    MapLoader();
    ~MapLoader();

    void SetTextureManager(TextureManager* texMgr) {
        m_texMgr = texMgr;
    }

    const std::vector<SharedVertex>& GetVertices() const {
        return m_allVertices;
    }

    const std::vector<UINT>& GetIndices() const {
        return m_allIndices;
    }

    // =========================================================
    // DRAW
    // =========================================================
    void Draw(ID3D11DeviceContext* context,
        ID3D11Buffer* cbb,
        const SharedSceneData& engineSceneData)
    {
        SharedSceneData sceneData = engineSceneData;

        UINT stride = sizeof(SharedVertex);
        UINT offset = 0;

        context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

        for (const auto& subset : m_subsets)
        {
            sceneData.material = subset.material;
                

            D3D11_MAPPED_SUBRESOURCE mapped{};
            context->Map(cbb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
            memcpy(mapped.pData, &sceneData, sizeof(SharedSceneData));
            context->Unmap(cbb, 0);

            ID3D11ShaderResourceView* srv = nullptr;

            if (subset.materialIndex >= 0 &&
                subset.materialIndex < (int)m_materialSRVs.size())
            {
                srv = m_materialSRVs[subset.materialIndex];
            }

            if (!srv)
            {
                if (!m_materialSRVs.empty())
                    srv = m_materialSRVs[0];
            }

            // bind safely
            if (srv)
            {
                context->PSSetShaderResources(0, 1, &srv);
            }

            context->VSSetConstantBuffers(0, 1, &cbb);
            context->PSSetConstantBuffers(0, 1, &cbb);

            context->DrawIndexed(subset.indexCount, subset.startIndex, 0);
        }
    }

    // =========================================================
    // LOAD ENTRY
    // =========================================================
    bool OpenAndLoad(const std::string& filename,
        ID3D11Device* device,
        ID3D11DeviceContext* context)
    {
        return LoadWorld(filename, device, context);
    }

    // =========================================================
    // ASSIMP WORLD LOADER
    // =========================================================
    bool LoadWorld(const std::string& filename,
        ID3D11Device* device,
        ID3D11DeviceContext* context)
    {
        Assimp::Importer importer;

        /*const aiScene* scene = importer.ReadFile(
            filename,
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_FlipUVs |
            aiProcess_PreTransformVertices |
            aiProcess_MakeLeftHanded |
            aiProcess_GenNormals
        );*/       
        
        
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

        // reset state (IMPORTANT FIX: prevents garbage grey maps)
        m_allVertices.clear();
        m_allIndices.clear();
        m_subsets.clear();
        m_materialSRVs.clear();

        size_t lastSlash = filename.find_last_of("\\/");
        std::string directory =
            (lastSlash != std::string::npos) ?
            filename.substr(0, lastSlash + 1) : "";

        // =====================================================
        // MATERIAL LOADING (SAFE DEFAULT HANDLING)
        // =====================================================
        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        {
            aiString texPath;
            ID3D11ShaderResourceView* srv = nullptr;

            if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
            {
                std::string textureName = texPath.C_Str();
                std::string file = textureName.substr(textureName.find_last_of("/\\") + 1);
               
    

       

                OutputDebugStringA(
                    ("Material " + std::to_string(i) +
                        " texture=" + file +
                        "\n").c_str());


                std::wstring wpath(directory.begin(), directory.end());
                wpath += std::wstring(file.begin(), file.end());

                if (m_texMgr)
                    srv = m_texMgr->GetTexture(wpath, context);
            }

            m_materialSRVs.push_back(srv);
        }

        // =====================================================
        // PROCESS MESHES
        // =====================================================
        for (unsigned int i = 0; i < scene->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[i];

            UINT baseVertex = (UINT)m_allVertices.size();

            MeshSubset subset{};
            subset.startIndex = (UINT)m_allIndices.size();
            subset.indexCount = 0;
            subset.materialIndex = mesh->mMaterialIndex;
            

        
            std::string meshName = mesh->mName.C_Str();

            std::string n = meshName;
            std::transform(n.begin(), n.end(), n.begin(), ::tolower);

            if (n.find("road") != std::string::npos ||
                n.find("asphalt") != std::string::npos)
            {
                OutputDebugStringA(("Asphalt: " + meshName + "\n").c_str());
                subset.material.materialType = static_cast<float>(MaterialType::MATERIAL_ASPHALT);
            }       
            
            /*if (n.find("tree") != std::string::npos || meshName.find("KSTREE") != std::string::npos)
            {
                OutputDebugStringA(("Tree: " + meshName + "\n").c_str());
                subset.material.materialType = static_cast<float>(MaterialType::MATERIAL_TREE);
            }*/

            if (n.find("bulb") != std::string::npos || n.find("tunnel_lamps") != std::string::npos || n.find("lightemitter") != std::string::npos|| n.find("streetlamp_sub1") != std::string::npos)
            {
                OutputDebugStringA(("Lamp: " + meshName + "\n").c_str());
                subset.material.materialType = static_cast<float>(MaterialType::MATERIAL_LAMP);
            }

 

            OutputDebugStringA(
                ("Mesh " + std::to_string(i) +
                    " name=" + meshName +
                    " mat=" + std::to_string(mesh->mMaterialIndex) +
                    "\n").c_str());

      
            // default safe material (FIX for grey/black map issue)
            subset.material.diffuseColor = XMFLOAT3(1, 1, 1);
            subset.material.ambientColor = { 0.3f,0.3f,0.3f };
            subset.material.specularColor = { 0.1f,0.1f,0.1f };
            subset.material.specularPower = 32.0f;
            subset.material.d = 1.0f;

            if (mesh->mNumVertices > 0)
            {
                for (unsigned int v = 0; v < mesh->mNumVertices; v++)
                {
                    SharedVertex vert{};

                    aiVector3D p = mesh->mVertices[v];
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


            }

            for (unsigned int f = 0; f < mesh->mNumFaces; f++)
            {

              
               

                const aiFace& face = mesh->mFaces[f];
                if (face.mNumIndices != 3) continue;

                m_allIndices.push_back(face.mIndices[0] + baseVertex);
                m_allIndices.push_back(face.mIndices[1] + baseVertex);
                m_allIndices.push_back(face.mIndices[2] + baseVertex);

                subset.indexCount += 3;
            }

            m_subsets.push_back(subset);
        }

        index_count = (UINT)m_allIndices.size();

        // =====================================================
        // GPU BUFFERS
        // =====================================================
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

    // =========================================================
    // DATA
    // =========================================================
    TextureManager* m_texMgr = nullptr;

    UINT index_count = 0;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

    std::vector<ID3D11ShaderResourceView*> m_materialSRVs;
    std::vector<MeshSubset> m_subsets;

    std::vector<SharedVertex> m_allVertices;
    std::vector<UINT> m_allIndices;
};