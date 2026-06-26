#pragma once

#include "../SharedTypes.h"
#include <DirectXMath.h>

using namespace DirectX;

class Model; // Forward declaration

enum class CameraMode {
    FREE_ROAM,
    CHASE,
    ROOF,
    BUMPER
};

class Camera {
public:
    Camera();
    void Update(float deltaTime); 
    void SetPosition(float x, float y, float z) { position = { x, y, z }; }
    void AdjustPosition(float x, float y, float z) { rotation = { x, y, z }; }// For movement
    void AdjustRotation(float dx, float dy);    // For looking around
    void SetFollowTarget(Model* target) { m_targetModel = target; }
    void CycleCameraMode(); // This will replace ToggleChaseCam
    void ReverseCamera(bool active);
    bool IsInManualMode() const { return m_currentMode == CameraMode::FREE_ROAM; }
    bool m_isLookingBack = false;
    DirectX::XMMATRIX GetViewMatrix() { return viewMatrix; }
    DirectX::XMMATRIX GetProjectionMatrix() { return projectionMatrix; }

    DirectX::XMFLOAT3 GetPosition() const { 
        return this->position; // Or whatever your variable name is
    }
    DirectX::XMVECTOR GetForwardVector() {
        DirectX::XMFLOAT4X4 mat;
        XMStoreFloat4x4(&mat, viewMatrix);
        return DirectX::XMVectorSet(mat._13, mat._23, mat._33, 0.0f);
    }
    DirectX::XMMATRIX viewMatrix;
    DirectX::XMMATRIX projectionMatrix;
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 rotation;
    float height = 0.0f;
    float distance = 0.0f;
    float pitchDeg = 0.0f;

    void SetVehicleCameraDefinition(const CameraDefinition& cameraDef);
private:
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    float m_moveSpeed = 5.0f;
    float m_mouseSensitivity = 0.005f;
    float yVelocity = 0.0f;
    float gravity = -9.81f;
    float groundLevel = 1.0f;
    Model* m_targetModel = nullptr;
    CameraMode m_currentMode = CameraMode::CHASE;
    DirectX::XMFLOAT3 m_pos = { 0.0f, 0.0f, -5.0f };
    DirectX::XMVECTOR m_posVector;
    CameraDefinition m_camera;
    

};