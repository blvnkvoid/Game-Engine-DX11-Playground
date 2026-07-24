#pragma once

#pragma warning(push)
#pragma warning(disable : 26812)
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma warning(pop)

#include "../SharedTypes.h"
#include "TextureManager.h"
#include <cfloat>
#include <algorithm>
#include <DirectXCollision.h>
#include "../Scene/Camera.h"
#include <sstream>

struct MapMarker
{
    std::string name;

    float x;
    float y;
    float z;

    float rotX;
    float rotY;
    float rotZ;
    float rotW;
};

struct MapRendererStats
{
    uint32_t  m_drawCalls = 0;
    uint32_t  m_culledDrawCalls = 0;
    uint32_t  m_meshes = 0;
    uint32_t  m_totalVertices = 0;
    uint32_t  m_totalTriangles = 0;
    uint32_t  lampCount = 0;
    uint32_t  activelampCount = 0;
    bool lightsEnabled = true;
};

struct MapMeshSubset
{
    UINT startIndex = 0;
    UINT indexCount = 0;
    int materialIndex = -1;

    SharedMaterial material;
    DirectX::BoundingBox bounds;
};

struct LampLight
{
    DirectX::XMFLOAT3 position;
    float radius;

    //DirectX::XMFLOAT3 color;
    //float intensity;
};

class MapLoader {
public:

   

    void SetTextureManager(TextureManager* texMgr) { m_texMgr = texMgr; }
    const std::vector<SharedVertex>& GetVertices() const { return m_allVertices; }
    const std::vector<UINT>& GetIndices() const { return m_allIndices; }
    const std::vector<MapMarker>& GetMarkers() const { return m_markers; }
    
    void Draw(ID3D11DeviceContext* context,
        ID3D11Buffer* cbb,
        ID3D11Buffer* lampInfoBuffer,
        ID3D11ShaderResourceView* lampLightsSRV,
        const SharedSceneData& engineSceneData,
        const DirectX::BoundingFrustum& frustum,
        ID3D11DepthStencilState* depthWriteOn);

    void DrawShadow(
        ID3D11DeviceContext* context,
        ID3D11Buffer* cbb,
        const SharedSceneData& engineSceneData,
        ID3D11DepthStencilState* depthWriteOn);


    void UpdateVisibleLights(
        ID3D11DeviceContext* context,
        ID3D11Buffer* lampStructuredBuffer,
        ID3D11Buffer* lampInfoBuffer,
        const DirectX::BoundingFrustum& frustum,
        Camera* cam
        );

    void UploadLampData(
        ID3D11DeviceContext* context,
        ID3D11Buffer* lampStructuredBuffer,
        ID3D11Buffer* lampInfoBuffer);


    bool OpenAndLoad(const std::string& filename,
        ID3D11Device* device,
        ID3D11DeviceContext* context)
    {
        return LoadWorld(filename, device, context);
    }
    TextureManager* m_texMgr = nullptr;

    const MapRendererStats& GetStats() const
    {
        return m_stats;
    }
    

 private:
     void ProcessNode(
         const aiNode* node,
         const aiScene* scene,
         const aiMatrix4x4& parentTransform);


     void FindGridNodes(
         aiNode* node,
         const aiMatrix4x4& parentTransform);

     bool LoadWorld(const std::string& filename,
         ID3D11Device* device,
         ID3D11DeviceContext* context);

     DirectX::XMFLOAT3 CalculateMeshCenter(const aiMesh* mesh);     

     UINT index_count = 0;
     Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
     Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
     Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
     std::vector<ID3D11ShaderResourceView*> m_materialSRVs;
     std::vector<MapMeshSubset> m_subsets;
     std::vector<SharedVertex> m_allVertices;
     std::vector<UINT> m_allIndices;

     std::vector<MapMarker> m_markers;
     std::vector<LampLight> m_visibleLampLights;
     std::vector<LampLight> m_lampLights;

     MapRendererStats m_stats;
     bool m_lightsEnabled = true;

};