 #include "TimeCycle.h"
#include <algorithm>
#include "../Sky/Sun.h"
#include "../Scene/Camera.h"

using namespace DirectX;
void TimeCycle::Update(float time, EnvironmentState& state) const
{
    const float sunriseSky[4] = { 0.52f, 0.38f, 0.28f, 1.0f };
    const float daySky[4] = { 0.65f, 0.75f, 1.00f, 1.0f };
    const float sunsetSky[4] = { 0.65f, 0.42f, 0.22f, 1.0f };
    const float nightSky[4] = { 0.02f, 0.025f, 0.04f, 1.0f };
    const float midnightSky[4] = { 0.01f, 0.015f, 0.03f, 1.0f };

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

    float angle = (time / 240.0f) * XM_2PI;

    state.lightDirection =
    {
        cosf(angle),
        -sinf(angle),
        0.25f,
        0.0f
    };

    float t = 0.0f;

    if (time < m_noon)
    {
        // Sunrise -> Day
        t = SmoothStep(m_sunrise, m_noon, time);
        LerpColor4(state.clearColor, sunriseSky, daySky, t);
        state.lightColor = Lerp4(sunriseLight, dayLight, t);
        state.ambientIntensity = Lerp(sunriseAmbient, dayAmbient, t);
        state.headlightIntensity = Lerp(sunriseHeadlight, noonHeadlight, t);
    }
    else if (time < m_sunset)
    {
        // Day -> Sunset
        t = SmoothStep(m_noon, m_sunset, time);
        LerpColor4(state.clearColor, daySky, sunsetSky, t);
        state.lightColor = Lerp4(dayLight, sunsetLight, t);
        state.ambientIntensity = Lerp(dayAmbient, sunsetAmbient, t);
        state.headlightIntensity = Lerp(noonHeadlight, sunsetHeadlight, t);
    }
    else if (time < m_night)
    {
        // Sunset -> Night
        t = SmoothStep(m_sunset, m_night, time);
        LerpColor4(state.clearColor, sunsetSky, nightSky, t);
        state.lightColor = Lerp4(sunsetLight, nightLight, t);
        state.ambientIntensity = Lerp(sunsetAmbient, nightAmbient, t);
        state.headlightIntensity = Lerp(sunsetHeadlight, nightHeadlight, t);
    }
    else if (time < m_midnight)
    {
        // Night -> Midnight
        t = SmoothStep(m_night, m_midnight, time);
        LerpColor4(state.clearColor, nightSky, midnightSky, t);
        state.lightColor = Lerp4(nightLight, midnightLight, t);
        state.ambientIntensity = Lerp(nightAmbient, midnightAmbient, t);
        state.headlightIntensity = Lerp(nightHeadlight,midnightHeadlight , t);
    }
    else
    {
        // Midnight -> Sunrise
        t = SmoothStep(m_midnight, m_cycleEnd, time);
        LerpColor4(state.clearColor, midnightSky, sunriseSky, t);
        state.lightColor = Lerp4(midnightLight, sunriseLight, t);
        state.ambientIntensity = Lerp(midnightAmbient, sunriseAmbient, t);
        state.headlightIntensity = Lerp(midnightHeadlight,sunriseHeadlight , t);
    }
}





void TimeCycle::UpdateSun(
    const Time& time,
    Camera* cam,
    Sun& sun)
{
    float cycleTime = fmod(
        time.GetTime(),
        m_cycleEnd);

    if (cycleTime < 0.0f)
        cycleTime += m_cycleEnd;

    //-----------------------------------------------------
    // Sun orbit
    //-----------------------------------------------------

    // Sunrise = eastern horizon
    // Noon    = directly overhead
    // Sunset  = western horizon
    //
    // 0..120 seconds maps to 0..PI radians.

    float daylightProgress =
        std::clamp(
            (cycleTime - m_sunrise) /
            (m_sunset - m_sunrise),
            0.0f,
            1.0f);

    float sunArc =
        sinf(daylightProgress * DirectX::XM_PI);

    // 3° at sunrise/sunset, 10° at noon.
    float pitchOffset =
        DirectX::XMConvertToRadians(
            Lerp(3.0f, 10.0f, sunArc));

    // -30° (left) -> +30° (right)
    float yawOffset =
        DirectX::XMConvertToRadians(
            Lerp(-30.0f, 30.0f, daylightProgress));

    float horizontal =
        Lerp(-0.5f, 0.5f, daylightProgress);

    float vertical =
        Lerp(0.05f, 0.18f, sunArc);

    DirectX::XMVECTOR forward =
        DirectX::XMVector3Normalize(
            cam->GetForward());

    DirectX::XMVECTOR right =
        DirectX::XMVector3Normalize(
            cam->GetRight());

    DirectX::XMVECTOR up =
        DirectX::XMVectorSet(
            0.0f,
            1.0f,
            0.0f,
            0.0f);

    // Lift the base direction slightly so the sun
    // naturally sits above the horizon.
    forward =
        DirectX::XMVectorSetY(
            forward,
            0.15f);

    forward =
        DirectX::XMVector3Normalize(
            forward);

    float horizontalScale = 1.0f;

    // Camera-relative offsets.
    DirectX::XMVECTOR sunDirection =
    XMVector3Normalize(
        forward -
        right * horizontal 
        *1.0f * vertical * horizontalScale);



    sunDirection =
        DirectX::XMVector3Normalize(
            sunDirection);

    DirectX::XMStoreFloat3(
        &sun.m_Direction,
        sunDirection);




    //-----------------------------------------------------
    // Visibility
    //-----------------------------------------------------

    sun.m_Visible =
        cycleTime >= m_sunrise &&
        cycleTime <= 180.0f;

    //-----------------------------------------------------
    // Color and intensity from elevation
    //-----------------------------------------------------

    float colorelevation =
        max(sin(sunArc), 0.0f);

    float daylight =
        SmoothStep(0.0f, 0.35f, colorelevation);

    sun.m_Intensity =
        Lerp(
            0.30f,
            0.75f,
            daylight);

    sun.m_ColorTint =
    {
        Lerp(0.75f, 1.00f, daylight),
        Lerp(0.35f, 0.985f, daylight),
        Lerp(0.25f, 0.94f, daylight)
    };
}