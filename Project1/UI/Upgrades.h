#pragma once
#include "../SharedTypes.h"
#include "../UI/UIContext.h"
#include "../Cars/TyreCompounds.h"

class Upgrades
{
public:
	void Draw(const UIContext& ui);
	bool m_ShowEngineUpgrades = false;
	bool m_ShowWeightReductionUpgrades = false;
	bool m_ShowTyresUpgrades = false;
	EngineUpgradeSelection   m_EngineUpgradeSelection = EngineUpgradeSelection::StockEngine;
	WeightReductionSelection m_WeightReductionSelection = WeightReductionSelection::StockWeight;
	TyreCompound m_TyreCompoundSelection = TyreCompound::S2_SportsMedium;
};