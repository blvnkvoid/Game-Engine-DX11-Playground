#include "Tyres.h"
#include <cmath>
#include <algorithm>
#include <Windows.h>
#include <cstdio>

#define clamp(val, low, high) ((val) < (low) ? (low) : ((val) > (high) ? (high) : (val)))

Tire::Tire()
    : m_smoothedLoad(0.0f), m_latForceBuffer(0.0f)
{
}
Tire::~Tire()
{
}

void Tire::SetTyreDefinition(const TyreDefinition &definition)
{
    m_tyre = definition;
}

float Tire::Update(float verticalLoad, float slipAngle, float speed_kmh, float localVx, float localVy, float &outTheta, float &outFrictionCoeff, float &outVx, float &outVy, float camberDeg, int wheelIndex, float slipRatio, float deltaTime)
{
    // DIAGNOSTICS ONLY: every 12th physics tick, all four wheels.
    static unsigned long long tyreDebugTick = 0;
    if (wheelIndex == 0) ++tyreDebugTick;
    const bool tyreDebug = speed_kmh > 100.0f && (tyreDebugTick % 12ULL == 0ULL);
    float debugLatBeforeGrip = 0.0f;
    float debugLongBeforeGrip = 0.0f;
    float debugGripScale = 1.0f;

    float loadAlpha = 1.0f - exp(-m_tyre.loadSmoothing * deltaTime);
    m_smoothedLoad += (verticalLoad - m_smoothedLoad) * loadAlpha;

    float camberEffect = 1.0f + abs(camberDeg) * m_tyre.camberGripStrength;
    camberEffect = clamp(camberEffect, 1.0f, 1.10f);

    float mu = m_tyre.frictionCoeff;
    float Fz = m_smoothedLoad;

    if (Fz < 10.0f)
    {
        m_latForceBuffer = 0.0f;
        m_longitudinalForce = 0.0f;
        if (tyreDebug)
        {
            char dbg[384];
            snprintf(dbg, sizeof(dbg), "[PZ][TYRE][W%d][UNLOADED] speed=%.2f FzRaw=%.3f FzSmooth=%.3f slipAngle=%.7f\n", wheelIndex, speed_kmh, verticalLoad, Fz, slipAngle);
            OutputDebugStringA(dbg);
        }
        return 0.0f;
    }

    float effectiveStiffness = m_tyre.stiffness;
    float referenceLoad = 3000.0f;
    float loadRatio = clamp(Fz / referenceLoad, 0.35f, 2.5f);
    float loadSensitivity = 0.16f;

    float loadSensitiveMu = mu * (1.0f - loadSensitivity * (loadRatio - 1.0f));
    loadSensitiveMu = clamp(loadSensitiveMu, mu * 0.72f, mu * 1.08f);

    float maxFriction = loadSensitiveMu * Fz * camberEffect;
    float theta = (effectiveStiffness * abs(slipAngle)) / (3.0f * maxFriction);

    float finalForce = 0.0f;

    if (theta < 1.0f)
    {
        finalForce = maxFriction * theta * (3.0f - 3.0f * theta + theta * theta);
    }
    else
    {
        float excessTheta = theta - 1.0f;
        float slideGrip = expf(-0.45f * excessTheta);
        slideGrip = clamp(slideGrip, 0.68f, 1.0f);
        finalForce = maxFriction * slideGrip;

    }

    float targetForce = slipAngle > 0.0f ? -finalForce : finalForce;

    float absSlip = abs(slipRatio);
    float peakSlip = 0.25f;
    float postPeakGrip = 0.58f;



    float longitudinalGrip = 0.0f;

    if (absSlip < peakSlip)
    {
        longitudinalGrip = absSlip / peakSlip;
    }
    else
    {
        float excessSlip = (absSlip - peakSlip) / peakSlip;
        longitudinalGrip = postPeakGrip + (1.0f - postPeakGrip) * expf(-excessSlip);
    }

    float longitudinalForce = longitudinalGrip * maxFriction;

    if (slipRatio < 0.0f)
        longitudinalForce = -longitudinalForce;

    float relaxAlpha = 1.0f - exp(-m_tyre.relaxationRate * deltaTime);
    m_latForceBuffer += (targetForce - m_latForceBuffer) * relaxAlpha;

    g_DebugTelemetry.lateralTargetForce[wheelIndex] = targetForce;
    g_DebugTelemetry.lateralRelaxedForce[wheelIndex] = m_latForceBuffer;
    g_DebugTelemetry.tyreSideStiffness[wheelIndex] = effectiveStiffness;
    g_DebugTelemetry.maxFriction[wheelIndex] = maxFriction;

    outTheta = theta;
    outFrictionCoeff = mu;
    outVx = localVx;
    outVy = localVy;

float lateralForce = m_latForceBuffer;

    // Relax longitudinal force FIRST
    m_longitudinalForce += (longitudinalForce - m_longitudinalForce) * relaxAlpha;

    debugLatBeforeGrip = lateralForce;
    debugLongBeforeGrip = m_longitudinalForce;
    // Enforce combined grip on the ACTUAL output forces
    float combinedForce = sqrtf(
        m_longitudinalForce * m_longitudinalForce +
        lateralForce * lateralForce);

if (combinedForce > maxFriction && combinedForce > 0.0f)
    {
        float gripScale = maxFriction / combinedForce;
        debugGripScale = gripScale;

        m_longitudinalForce *= gripScale;
        m_latForceBuffer *= gripScale;
        lateralForce = m_latForceBuffer;
    }

    if (tyreDebug)
    {
        char dbg[640];
        snprintf(dbg, sizeof(dbg),
            "[PZ][TYRE][W%d] v=%.1f FzIn=%.2f Fz=%.2f muEff=%.4f maxF=%.2f "
            "slipA=%.7f slipR=%.5f theta=%.5f target=%.2f "
            "latPreGrip=%.2f longPreGrip=%.2f combined=%.2f gripScale=%.5f "
            "latOUT=%.2f longOUT=%.2f alpha=%.5f\n",
            wheelIndex, speed_kmh, verticalLoad, Fz, loadSensitiveMu, maxFriction,
            slipAngle, slipRatio, theta, targetForce, debugLatBeforeGrip,
            debugLongBeforeGrip, combinedForce, debugGripScale,
            lateralForce, m_longitudinalForce, relaxAlpha);
        OutputDebugStringA(dbg);
    }
    return lateralForce;
}

float Tire::GetLongitudinalForce() const
{
    return m_longitudinalForce;
}
