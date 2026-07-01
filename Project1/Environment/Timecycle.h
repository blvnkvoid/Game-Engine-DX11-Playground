#pragma once
#include "../SharedTypes.h"
#include "../Environment/Time.h"


struct EnvironmentState
{
    float clearColor[4] = { 0.65f, 0.75f, 1.0f, 1.0f };

    DirectX::XMFLOAT4 lightDirection;
    DirectX::XMFLOAT4 lightColor;

    float ambientIntensity = 0.0f;
    
    float headlightIntensity = 1.0f;
};

class TimeCycle
{
public:
    void Update(float time, EnvironmentState& state) const;



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



private:
    float m_sunrise = 0.0f;
    float m_noon = 60.0f;
    float m_sunset = 120.0f;
    float m_night = 180.0f;
    float m_midnight = 210.0f;
    float m_cycleEnd = 240.0f;
};