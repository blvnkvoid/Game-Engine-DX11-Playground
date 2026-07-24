#include "MapLoader.h"

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
    ID3D11DepthStencilState* depthWriteOn)
{
    SharedSceneData sceneData = engineSceneData;

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


    for (const auto& subset : m_subsets)
    {
        context->DrawIndexed(
            subset.indexCount,
            subset.startIndex,
            0
        );
    }
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


    for (const auto& subset : m_subsets)
    {
        if (m_enableFrustumCulling && (!frustum.Intersects(subset.bounds)))
        {
            m_stats.m_culledDrawCalls++;
            continue;
        }

        sceneData.material = subset.material;
        D3D11_MAPPED_SUBRESOURCE mapped{};
        context->Map(cbb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        memcpy(mapped.pData, &sceneData, sizeof(sceneData));
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

        context->PSSetShaderResources(0, 1, &srv); 
        context->VSSetConstantBuffers(0, 1, &cbb);
        context->PSSetConstantBuffers(0, 1, &cbb);

        m_stats.m_drawCalls++;
        context->DrawIndexed(subset.indexCount, subset.startIndex, 0);
    }
    // OutputDebugStringA(("Draw calls: " + std::to_string(m_drawCalls) + "\n").c_str());
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
    m_subsets.clear();
    m_materialSRVs.clear();

    size_t lastSlash = filename.find_last_of("\\/");
    std::string directory =
        (lastSlash != std::string::npos) ?
        filename.substr(0, lastSlash + 1) : "";


    m_stats.m_totalVertices = 0;
    m_stats.m_totalTriangles = 0;

    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        aiString texPath;
        ID3D11ShaderResourceView* srv = nullptr;

        if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
        {
            std::string textureName = texPath.C_Str();
            std::string file = textureName.substr(textureName.find_last_of("/\\") + 1);

            std::wstring wpath(directory.begin(), directory.end());
            wpath += std::wstring(file.begin(), file.end());

            if (m_texMgr)
                srv = m_texMgr->GetTexture(wpath, context);
        }

        m_materialSRVs.push_back(srv);
    }

    m_stats.m_meshes = scene->mNumMeshes;

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];


        m_stats.m_totalVertices += mesh->mNumVertices;
        m_stats.m_totalTriangles += mesh->mNumFaces; // assuming triangulation

        UINT baseVertex = (UINT)m_allVertices.size();

        MapMeshSubset  subset{};
        subset.startIndex = (UINT)m_allIndices.size();
        subset.indexCount = 0;
        subset.materialIndex = mesh->mMaterialIndex;


        std::string meshName = mesh->mName.C_Str();

        std::string n = meshName;
        std::transform(n.begin(), n.end(), n.begin(), ::tolower);

        if (n.find("road") != std::string::npos ||
            n.find("asphalt") != std::string::npos ||
            n.find("grass") != std::string::npos)
        {
            // OutputDebugStringA(("Asphalt: " + meshName + "\n").c_str());
            subset.material.materialType = static_cast<float>(MaterialType::MATERIAL_ASPHALT);
        }

        if (n.find("tree") != std::string::npos || meshName.find("KSTREE") != std::string::npos)
        {
            //   OutputDebugStringA(("Tree: " + meshName + "\n").c_str());
            subset.material.materialType = static_cast<float>(MaterialType::MATERIAL_TREE);
        }

        if (n.find("bulb") != std::string::npos || n.find("tunnel_lamps") != std::string::npos || n.find("lightemitter") != std::string::npos || n.find("streetlamp_sub1") != std::string::npos)
        {
            //OutputDebugStringA(("Lamp: " + meshName + "\n").c_str());
            subset.material.materialType = static_cast<float>(MaterialType::MATERIAL_LAMP);
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

        subset.material.diffuseColor = XMFLOAT3(1, 1, 1);
        subset.material.ambientColor = { 0.3f,0.3f,0.3f };
        subset.material.specularColor = { 0.1f,0.1f,0.1f };
        subset.material.specularPower = 32.0f;
        subset.material.d = 1.0f;

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

            m_allIndices.push_back(face.mIndices[0] + baseVertex);
            m_allIndices.push_back(face.mIndices[1] + baseVertex);
            m_allIndices.push_back(face.mIndices[2] + baseVertex);

            subset.indexCount += 3;
        }

        m_subsets.push_back(subset);
    }

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