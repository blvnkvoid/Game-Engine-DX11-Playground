#pragma once
#include "../SharedVehicleTypes.h"
#include "../SharedTypes.h"


struct TyreCompoundData
{
    float frictionCoeff;
    float loadSmoothing;
    float relaxationRate;
    float stiffness;
};

const TyreCompoundData& GetTyreCompoundData(TyreCompound compound);

void ApplyTyreCompound(TyreDefinition& tyre, TyreCompound compound);