#include "TyreCompounds.h"

// Tyres 6.0: independent lateral and longitudinal relaxation rates.
// Both axes start at each compound's original rate for baseline parity.

// N — ROAD TYRES
const TyreCompoundData N1 = {0.95f, 18.0f, 80.0f, 20.0f, 14000.0f, {1.0f, 0.75f, 0.52f}, {0.16f, 0.52f}};
const TyreCompoundData N2 = {1.00f, 17.0f, 18.0f, 22.0f, 15000.0f, {1.0f, 0.70f, 0.54f}, {0.15f, 0.54f}};
const TyreCompoundData N3 = {1.08f, 16.0f, 20.0f, 24.0f, 17000.0f, {1.0f, 0.65f, 0.56f}, {0.14f, 0.56f}};

// S — SPORTS TYRES
const TyreCompoundData S1 = {1.18f, 15.0f, 22.0f, 26.0f, 20000.0f, {1.0f, 0.60f, 0.58f}, {0.13f, 0.58f}};
const TyreCompoundData S2 = {1.28f, 14.0f, 24.0f, 28.0f, 21000.0f, {1.0f, 0.56f, 0.60f}, {0.12f, 0.60f}};
const TyreCompoundData S3 = {1.38f, 13.0f, 26.0f, 30.0f, 24000.0f, {1.0f, 0.52f, 0.62f}, {0.11f, 0.62f}};

// R — RACING TYRES
const TyreCompoundData R1 = {1.50f, 12.0f, 28.0f, 32.0f, 27000.0f, {1.0f, 0.48f, 0.64f}, {0.10f, 0.64f}};
const TyreCompoundData R2 = {1.62f, 11.0f, 30.0f, 34.0f, 31000.0f, {1.0f, 0.45f, 0.66f}, {0.10f, 0.66f}};
const TyreCompoundData R3 = {1.75f, 10.0f, 32.0f, 36.0f, 35000.0f, {1.0f, 0.42f, 0.68f}, {0.09f, 0.68f}};
const TyreCompoundData R4 = {1.90f, 9.0f, 34.0f, 38.0f, 40000.0f, {1.0f, 0.40f, 0.70f}, {0.09f, 0.70f}};
const TyreCompoundData R5 = {2.05f, 8.0f, 36.0f, 40.0f, 45000.0f, {1.0f, 0.38f, 0.72f}, {0.08f, 0.72f}};

// P — PROTOTYPE TYRES
const TyreCompoundData P1 = {2.30f, 7.0f, 40.0f, 44.0f, 52000.0f, {1.0f, 0.35f, 0.76f}, {0.08f, 0.76f}};
const TyreCompoundData P2 = {2.50f, 6.0f, 32.0f, 48.0f, 60000.0f, {1.0f, 0.32f, 0.80f}, {0.07f, 0.80f}};


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
    tyre.lateralRelaxationRate = data.lateralRelaxationRate;
    tyre.longitudinalRelaxationRate = data.longitudinalRelaxationRate;
    tyre.stiffness = data.stiffness;
    tyre.tyreCompound = compound;
}
