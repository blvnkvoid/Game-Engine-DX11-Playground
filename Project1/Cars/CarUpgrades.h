#pragma once

#include "../SharedVehicleTypes.h"
#include "../SharedTypes.h"
#include "TyreCompounds.h"

struct EngineUpgrade
{
    float torqueMultiplier = 1.0f;
    float redlineDelta = 0.0f;
};

struct WeightReductionUpgrade
{
    float massMultiplier = 1.0f;
};




extern const EngineUpgrade StockEngine;
extern const EngineUpgrade NATune;
extern const EngineUpgrade Turbo;


extern const WeightReductionUpgrade StockWeight;
extern const WeightReductionUpgrade WeightReductionStage1;
extern const WeightReductionUpgrade WeightReductionStage2;
extern const WeightReductionUpgrade WeightReductionStage3;




void ApplyEngineUpgrade(
    VehicleDefinition& car,
    const EngineUpgrade& upgrade);

void ApplyWeightReductionUpgrade(
    VehicleDefinition& car,
    const WeightReductionUpgrade& upgrade);

void ApplyTyresUpgrade(
    VehicleDefinition& car,
    TyreCompound compound);

const EngineUpgrade& GetEngineUpgrade(
    EngineUpgradeSelection selection);


const WeightReductionUpgrade& GetWeightReductionUpgrade(
    WeightReductionSelection selection);

