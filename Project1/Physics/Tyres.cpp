#include "Tyres.h"
#include <cmath>
#define clamp(val, low, high) ((val) < (low) ? (low) : ((val) > (high) ? (high) : (val)))

Tire::Tire() : m_smoothedLoad(0.0f), m_latForceBuffer(0.0f) {}
Tire::~Tire() {}



void Tire::SetTyreDefinition(const TyreDefinition& definition)
{
    m_tyre = definition;

}

float Tire::Update(float verticalLoad, float slipAngle, float speed_kmh, float localVx, float localVy, float& outTheta, float& outFrictionCoeff, float& outVx, float& outVy, float camberDeg, int  wheelIndex, float slipRatio) {
    float internalDelta = 1.0f / 300.0f;

    float loadAlpha = 1.0f - exp(-m_tyre.loadSmoothing * internalDelta);
    m_smoothedLoad += (verticalLoad - m_smoothedLoad) * loadAlpha;

    float camberEffect =
        1.0f + abs(camberDeg) * m_tyre.camberGripStrength;

    camberEffect = clamp(camberEffect, 1.0f, 1.10f);
    float mu = m_tyre.frictionCoeff;
    float Fz = m_smoothedLoad;
    if (Fz < 10.0f) {
        m_latForceBuffer = 0.0f;
        return 0.0f;
    }


    
    float effectiveStiffness = m_tyre.stiffness;

    float referenceLoad = 3000.0f;
    float loadRatio = Fz / referenceLoad;
    loadRatio = clamp(loadRatio, 0.35f, 2.5f);

    float loadSensitivity = 0.16f;

    float loadSensitiveMu =
        mu * (1.0f - loadSensitivity * (loadRatio - 1.0f));

    loadSensitiveMu = clamp(
        loadSensitiveMu,
        mu * 0.72f,
        mu * 1.08f);

    float maxFriction = loadSensitiveMu * Fz * camberEffect;

    float theta = (effectiveStiffness * abs(slipAngle)) / (3.0f * maxFriction);

    float finalForce = 0.0f;
    if (theta < 1.0f)
    {
        finalForce =
            maxFriction *
            theta *
            (3.0f - 3.0f * theta + theta * theta);
    }
    else
    {
        float excessTheta = theta - 1.0f;
        float slideGrip = expf(-0.45f * excessTheta);
        slideGrip = clamp(slideGrip, 0.68f, 1.0f);

        finalForce = maxFriction * slideGrip;
    }

    float targetForce = (slipAngle > 0.0f) ? -finalForce : finalForce;



    float relaxAlpha = 1.0f - exp(-m_tyre.relaxationRate * internalDelta);
    m_latForceBuffer += (targetForce - m_latForceBuffer) * relaxAlpha;

   g_DebugTelemetry.tyreSideStiffness[wheelIndex] = effectiveStiffness;
   g_DebugTelemetry.maxFriction[wheelIndex] = maxFriction;
   outTheta = theta;
   outFrictionCoeff = mu;
   outVx = localVx;
   outVy = localVy;
    

    return m_latForceBuffer;
}
