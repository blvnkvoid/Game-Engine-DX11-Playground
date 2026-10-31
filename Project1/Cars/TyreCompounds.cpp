
#include "TyreCompounds.h"

const TyreCompoundData N1 = {1.00f, 18.0f, 18.0f, 10500.0f};
const TyreCompoundData N2 = {1.05f, 17.0f, 20.0f, 11500.0f};
const TyreCompoundData N3 = {1.12f, 16.0f, 22.0f, 12500.0f};

const TyreCompoundData S1 = {1.20f, 15.0f, 24.0f, 14000.0f};
const TyreCompoundData S2 = {1.30f, 14.0f, 26.0f, 15500.0f};
const TyreCompoundData S3 = {1.40f, 13.0f, 28.0f, 17000.0f};

const TyreCompoundData R1 = {1.50f, 12.0f, 30.0f, 18500.0f};
const TyreCompoundData R2 = {1.60f, 11.0f, 32.0f, 20500.0f};
const TyreCompoundData R3 = {1.70f, 10.0f, 34.0f, 22500.0f};
const TyreCompoundData R4 = {1.85f, 9.0f, 36.0f, 24500.0f};
const TyreCompoundData R5 = {2.00f, 8.0f, 38.0f, 27000.0f};

const TyreCompoundData P1 = {2.25f, 7.0f, 45.0f, 32000.0f};
const TyreCompoundData P2 = {2.45f, 6.0f, 50.0f, 36000.0f};

const TyreCompoundData &GetTyreCompoundData(TyreCompound compound)
{
    switch (compound)
    {
    case TyreCompound::N1_Economic:
        return N1;
    case TyreCompound::N2_Comfort:
        return N2;
    case TyreCompound::N3_Road:
        return N3;

    case TyreCompound::S1_SportsHard:
        return S1;
    case TyreCompound::S2_SportsMedium:
        return S2;
    case TyreCompound::S3_SportsSoft:
        return S3;

    case TyreCompound::R1_RacingSuperHard:
        return R1;
    case TyreCompound::R2_RacingHard:
        return R2;
    case TyreCompound::R3_RacingMedium:
        return R3;
    case TyreCompound::R4_RacingSoft:
        return R4;
    case TyreCompound::R5_RacingSuperSoft:
        return R5;

    case TyreCompound::P1_Prototype:
        return P1;
    case TyreCompound::P2_Prototype2:
        return P2;

    default:
        return S2;
    }
}

void ApplyTyreCompound(TyreDefinition &tyre, TyreCompound compound)
{
    const TyreCompoundData &data = GetTyreCompoundData(compound);

    tyre.frictionCoeff = data.frictionCoeff;
    tyre.loadSmoothing = data.loadSmoothing;
    tyre.relaxationRate = data.relaxationRate;
    tyre.stiffness = data.stiffness;
    tyre.tyreCompound = compound;
}
