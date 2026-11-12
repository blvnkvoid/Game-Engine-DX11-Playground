#pragma once
#include "../SharedVehicleTypes.h"

class Tire
{
  public:
    Tire();
    ~Tire();
    float Update(float verticalLoad, float slipAngle, float speed_kmh,
                 float localVx, float localVy, float &outTheta, float &outFrictionCoeff,
                 float &outVx, float &outVy, float camberDeg, int wheelIndex,
                 float slipRatio, float deltaTime);
    void SetTyreDefinition(const TyreDefinition &definition);
    float GetLongitudinalForce() const;
    // Read-only Tyres 3.0 parameterized lateral curve; no state changes. Signed slip angle in radians.
    static float SampleLateralCurve(TyreCompound compound, float slipAngle,
                                    float maxFriction, float effectiveStiffness);

  private:
    float m_smoothedLoad = 0.0f;
    float m_latForceBuffer = 0.0f;
    float m_longitudinalForce = 0.0f;
    float m_longitudinalOutput = 0.0f; // Limited output, separate from relaxation state
    TyreDefinition m_tyre;
};
