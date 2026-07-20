#pragma once

#include <DirectXMath.h>
#include "../Environment/Timecycle.h"
#include <d3d11.h>
#include <wrl/client.h>
#include "../Scene/Camera.h"
#include "SkyEngine.h"

class Sun
{
public:
    


    void Render(ID3D11DeviceContext* context,
        Camera* camera,
        SkyEngine& skyEngine);

    bool m_Visible = true;

    float m_AngularSize = 0.53f;      // degrees (real sun)
    float m_Distance = 3500.0f;
    float m_Intensity = 0.3f;

    float m_YawOffset = 0.0f;
    float m_PitchOffset = 0.0f;

    DirectX::XMFLOAT3 m_ColorTint =
    {
    0.75f,
    0.35f,
    0.25f
    };

    DirectX::XMFLOAT3 m_Direction =
    {
        0.6f,
        0.4f,
        0.7f
    };
};