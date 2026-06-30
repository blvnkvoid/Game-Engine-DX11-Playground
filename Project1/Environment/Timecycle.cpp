    #include "TimeCycle.h"

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

    const float sunriseAmbient = 0.12f;
    const float dayAmbient = 0.22f;
    const float sunsetAmbient = 0.10f;
    const float nightAmbient = 0.035f;
    const float midnightAmbient = 0.020f;

    const float sunriseHeadlight = 0.7f;
    const float noonHeadlight = 0.5f;
    const float sunsetHeadlight = 0.7f;
    const float nightHeadlight = 1.0f;
    const float midnightHeadlight = 1.0f;


    state.lightDirection = { 0.5f, -1.0f, 0.5f, 0.0f };

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





