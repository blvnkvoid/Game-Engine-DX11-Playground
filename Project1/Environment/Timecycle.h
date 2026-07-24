#pragma once
#include "../SharedTypes.h"
#include "../Environment/Time.h"
#include <DirectXMath.h>

class Sun;
class Camera;
class Clouds;

struct EnvironmentState
{
    float clearColor[4] = { 0.00f, 0.00f, 0.0f, 0.0f }; //Just init data, touching changes nothing

    DirectX::XMFLOAT4 lightDirection;
    DirectX::XMFLOAT4 lightColor;

    float ambientIntensity = 0.0f;    
    float headlightIntensity = 1.0f;
    float sunSize = 250.0f;
    float sunIntensity = 1.0f;
};

class TimeCycle
{
public:
    void Update(float time, EnvironmentState& state) const;

    const DirectX::XMFLOAT4 sunriseDir =
    {
        -0.95f,
        -0.15f,
         0.25f,
         0.0f
    };

    const DirectX::XMFLOAT4 dayDir =
    {
        -0.35f,
        -0.95f,
         0.20f,
         0.0f
    };

    const DirectX::XMFLOAT4 sunsetDir =
    {
         0.95f,
        -0.15f,
         0.25f,
         0.0f
    };

    const DirectX::XMFLOAT4 nightDir =
    {
         0.35f,
         0.95f,
        -0.20f,
         0.0f
    };

    const DirectX::XMFLOAT4 midnightDir =
    {
         0.00f,
         1.00f,
        -0.30f,
         0.0f
    };

    DirectX::XMFLOAT4 Normalize4(const DirectX::XMFLOAT4& v) const
    {
        DirectX::XMVECTOR vec =
            DirectX::XMLoadFloat4(&v);

        vec = DirectX::XMVector3Normalize(vec);

        DirectX::XMFLOAT4 result;
        DirectX::XMStoreFloat4(&result, vec);

        result.w = 0.0f;
        return result;
    }

    const float sunriseSky[4] = { 0.52f, 0.38f, 0.28f, 1.0f };
    const float daySky[4] = { 0.65f, 0.75f, 1.00f, 1.0f };
    const float sunsetSky[4] = { 0.65f, 0.42f, 0.22f, 1.0f };
    const float nightSky[4] = { 0.02f, 0.025f, 0.04f, 1.0f };
    const float midnightSky[4] = { 0.01f, 0.015f, 0.03f, 1.0f };

    /*const DirectX::XMFLOAT3 sunriseCloudColor = { 1.00f, 0.88f, 0.78f };
    const DirectX::XMFLOAT3 dayCloudColor = { 1.00f, 1.00f, 1.00f };
    const DirectX::XMFLOAT3 sunsetCloudColor = { 1.00f, 0.72f, 0.56f };
    const DirectX::XMFLOAT3 nightCloudColor = { 0.38f, 0.42f, 0.55f };
    const DirectX::XMFLOAT3 midnightCloudColor = { 0.24f, 0.28f, 0.38f };*/

    const DirectX::XMFLOAT3 sunriseCloudColor = { 0.78f, 0.66f, 0.58f };
    const DirectX::XMFLOAT3 dayCloudColor = { 0.82f, 0.88f, 0.95f };
    const DirectX::XMFLOAT3 sunsetCloudColor = { 0.76f, 0.58f, 0.46f };
    const DirectX::XMFLOAT3 nightCloudColor = { 0.10f, 0.12f, 0.16f };
    const DirectX::XMFLOAT3 midnightCloudColor = { 0.06f, 0.08f, 0.11f };

    DirectX::XMFLOAT4 sunriseLight = { 0.95f, 0.78f, 0.62f, 1.0f };
    DirectX::XMFLOAT4 dayLight = { 1.00f, 1.00f, 1.00f, 1.0f };
    DirectX::XMFLOAT4 sunsetLight = { 1.00f, 0.72f, 0.42f, 1.0f };
    DirectX::XMFLOAT4 nightLight = { 0.22f, 0.24f, 0.28f, 1.0f };
    DirectX::XMFLOAT4 midnightLight = { 0.12f, 0.14f, 0.18f, 1.0f };

    const float sunriseAmbient = 0.22f;
    const float dayAmbient = 0.22f;
    const float sunsetAmbient = 0.22f;
    const float nightAmbient = 0.12f;
    const float midnightAmbient = 0.18f;

    const float sunriseHeadlight = 0.7f;
    const float noonHeadlight = 0.2f;
    const float sunsetHeadlight = 0.7f;
    const float nightHeadlight = 1.0f;
    const float midnightHeadlight = 1.0f;

    const float sunriseCloudBrightness = 0.4f;
    const float dayCloudBrightness = 1.0f;
    const float sunsetCloudBrightness = 0.7f;
    const float nightCloudBrightness = 0.2f;
    const float midnightCloudBrightness = 0.05f;






    static DirectX::XMFLOAT3 Lerp3(
        const DirectX::XMFLOAT3& a,
        const DirectX::XMFLOAT3& b,
        float t)
    {
        return DirectX::XMFLOAT3(
            Lerp(a.x, b.x, t),
            Lerp(a.y, b.y, t),
            Lerp(a.z, b.z, t));
    }

    static float Lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    float Saturate(float x) const
    {
        if (x < 0.0f)
            return 0.0f;

        if (x > 1.0f)
            return 1.0f;

        return x;
    }

    float SmoothStep(float edge0, float edge1, float x) const
    {
        x = Saturate((x - edge0) / (edge1 - edge0));
        return x * x * (3.0f - 2.0f * x);
    }

    static DirectX::XMFLOAT4 Lerp4(DirectX::XMFLOAT4 a, DirectX::XMFLOAT4 b, float t)
    {
        return {
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t,
            a.w + (b.w - a.w) * t
        };
    }

    static void LerpColor4(float out[4], const float a[4], const float b[4], float t)
    {
        out[0] = a[0] + (b[0] - a[0]) * t;
        out[1] = a[1] + (b[1] - a[1]) * t;
        out[2] = a[2] + (b[2] - a[2]) * t;
        out[3] = a[3] + (b[3] - a[3]) * t;
    }

    float GetCycleLength() const
    {
        return 240.0f;
    }


    void UpdateSun(
        const Time& time,
        Camera* cam,
        Sun& sun);

    void UpdateClouds(
        float time,
        Clouds& clouds
    );

private:
    float m_sunrise = 0.0f;
    float m_noon = 60.0f;
    float m_sunset = 120.0f;
    float m_night = 180.0f;
    float m_midnight = 210.0f;
    float m_cycleEnd = 240.0f;
};