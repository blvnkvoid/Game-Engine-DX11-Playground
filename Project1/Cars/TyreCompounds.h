#pragma once
#include "../SharedVehicleTypes.h"
#include "../SharedTypes.h"

// peakScale scales the original load-dependent Tyres 3.0 peak angle.
// 1.0 preserves the original cornering stiffness and peak location.
// Falloff is per normalized theta past peak; postPeakGrip is a HARD floor.
struct TyreCurveData
{
    float peakScale;
    float postPeakFalloff;
    float postPeakGrip;
};

struct TyreCompoundData
{
    float frictionCoeff;
    float loadSmoothing;
    float relaxationRate;
    float stiffness;
    TyreCurveData lateralCurve;
};

const TyreCompoundData &GetTyreCompoundData(TyreCompound compound);
void ApplyTyreCompound(TyreDefinition &tyre, TyreCompound compound);
