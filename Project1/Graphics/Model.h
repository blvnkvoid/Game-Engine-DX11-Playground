#pragma once


#include "../SharedTypes.h"
#include <fstream>
#include <sstream>
#include <wrl/client.h> // The home of ComPtr
#include "../Scene/Camera.h"
#include <map>
#include <d3d11.h>          // Added this for ID3D11 types

class TextureManager;

struct MeshSubset {
    SharedMaterial material;
    UINT startIndex = 0;
    UINT indexCount = 0;
    UINT materialIndex;
    std::string materialName;
    MeshSubset(unsigned int start = 0) : startIndex(start), indexCount(0) {};
    ID3D11ShaderResourceView* diffuseTexture = nullptr;
    std::string diffuseTextureName;
};

class Model {
public:
    Model();
    void BindAndDraw(ID3D11DeviceContext* context, UINT stride, DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, Camera* cam);
    void BindTexture(ID3D11DeviceContext* context);
    void SetModelPosition(float x, float y, float z) { modelposition = { x, y, z }; }
    void SetModelRotation(DirectX::XMMATRIX rotation) { m_rotationMatrix = rotation; }
    bool LoadOBJ(const std::string& mtlFile, const std::string& objFile, ID3D11Device* device);
    bool LoadTexture(ID3D11Device* device, std::wstring filename);
    const SharedMaterial& GetMaterial() const { return m_material; }
    const std::vector<MeshSubset>& GetSubsets() const { return m_subsets; }
    UINT GetIndexCount() { return index_count; }
    ID3D11ShaderResourceView* GetSRV() { return m_textureRV.Get(); } 
    SharedMaterial m_material;
    XMMATRIX GetModelWorldMatrix() { return m_rotationMatrix * XMMatrixTranslation(modelposition.x, modelposition.y, modelposition.z); }
    DirectX::XMMATRIX GetModelRotation() const { return m_rotationMatrix; }
    DirectX::XMFLOAT3 GetModelPosition() const { return modelposition; }
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;        
    std::map<std::string, MaterialData> m_materialLib;
    void ResolveMaterialTextures(TextureManager* textureManager, ID3D11DeviceContext* context, const std::wstring& textureFolder);

private:
    UINT index_count;
    std::vector<MeshSubset> m_subsets;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureRV;    
    DirectX::XMMATRIX m_rotationMatrix = DirectX::XMMatrixIdentity();
    DirectX::XMFLOAT3 modelposition = { 0.0f, 0.0f, 0.0f };
};