#include "Tyres.h"
#include "../Cars/TyreCompounds.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <Windows.h>

// Tyres XI: exact Tyres 3.0 lateral equation with per-compound constants.
// Longitudinal, combined-slip, load and relaxation pipeline unchanged.
namespace
{
constexpr float kReferenceLoad = 3000.0f;
constexpr float kStiffnessExponent = 0.80f;
constexpr float kMinimumRollingSpeed = 3.0f;
constexpr float kReferenceRollingSpeed = 20.0f;
constexpr float kMinimumRelaxationRate = 0.01f;
constexpr float kMaximumRelaxationRate = 80.0f;
constexpr float kMinLoadForContact = 10.0f;
constexpr float kCombinedSlipBlendStart = 0.82f;
constexpr float kRecoveryRateMultiplier = 1.12f;

inline float Clamp(float x, float lo, float hi)
{
    return (std::max)(lo, (std::min)(x, hi));
}

inline float Approach(float state, float target, float rate, float dt)
{
    const float recovery = std::fabs(target) < std::fabs(state)
                               ? kRecoveryRateMultiplier
                               : 1.0f;
    const float alpha = 1.0f - std::exp(-rate * recovery * dt);
    return state + (target - state) * alpha;
}

// Tyres 3.0 lateral-force evaluator, with compound-specific constants.
// peakScale = 1, falloff = 0.45, postPeakGrip = 0.68 reproduces
// the original Tyres 3.0 curve (including its hard sliding-grip floor).
// Note: peakScale != 1 changes the initial cornering stiffness too.
inline float EvaluateLateralCurve(float absAngle, float maxFriction,
                                  float effectiveStiffness,
                                  const TyreCurveData &curve,
                                  float &outNormalizedAngle)
{
    const float scale = (std::max)(curve.peakScale, 0.001f);
    const float theta = effectiveStiffness * absAngle /
                        (3.0f * (std::max)(maxFriction, 1.0f) * scale);
    outNormalizedAngle = theta;

    if (theta < 1.0f)
    {
        return maxFriction * theta *
               (3.0f - 3.0f * theta + theta * theta);
    }

    const float slideGrip = Clamp(
        std::exp(-(std::max)(curve.postPeakFalloff, 0.0f) * (theta - 1.0f)),
        Clamp(curve.postPeakGrip, 0.0f, 1.0f), 1.0f);
    return maxFriction * slideGrip;
}
} // namespace

float Tire::SampleLateralCurve(TyreCompound compound, float slipAngle,
                               float maxFriction, float effectiveStiffness)
{
    if (maxFriction <= 0.0f || effectiveStiffness <= 0.0f)
        return 0.0f;
    float normalizedAngle = 0.0f;
    const auto &curve = GetTyreCompoundData(compound).lateralCurve;
    const float magnitude = EvaluateLateralCurve(std::fabs(slipAngle),
                                                 maxFriction, effectiveStiffness, curve, normalizedAngle);
    return slipAngle > 0.0f ? -magnitude : magnitude;
}

Tire::Tire() = default;
Tire::~Tire() = default;

void Tire::SetTyreDefinition(const TyreDefinition &definition)
{
    m_tyre = definition;
}

float Tire::Update(float verticalLoad, float slipAngle, float speed_kmh,
                   float localVx, float localVy, float &outTheta, float &outFrictionCoeff,
                   float &outVx, float &outVy, float camberDeg, int wheelIndex,
                   float slipRatio, float deltaTime)
{
    static unsigned long long tyreDebugTick = 0;
    if (wheelIndex == 0)
        ++tyreDebugTick;
    const bool tyreDebug = speed_kmh > 100.0f && (tyreDebugTick % 12ULL == 0ULL);

    const float Fz = (std::max)(verticalLoad, 0.0f);
    m_smoothedLoad = Fz;
    outTheta = 0.0f;
    outFrictionCoeff = m_tyre.frictionCoeff;
    outVx = localVx;
    outVy = localVy;

    if (Fz < kMinLoadForContact)
    {
        m_latForceBuffer = 0.0f;
        m_longitudinalForce = 0.0f;
        m_longitudinalOutput = 0.0f;
        g_DebugTelemetry.lateralTargetForce[wheelIndex] = 0.0f;
        g_DebugTelemetry.lateralRelaxedForce[wheelIndex] = 0.0f;
        g_DebugTelemetry.tyreSideStiffness[wheelIndex] = 0.0f;
        g_DebugTelemetry.maxFriction[wheelIndex] = 0.0f;
        g_DebugTelemetry.tyreSlipAngle[wheelIndex] = slipAngle;
        g_DebugTelemetry.tyrePeakSlipAngle[wheelIndex] = 0.0f;
        g_DebugTelemetry.tyreFreeLateralForce[wheelIndex] = 0.0f;
        g_DebugTelemetry.tyreCompoundId[wheelIndex] = static_cast<int>(m_tyre.tyreCompound);
        g_DebugTelemetry.tyreCurveValid[wheelIndex] = 0;
        return 0.0f;
    }

    const float mu = m_tyre.frictionCoeff;
    const float loadRatio = Clamp(Fz / kReferenceLoad, 0.35f, 2.5f);
    const float loadSensitiveMu = Clamp(
        mu * (1.0f - 0.16f * (loadRatio - 1.0f)), mu * 0.72f, mu * 1.08f);
    const float camberEffect = Clamp(
        1.0f + std::fabs(camberDeg) * m_tyre.camberGripStrength, 1.0f, 1.10f);
    const float maxFriction = (std::max)(0.0f, loadSensitiveMu * Fz * camberEffect);
    const float effectiveStiffness = m_tyre.stiffness *
                                     std::pow(Fz / kReferenceLoad, kStiffnessExponent);

    const TyreCurveData &curve =
        GetTyreCompoundData(m_tyre.tyreCompound).lateralCurve;
    float theta = 0.0f;
    const float lateralMagnitude = EvaluateLateralCurve(
        std::fabs(slipAngle), maxFriction, effectiveStiffness, curve, theta);
    const float freeLateral = slipAngle > 0.0f ? -lateralMagnitude : lateralMagnitude;

    const float absSlip = std::fabs(slipRatio);
    const float peakSlip = 0.25f;
    const float postPeakGrip = 0.58f;
    const float longitudinalGrip = absSlip < peakSlip
                                       ? absSlip / peakSlip
                                       : postPeakGrip + (1.0f - postPeakGrip) *
                                                            std::exp(-(absSlip - peakSlip) / peakSlip);
    const float freeLongitudinal = std::copysign(longitudinalGrip * maxFriction, slipRatio);

    const float freeMagnitude = std::hypot(freeLateral, freeLongitudinal);
    const float utilization = freeMagnitude / (std::max)(maxFriction, 1.0f);
    float coupledScale = 1.0f;
    float blend = 0.0f;
    if (utilization > kCombinedSlipBlendStart)
    {
        if (utilization >= 1.0f)
        {
            coupledScale = 1.0f / utilization;
            blend = 1.0f;
        }
        else
        {
            const float t = (utilization - kCombinedSlipBlendStart) /
                            (1.0f - kCombinedSlipBlendStart);
            const float dip = 0.035f * std::pow(std::sin(3.14159265f * t), 2.0f);
            const float softenedUtilization = utilization - dip;
            coupledScale = softenedUtilization / utilization;
            blend = t;
        }
    }
    const float targetLateral = freeLateral * coupledScale;
    const float targetLongitudinal = freeLongitudinal * coupledScale;

    const float rollingSpeed = std::fabs(localVx);
    const float speedScale = (std::max)(rollingSpeed, kMinimumRollingSpeed) /
                             kReferenceRollingSpeed;
    const float relaxationRate = Clamp(
        m_tyre.relaxationRate * speedScale,
        kMinimumRelaxationRate, kMaximumRelaxationRate);
    const float dt = (std::max)(deltaTime, 0.0f);

    m_latForceBuffer = Approach(m_latForceBuffer, targetLateral, relaxationRate, dt);
    m_longitudinalForce = Approach(m_longitudinalForce, targetLongitudinal, relaxationRate, dt);

    float lateralForce = m_latForceBuffer;
    float longitudinalForce = m_longitudinalForce;
    const float outputMagnitude = std::hypot(lateralForce, longitudinalForce);
    float outputScale = 1.0f;
    if (outputMagnitude > maxFriction && outputMagnitude > 0.0f)
    {
        outputScale = maxFriction / outputMagnitude;
        lateralForce *= outputScale;
        longitudinalForce *= outputScale;
    }
    m_longitudinalOutput = longitudinalForce;

    g_DebugTelemetry.lateralTargetForce[wheelIndex] = targetLateral;
    g_DebugTelemetry.lateralRelaxedForce[wheelIndex] = lateralForce;
    g_DebugTelemetry.tyreSideStiffness[wheelIndex] = effectiveStiffness;
    g_DebugTelemetry.maxFriction[wheelIndex] = maxFriction;
    // Peak theta=1 from the original Tyres 3.0 equation.
    const float safeForce = (std::max)(maxFriction, 1.0f);
    const float safeStiffness = (std::max)(effectiveStiffness, 1.0f);
    g_DebugTelemetry.tyreSlipAngle[wheelIndex] = slipAngle;
    g_DebugTelemetry.tyrePeakSlipAngle[wheelIndex] =
        3.0f * safeForce * (std::max)(curve.peakScale, 0.001f) / safeStiffness;
    g_DebugTelemetry.tyreFreeLateralForce[wheelIndex] = freeLateral;
    g_DebugTelemetry.tyreCompoundId[wheelIndex] = static_cast<int>(m_tyre.tyreCompound);
    g_DebugTelemetry.tyreCurveValid[wheelIndex] = 1;
    outTheta = theta;

    if (tyreDebug)
    {
        char dbg[768];
        snprintf(dbg, sizeof(dbg),
                 "[PZ4][TYRE][W%d] v=%.1f Fz=%.2f muEff=%.4f maxF=%.2f "
                 "slipA=%.7f slipR=%.5f peakX=%.5f freeLat=%.2f freeLong=%.2f "
                 "util=%.3f blend=%.3f targetLat=%.2f targetLong=%.2f "
                 "latState=%.2f longState=%.2f latOUT=%.2f longOUT=%.2f "
                 "outputScale=%.4f rate=%.3f\n",
                 wheelIndex, speed_kmh, Fz, loadSensitiveMu, maxFriction,
                 slipAngle, slipRatio, theta, freeLateral, freeLongitudinal,
                 utilization, blend, targetLateral, targetLongitudinal,
                 m_latForceBuffer, m_longitudinalForce,
                 lateralForce, longitudinalForce, outputScale, relaxationRate);
        OutputDebugStringA(dbg);
    }
    return lateralForce;
}

float Tire::GetLongitudinalForce() const
{
    return m_longitudinalOutput;
}
