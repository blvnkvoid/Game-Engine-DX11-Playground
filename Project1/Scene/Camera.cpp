    #include "Camera.h"
    #include "GameObject.h"
    
    Camera::Camera()
    {
        position = { 0.0f, 0.0f, -1.0f };
        rotation = { 0.0f, 0.0f,  0.0f };
        m_posVector = XMLoadFloat3(&position);
        m_yaw = 0.0f;
        m_pitch = 0.0f;
    }

    void Camera::SetVehicleCameraDefinition(const CameraDefinition& cameraDef)
    {
        m_camera = cameraDef;
    }

    void Camera::CycleCameraMode() {
        m_currentMode = static_cast<CameraMode>((static_cast<int>(m_currentMode) + 1) % 4);
        const char* modeNames[] = { "FREE ROAM", "CHASE", "ROOF", "BUMPER" };
        switch (m_currentMode) {
        case CameraMode::CHASE:
            height = m_camera.chaseHeight; distance = m_camera.chaseDistance; pitchDeg = m_camera.chasePitchDeg;
            break;
        case CameraMode::ROOF:
            height = m_camera.roofHeight; distance = m_camera.roofDistance;  pitchDeg = m_camera.roofPitchDeg;
            break;
        case CameraMode::BUMPER:
            height = m_camera.bumperHeight; distance = m_camera.bumperDistance;  pitchDeg = m_camera.bumperPitchDeg;
            break;
        }
    }

    void Camera::ReverseCamera(bool active)
    {
        m_isLookingBack = active;
    }


    void Camera::Update(float deltaTime) {

        XMMATRIX camRotation = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0);
        XMVECTOR lookDir = XMVector3TransformCoord(XMVectorSet(0, 0, 1, 0), camRotation);
        XMVECTOR Up = XMVector3TransformCoord(XMVectorSet(0, 1, 0, 0), camRotation);
        XMVECTOR rightDir = XMVector3Cross(Up, lookDir);
        XMVECTOR vLookAt = XMVectorSet(0, 0, 0, 0);
        XMVECTOR vCarUp = XMVectorSet(0, 1, 0, 0); // Default world up

        static XMVECTOR smoothedUp = XMVectorSet(0, 1, 0, 0);

        float lerpFactor = 75.0f; // Adjust this: 1.0 is a hard snap, 0.05 is very lazy
    

        float targetFOV = XM_PIDIV4; // Default 45 degrees

        static bool cWasPressed = false; // This remembers if we were holding 'C' last frame

        if (GetAsyncKeyState('C') & 0x8000) {
            if (!cWasPressed) {             // Only fire on the INITIAL press
                CycleCameraMode();
                cWasPressed = true;         // Lock the gate
            }
        }
        else {
            cWasPressed = false;            // Unlock when finger leaves key
        }

        if (m_currentMode == CameraMode::FREE_ROAM) {
       
            if (GetAsyncKeyState('W')) m_posVector += lookDir * m_moveSpeed * deltaTime;
            if (GetAsyncKeyState('S')) m_posVector -= lookDir * m_moveSpeed * deltaTime;
            if (GetAsyncKeyState('A')) m_posVector -= rightDir * m_moveSpeed * deltaTime;
            if (GetAsyncKeyState('D')) m_posVector += rightDir * m_moveSpeed * deltaTime;

            if (GetAsyncKeyState(VK_SPACE) && XMVectorGetY(m_posVector) <= groundLevel + 0.01f) {
                yVelocity = 145.0f; // Jump strength
            }

            if (XMVectorGetY(m_posVector) > groundLevel || yVelocity > 0) {
                yVelocity += gravity * deltaTime;
                // Apply vertical movement (Up is Y+)
                m_posVector += XMVectorSet(0, yVelocity * deltaTime, 0, 0);
            }

            // Floor Collision
            if (XMVectorGetY(m_posVector) < groundLevel) {
                m_posVector = XMVectorSetY(m_posVector, groundLevel);
                yVelocity = 0.0f;
            }
            vLookAt = m_posVector + lookDir;
        }   

       else if (m_targetModel) {
            // --- CHASE MODE: THE SNAP ---
            DirectX::XMFLOAT3 carPos = m_targetModel->GetModelPosition();
            XMMATRIX mCarRot = m_targetModel->GetModelRotation();
            XMVECTOR vCarPos = XMLoadFloat3(&carPos);
            XMVECTOR vCarForward = mCarRot.r[2]; // Row 2 is typically Forward in DX
            vCarUp = mCarRot.r[1];      // Row 1 is Up
      

            float lookDirection = m_isLookingBack ? -1.0f : 1.0f;

            smoothedUp = XMVectorLerp(smoothedUp, vCarUp, 10.0f * deltaTime);

            XMVECTOR targetPos = vCarPos + (vCarForward * (distance/4) * lookDirection) + (vCarUp * height);       


            switch (m_currentMode) {
            case CameraMode::CHASE:
                targetFOV = XMConvertToRadians(45.0f); // Slightly wider
                vLookAt = vCarPos + (vCarForward * (5.0f * lookDirection));
                m_posVector = XMVectorLerp(m_posVector, targetPos, 300.0f * deltaTime);
                break;
            case CameraMode::ROOF:
                targetFOV = XMConvertToRadians(52.0f); // Slightly wider
                vLookAt = vCarPos + (vCarForward * (5.0f * lookDirection));
                m_posVector = XMVectorLerp(m_posVector, targetPos, 75.0f * deltaTime);
                break;
            case CameraMode::BUMPER:
                targetFOV = XMConvertToRadians(52.0f); // Slightly wider
                vLookAt = vCarPos + (vCarForward * (5.0f * lookDirection));
                break;
            }


            m_posVector = XMVectorLerp(m_posVector, targetPos, lerpFactor * deltaTime);
            m_pitch = m_isLookingBack ? 0.0f : XMConvertToRadians(pitchDeg);
            m_yaw = 0.0f;

        }
       else {
            // Fallback: Just in case there's no model and not in Free Roam
            vLookAt = m_posVector + lookDir;
        }

        XMStoreFloat3(&this->position, m_posVector);
        this->viewMatrix = XMMatrixLookAtLH(m_posVector, vLookAt, smoothedUp);
        this->projectionMatrix = XMMatrixPerspectiveFovLH(targetFOV, 1920.0f / 1080.0f, 1.0f, 1000.0f);        
    }

    void Camera::AdjustRotation(float dx, float dy) {
        m_yaw += dx * m_mouseSensitivity;
        m_pitch += dy * m_mouseSensitivity;

        // Limit pitch so you don't flip your neck 360 degrees
        if (m_pitch > XM_PIDIV2 - 0.1f) m_pitch = XM_PIDIV2 - 0.1f;
        if (m_pitch < -XM_PIDIV2 + 0.1f) m_pitch = -XM_PIDIV2 + 0.1f;


        XMMATRIX camRotation = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0);
        XMVECTOR lookDir = XMVector3TransformCoord(XMVectorSet(0, 0, 1, 0), camRotation);
        XMVECTOR Up = XMVector3TransformCoord(XMVectorSet(0, 1, 0, 0), camRotation);

        this->viewMatrix = XMMatrixLookAtLH(m_posVector, m_posVector + lookDir, Up);
    }