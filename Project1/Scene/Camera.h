#pragma once

#include "../SharedTypes.h"
#include <DirectXMath.h>
#include "../UI/Settings.h"
#include <DirectXCollision.h>
#include "../Tracks/TrackTable.h"

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
    void Update(float deltaTime, const TrackEntry& activeTrackEntry);
    void SetPosition(float x, float y, float z) { position = { x, y, z };     m_posVector = DirectX::XMLoadFloat3(&position);
    }
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

    float m_farPlane;

    float m_nearPlane;

    float GetFarPlane() const
    {
        return m_farPlane;
    }    
    
    float GetNearPlane() const
    {
        return m_nearPlane;
    }

    DirectX::XMMATRIX viewMatrix;
    DirectX::XMMATRIX projectionMatrix;
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 rotation;
    float height = 0.0f;
    float distance = 0.0f;
    float pitchDeg = 0.0f;
    Settings settings;
    TrackEntry track;

    void SetVehicleCameraDefinition(const CameraDefinition& cameraDef);

    void UpdateFrustum();

    void DebugFrustum() const;

    const DirectX::BoundingFrustum& GetFrustum() const
    {
        return m_worldFrustum;
    }

    const XMVECTOR GetForward() const
    {
        return m_forward;
    }

    const XMVECTOR GetRight() const
    {
        return m_right;
    }

    const XMVECTOR GetUp() const
    {
        return m_up;
    }
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
    DirectX::BoundingFrustum m_viewFrustum;
    DirectX::BoundingFrustum m_worldFrustum;
    XMVECTOR m_forward;
    XMVECTOR m_right;
    XMVECTOR m_up;

};