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
    UINT startIndex = 0;
    UINT indexCount = 0;
    UINT materialIndex;
    std::string materialName;
    MeshSubset(unsigned int start = 0) : startIndex(start), indexCount(0) {};
};

class CarLoader {
public:
    CarLoader();
    void BindAndDraw(ID3D11DeviceContext* context, UINT stride, DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, ID3D11DepthStencilState* depthWriteOn,
        ID3D11DepthStencilState* depthWriteOff, ID3D11BlendState* alphaBlendState, ID3D11Buffer* lampInfoBuffer, ID3D11ShaderResourceView* lampLightsSRV, const SharedSceneData& sceneData);
    void SetModelPosition(float x, float y, float z) { modelposition = { x, y, z }; }
    void SetModelRotation(DirectX::XMMATRIX rotation) { m_rotationMatrix = rotation; }
    bool LoadOBJ(const std::string& objFile, ID3D11Device* device);
    const std::vector<MeshSubset>& GetSubsets() const { return m_subsets; }
    UINT GetIndexCount() { return index_count; }
    // TODO: Legacy texture path.
    // Remove/rework after SharedMaterialLoader owns material texture binding.
    ID3D11ShaderResourceView* GetSRV() { return m_textureRV.Get(); }
    XMMATRIX GetModelWorldMatrix() { return m_rotationMatrix * XMMatrixTranslation(modelposition.x, modelposition.y, modelposition.z); }
    DirectX::XMMATRIX GetModelRotation() const { return m_rotationMatrix; }
    DirectX::XMFLOAT3 GetModelPosition() const { return modelposition; }
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
private:
    UINT index_count;
    std::vector<MeshSubset> m_subsets;
    // TODO: Legacy texture path - still consumed through GameObject/GraphicsEngine.
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureRV;
    DirectX::XMMATRIX m_rotationMatrix = DirectX::XMMatrixIdentity();
    DirectX::XMFLOAT3 modelposition = { 0.0f, 0.0f, 0.0f };
};