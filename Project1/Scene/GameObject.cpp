#include "GameObject.h"

GameObject::GameObject(UINT vertex_size) : vertex_buffer(nullptr), index_buffer(nullptr), index_count(0), stride(vertex_size), m_model(nullptr) {
    world_matrix = XMMatrixIdentity();
    this->stride = vertex_size;
}

GameObject::~GameObject() {
}

void GameObject::Draw(ID3D11DeviceContext* context) {


}

XMMATRIX GameObject::GetWorldMatrix() {
    return rotation_matrix * world_matrix;
}


