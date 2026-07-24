 #include "TimeCycle.h"
#include <algorithm>
#include "../Sky/Sun.h"
#include "../Sky/Clouds.h"
#include "../Scene/Camera.h"

using namespace DirectX;

void TimeCycle::Update(float time, EnvironmentState& state) const
{
    float t = 0.0f;

    if (time < m_noon)
    {
        // Sunrise -> Day
        t = SmoothStep(m_sunrise, m_noon, time);
        LerpColor4(state.clearColor, sunriseSky, daySky, t);
        state.lightColor = Lerp4(sunriseLight, dayLight, t);
        state.ambientIntensity = Lerp(sunriseAmbient, dayAmbient, t);
        state.headlightIntensity = Lerp(sunriseHeadlight, noonHeadlight, t);
        state.lightDirection = Normalize4(Lerp4(sunriseDir, dayDir, t));

      
    }
    else if (time < m_sunset)
    {
        // Day -> Sunset
        t = SmoothStep(m_noon, m_sunset, time);
        LerpColor4(state.clearColor, daySky, sunsetSky, t);
        state.lightColor = Lerp4(dayLight, sunsetLight, t);
        state.ambientIntensity = Lerp(dayAmbient, sunsetAmbient, t);
        state.headlightIntensity = Lerp(noonHeadlight, sunsetHeadlight, t);
        state.lightDirection = Normalize4(Lerp4(dayDir, sunsetDir, t));
    }
    else if (time < m_night)
    {
        // Sunset -> Night
        t = SmoothStep(m_sunset, m_night, time);
        LerpColor4(state.clearColor, sunsetSky, nightSky, t);
        state.lightColor = Lerp4(sunsetLight, nightLight, t);
        state.ambientIntensity = Lerp(sunsetAmbient, nightAmbient, t);
        state.headlightIntensity = Lerp(sunsetHeadlight, nightHeadlight, t);
        state.lightDirection = Normalize4(Lerp4(sunsetDir, nightDir, t));
    }
    else if (time < m_midnight)
    {
        // Night -> Midnight
        t = SmoothStep(m_night, m_midnight, time);
        LerpColor4(state.clearColor, nightSky, midnightSky, t);
        state.lightColor = Lerp4(nightLight, midnightLight, t);
        state.ambientIntensity = Lerp(nightAmbient, midnightAmbient, t);
        state.headlightIntensity = Lerp(nightHeadlight,midnightHeadlight , t);
        state.lightDirection = Normalize4(Lerp4(nightDir, midnightDir, t));
    }
    else
    {
        // Midnight -> Sunrise
        t = SmoothStep(m_midnight, m_cycleEnd, time);
        LerpColor4(state.clearColor, midnightSky, sunriseSky, t);
        state.lightColor = Lerp4(midnightLight, sunriseLight, t);
        state.ambientIntensity = Lerp(midnightAmbient, sunriseAmbient, t);
        state.headlightIntensity = Lerp(midnightHeadlight,sunriseHeadlight , t);
        state.lightDirection = Normalize4(Lerp4(midnightDir, sunriseDir, t));
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

    float daylightProgress =
        std::clamp(
            (cycleTime - m_sunrise) /
            (m_sunset - m_sunrise),
            0.0f,
            1.0f);

    float sunArc =
        sinf(daylightProgress * DirectX::XM_PI);

    float pitchOffset =
        DirectX::XMConvertToRadians(
            Lerp(3.0f, 10.0f, sunArc));

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
    forward =
        DirectX::XMVectorSetY(
            forward,
            0.15f);

    forward =
        DirectX::XMVector3Normalize(
            forward);

    float horizontalScale = 1.0f;

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

    sun.m_Visible =
        cycleTime >= m_sunrise &&
        cycleTime <= 180.0f;

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

void TimeCycle::UpdateClouds(
        float time,
        Clouds& clouds)
{  
   


    float t = 0.0f;

    if (time < m_noon)
    {
        // Sunrise -> Day
        t = SmoothStep(m_sunrise, m_noon, time);

        clouds.m_cloudColor =
            Lerp3(
                sunriseCloudColor,
                dayCloudColor,
                t);

        clouds.m_brightness =
            Lerp(
                sunriseCloudBrightness,
                dayCloudBrightness,
                t);

    }
    else if (time < m_sunset)
    {
        // Day -> Sunset
        t = SmoothStep(m_noon, m_sunset, time);

        clouds.m_cloudColor =
            Lerp3(
                dayCloudColor,
                sunsetCloudColor,
                t);

        clouds.m_brightness =
            Lerp(
                dayCloudBrightness,
                sunsetCloudBrightness,
                t);

    }
    else if (time < m_night)
    {
        // Sunset -> Night
        t = SmoothStep(m_sunset, m_night, time);

        clouds.m_cloudColor =
            Lerp3(
                sunsetCloudColor,
                nightCloudColor,
                t);

        clouds.m_brightness =
            Lerp(
                sunsetCloudBrightness,
                nightCloudBrightness,
                t);


    }
    else if (time < m_midnight)
    {
        // Night -> Midnight
        t = SmoothStep(m_night, m_midnight, time);

        clouds.m_cloudColor =
            Lerp3(
                nightCloudColor,
                midnightCloudColor,
                t);

        clouds.m_brightness =
            Lerp(
                nightCloudBrightness,
                midnightCloudBrightness,
                t);


    }
    else
    {
        // Midnight -> Sunrise
        t = SmoothStep(m_midnight, m_cycleEnd, time);

        clouds.m_cloudColor =
            Lerp3(
                midnightCloudColor,
                sunriseCloudColor,
                t);

        clouds.m_brightness =
            Lerp(
                midnightCloudBrightness,
                sunriseCloudBrightness,
                t);


    }

}