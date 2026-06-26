#pragma once


#include "../Graphics/Model.h"

class alignas(16) GameObject {
protected:
    ID3D11ShaderResourceView* m_textureRV = nullptr;
    Model* m_model = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
    DirectX::XMMATRIX world_matrix;
    UINT index_count;
    UINT stride;
public:
    void Draw(ID3D11DeviceContext* context);
    void SetModel(Model* model) { m_model = model; }
    void SetMaterial(SharedMaterial mat) { m_material = mat; }
    void SetWorldMatrix(DirectX::XMMATRIX matrix) { world_matrix = matrix; }
    void SetRotationMatrix(DirectX::XMMATRIX rotationMatrix) { this->rotation_matrix = rotationMatrix; }
    virtual void Update(float delta_time) {};
    virtual ~GameObject(); // Virtual is crucial for base classes!
    virtual ID3D11ShaderResourceView* GetTexture() {
        if (m_model) {
            return m_model->GetSRV(); // We need to add this helper to Model.h
        }
        return m_textureRV;
    }
    GameObject(UINT vertex_size);
    Model* GetModel() { return m_model; }
    DirectX::XMMATRIX GetWorldMatrix();
    UINT GetIndexCount() const { return index_count; }
    SharedMaterial GetMaterial() { return m_material; }
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const { return vertex_buffer; }
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() const { return index_buffer; }
    DirectX::XMMATRIX rotation_matrix = XMMatrixIdentity();;
    SharedMaterial m_material;    
};
