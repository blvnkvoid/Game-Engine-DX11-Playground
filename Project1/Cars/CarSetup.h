#pragma once
#include "../SharedVehicleTypes.h"

struct CarSetupState
{
    float finalDriveMultiplier = 1.0f;
    float brakeBiasFront = -1.0f; // -1 means use car default

    float frontARBMultiplier = 1.0f;
    float rearARBMultiplier = 1.0f;

    float frontSpringMultiplier = 1.0f;
    float rearSpringMultiplier = 1.0f;

    float frontDownforceMultiplier = 1.0f;
    float rearDownforceMultiplier = 1.0f;
};

void ApplySetup(VehicleDefinition& car, const CarSetupState& setup);