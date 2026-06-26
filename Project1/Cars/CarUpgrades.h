#pragma once

#include "../SharedVehicleTypes.h"
#include "../SharedTypes.h"

struct EngineUpgrade
{
    float torqueMultiplier = 1.0f;
    float redlineDelta = 0.0f;
};

struct WeightReductionUpgrade
{
    float massMultiplier = 1.0f;
};

struct TyresUpgrade
{
    float frictionMultiplier = 1.0f;
    float stiffnessMultiplier = 1.0f;
    float relaxationRateMultiplier = 1.0f;
};



extern const EngineUpgrade StockEngine;
extern const EngineUpgrade NATune;
extern const EngineUpgrade Turbo;


extern const WeightReductionUpgrade StockWeight;
extern const WeightReductionUpgrade WeightReductionStage1;
extern const WeightReductionUpgrade WeightReductionStage2;
extern const WeightReductionUpgrade WeightReductionStage3;


extern const TyresUpgrade StockTyres;
extern const TyresUpgrade SportTyres;
extern const TyresUpgrade SemiSlicks;
extern const TyresUpgrade RacingTyres;


void ApplyEngineUpgrade(
    VehicleDefinition& car,
    const EngineUpgrade& upgrade);

void ApplyWeightReductionUpgrade(
    VehicleDefinition& car,
    const WeightReductionUpgrade& upgrade);

void ApplyTyresUpgrade(
    VehicleDefinition& car,
    const TyresUpgrade& upgrade);

const EngineUpgrade& GetEngineUpgrade(
    EngineUpgradeSelection selection);


const WeightReductionUpgrade& GetWeightReductionUpgrade(
    WeightReductionSelection selection);

const TyresUpgrade& GetTyresUpgrade(
    TyresUpgradeSelection selection);