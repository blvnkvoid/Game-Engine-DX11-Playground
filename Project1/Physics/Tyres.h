#pragma once
#include "../SharedVehicleTypes.h"
class Tire {
public:
    Tire();
    ~Tire();

    float Update(float verticalLoad, float slipAngle, float speed_kmh, float localVx, float localVy, float& outTheta, float& outFrictionCoeff, float& outVx, float& outVy, float camberDeg, int wheelIndex, float slipRatio);
    void SetTyreDefinition(const TyreDefinition& definition);
private:
    float m_smoothedLoad = 0.0f;
    float m_latForceBuffer = 0.0f;
    TyreDefinition m_tyre;
};
