#pragma once
#include <DirectXMath.h>
#include <d3d11.h>

using namespace DirectX;

class SkyEngine;
class GraphicsEngine;
class Camera; 

class Clouds
{
public:


    XMFLOAT2 windDirection = { 1.0f, 0.0f };



    float m_speed = 0.02f;
    float m_coverage = 0.45f;
    float m_density = 1.0f;
    float m_scale = 0.5f;
    float m_brightness = 1.0f;


    XMFLOAT3 m_cloudColor = { 1.0f, 1.0f, 1.0f };
    XMFLOAT3 m_cloudshadowColor = { 0.7f, 0.7f, 0.75f };

    Clouds() = default;
    ~Clouds() = default;

    void Render(
        ID3D11DeviceContext* context,
        SkyEngine& skyEngine,
        GraphicsEngine& graphicsEngine,
        Camera* camera);


};