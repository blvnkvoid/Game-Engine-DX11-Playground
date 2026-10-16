#include "CarUpgrades.h"

const EngineUpgrade StockEngine =
{
    1.0f,
    0.0f
};

const EngineUpgrade NATune =
{
    1.08f,
    250.0f
};  


const EngineUpgrade Turbo =
{
    1.35f,
    0.0f
};


const WeightReductionUpgrade StockWeight =
{
    1.00f
};

const WeightReductionUpgrade WeightReductionStage1 =
{
     0.95f
};

const WeightReductionUpgrade WeightReductionStage2 =
{
    0.90f
};

const WeightReductionUpgrade WeightReductionStage3 =
{
    0.85f
};




const EngineUpgrade& GetEngineUpgrade(EngineUpgradeSelection selection)
{
    switch (selection)
    {
    case EngineUpgradeSelection::NATune:
        return NATune;


    case EngineUpgradeSelection::Turbo:
        return Turbo;

    case EngineUpgradeSelection::StockEngine:
    default:
        return StockEngine;
    }
}

const WeightReductionUpgrade& GetWeightReductionUpgrade(WeightReductionSelection selection)
{
    switch (selection)
    {
    case WeightReductionSelection::StockWeight:
        return StockWeight;


    case WeightReductionSelection::WeightReductionStage1:
        return WeightReductionStage1;

    case WeightReductionSelection::WeightReductionStage2:
        return WeightReductionStage2;

    case WeightReductionSelection::WeightReductionStage3:
        return WeightReductionStage3;


    default:
        return StockWeight;
    }
}





void ApplyEngineUpgrade(
    VehicleDefinition& car,
    const EngineUpgrade& upgrade)
{
    car.engine.maxTorque *= upgrade.torqueMultiplier;
    car.engine.redlineRPM += upgrade.redlineDelta;
    car.engine.limiterRPM += upgrade.redlineDelta;
}

void ApplyWeightReductionUpgrade(
    VehicleDefinition& car,
    const WeightReductionUpgrade& upgrade)
{
    car.mass *= upgrade.massMultiplier;
}

void ApplyTyresUpgrade(
    VehicleDefinition& car,
    TyreCompound compound)
{
    ApplyTyreCompound(car.frontTyres, compound);
    ApplyTyreCompound(car.rearTyres, compound);
}