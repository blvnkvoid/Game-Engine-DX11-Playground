#include "TyreCompounds.h"


const TyreCompoundData N1 =
{
    1.00f,     // frictionCoeff
    18.0f,     // loadSmoothing
    18.0f,     // relaxationRate
    10500.0f   // stiffness
};

const TyreCompoundData N2 =
{
    1.05f,     // frictionCoeff
    17.0f,     // loadSmoothing
    20.0f,     // relaxationRate
    11500.0f   // stiffness
};

const TyreCompoundData N3 =
{
    1.12f,     // frictionCoeff
    16.0f,     // loadSmoothing
    22.0f,     // relaxationRate
    12500.0f   // stiffness
};

const TyreCompoundData S1 =
{
    1.20f,     // frictionCoeff
    15.0f,     // loadSmoothing
    24.0f,     // relaxationRate
    14000.0f   // stiffness
};

const TyreCompoundData S2 =
{
    1.30f,     // frictionCoeff
    14.0f,     // loadSmoothing
    26.0f,     // relaxationRate
    15500.0f   // stiffness
};

const TyreCompoundData S3 =
{
    1.40f,     // frictionCoeff
    13.0f,     // loadSmoothing
    28.0f,     // relaxationRate
    17000.0f   // stiffness
};

const TyreCompoundData R1 =
{
    1.50f,     // frictionCoeff
    12.0f,     // loadSmoothing
    30.0f,     // relaxationRate
    18500.0f   // stiffness
};

const TyreCompoundData R2 =
{
    1.60f,     // frictionCoeff
    11.0f,     // loadSmoothing
    32.0f,     // relaxationRate
    20500.0f   // stiffness
};


const TyreCompoundData R3 =
{
    1.70f,     // frictionCoeff
    10.0f,     // loadSmoothing
    34.0f,     // relaxationRate
    22500.0f   // stiffness
};


const TyreCompoundData R4 =
{
    1.85f,     // frictionCoeff
    9.0f,     // loadSmoothing
    36.0f,     // relaxationRate
    24500.0f   // stiffness
};


const TyreCompoundData R5 =
{
    2.00f,     // frictionCoeff
    8.0f,     // loadSmoothing
    38.0f,     // relaxationRate
    27000.0f   // stiffness
};

const TyreCompoundData P1 =
{
    2.25f,     // frictionCoeff
    7.0f,     // loadSmoothing
    45.0f,     // relaxationRate
    32000.0f   // stiffness
};

const TyreCompoundData P2 =
{
    2.45f,     // frictionCoeff
    6.0f,     // loadSmoothing
    50.0f,     // relaxationRate
    36000.0f   // stiffness
};




const TyreCompoundData& GetTyreCompoundData(TyreCompound compound)
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


void ApplyTyreCompound(
    TyreDefinition& tyre,
    TyreCompound compound)
{
    const TyreCompoundData& data = GetTyreCompoundData(compound);

    tyre.frictionCoeff = data.frictionCoeff;
    tyre.loadSmoothing = data.loadSmoothing;
    tyre.relaxationRate = data.relaxationRate;
    tyre.stiffness = data.stiffness;

    tyre.tyreCompound = compound;
}