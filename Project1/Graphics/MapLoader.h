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
#include "SharedMaterialLoader.h"

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

struct BVHNode
{
    DirectX::BoundingBox bounds;

    int left = -1;
    int right = -1;

    uint32_t first = 0;
    uint32_t count = 0;

    uint64_t totalIndices = 0;

    bool IsLeaf() const
    {
        return left == -1;
    }
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
    uint32_t  m_shadowdrawCalls = 0;
    uint32_t m_shadowDrawCallsCulled = 0;


    uint64_t culledIndices = 0;
    uint64_t renderedIndices = 0;
    uint32_t zeroIndexSubsets = 0;

    bool lightsEnabled = true;

    double shadowCpuMs = 0.0;
};



struct MapMeshSubset
{
    UINT startIndex = 0;
    UINT indexCount = 0;
    unsigned int materialIndex;

    MaterialType materialType = MaterialType::MATERIAL_DEFAULT;
    DirectX::BoundingBox bounds;

    const MaterialData* resolvedMaterial = nullptr;
};


struct MapRenderBatch
{
    UINT startIndex = 0;
    UINT indexCount = 0;
    int cellX = 0;
    int cellY = 0;
    int cellZ = 0;

    uint32_t chunkIndex = UINT32_MAX;
    UINT chunkStartIndex = 0;

    const MaterialData *resolvedMaterial = nullptr;
    MaterialType materialType = MaterialType::MATERIAL_DEFAULT;
    unsigned int materialIndex = 0;

    DirectX::BoundingBox bounds;
};


struct MapGeometryChunk
{
    // Spatial identity
    int cellX = 0;
    int cellY = 0;
    int cellZ = 0;

    DirectX::BoundingBox bounds{};

    // GPU resources owned by this chunk
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    std::vector<uint32_t> batchIndices;

    // CPU geometry retained for future uploads
    std::vector<SharedVertex> vertices;
    std::vector<UINT> indices;

    // Residency state
    bool resident = false;

    void Unload()
    {
        vertexBuffer.Reset();
        indexBuffer.Reset();

        resident = false;
    }
};


struct MapMaterialKey
{
    ID3D11ShaderResourceView *diffuse;
    ID3D11ShaderResourceView *normal;
    ID3D11ShaderResourceView *detail;
    ID3D11ShaderResourceView *normalDetail;
    ID3D11ShaderResourceView *maps;

    float materialType;
    float hasDiffuseTexture;
    float hasDetailTexture;
    float hasDiffuseColor;
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
    const std::vector<UINT>& GetCollisionIndices() const { return m_collisionIndices; }
    const std::vector<MapMarker>& GetMarkers() const { return m_markers; }
    
    void ResolveMaterials();

    void Draw(ID3D11DeviceContext* context,
        ID3D11Buffer* cbb,
        ID3D11Buffer* lampInfoBuffer,
        ID3D11ShaderResourceView* lampLightsSRV,
        const SharedSceneData& engineSceneData,
        const DirectX::BoundingFrustum& frustum,
        ID3D11DepthStencilState* depthWriteOn,
        const DirectX::XMFLOAT3 &cameraPosition);

    void DrawShadow(
        ID3D11DeviceContext* context,
        ID3D11Buffer* cbb,
        const SharedSceneData& engineSceneData,
        const DirectX::BoundingFrustum& frustum,
        ID3D11DepthStencilState* depthWriteOn,
        ID3D11PixelShader* shadowAlphaPS);


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

    void SetMaterialLoader(SharedMaterialLoader& materialLoader);
    const std::string& GetMaterialLibraryPath() const { return m_materialLibraryPath; }

    void SetMaterialLibraryPath(const std::string& path)
    {
        m_materialLibraryPath = path;
    }

    void DrawShadowBVHNode(
        int nodeIndex,
        ID3D11DeviceContext* context,
        const DirectX::BoundingFrustum& frustum,
        ID3D11PixelShader *shadowAlphaPS
    );

    void CollectVisibleBVH(
        int nodeIndex,
        const DirectX::BoundingFrustum& frustum,
        std::vector<uint32_t>& visible
    );
    
    
     std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_materialSRVs;

     std::vector<BVHNode> m_bvhNodes;
     std::vector<uint32_t> m_bvhIndices;

     int BuildBVHNode(uint32_t first, uint32_t count);
     void BuildBVH();


 private:

   void UpdateChunkResidency(const DirectX::XMFLOAT3 &cameraPosition);
   bool UploadChunk(MapGeometryChunk &chunk);
   static float DistanceToChunk(const DirectX::XMFLOAT3 &position, const DirectX::BoundingBox &bounds);
   void BuildGeometryChunks(const std::vector<UINT> &batchIndices);
   Microsoft::WRL::ComPtr<ID3D11Device> m_batchDevice;
   Microsoft::WRL::ComPtr<ID3D11Buffer> m_batchIndexBuffer;
   std::vector<MapGeometryChunk> m_geometryChunks;
   std::vector<MapRenderBatch> m_renderBatches;
   std::vector<BVHNode> m_batchBVHNodes;
   std::vector<uint32_t> m_batchBVHIndices;

   void BuildRenderBatches();
   void BuildBatchBVH();
   int BuildBatchBVHNode(uint32_t first, uint32_t count);

   void CollectVisibleBatchBVH(
       int nodeIndex,
       const DirectX::BoundingFrustum &frustum,
       std::vector<uint32_t> &visible);

     void ProcessNode(
         const aiNode* node,
         const aiScene* scene,
         const aiMatrix4x4& parentTransform);

     void DrawShadowSubset(
         ID3D11DeviceContext *context,
         const MapMeshSubset &subset,
         ID3D11PixelShader *shadowAlphaPS);


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

     std::vector<SharedVertex> m_allVertices;
     std::vector<UINT> m_allIndices;
     std::vector<UINT> m_collisionIndices;

     std::vector<MapMarker> m_markers;
     std::vector<LampLight> m_visibleLampLights;
     std::vector<LampLight> m_lampLights;

     MapRendererStats m_stats;
     bool m_lightsEnabled = true;


     SharedMaterialLoader* m_materialLoader = nullptr;
     std::vector<MapMeshSubset> m_mapSubsets;
     std::vector<std::string> m_mapMaterialNames;
     std::string m_materialLibraryPath;

};