#pragma once
#include "../SharedTypes.h"

class Upgrades
{
public:
	void Draw();
	bool m_ShowEngineUpgrades = false;
	bool m_ShowWeightReductionUpgrades = false;
	bool m_ShowTyresUpgrades = false;
	EngineUpgradeSelection   m_EngineUpgradeSelection = EngineUpgradeSelection::StockEngine;
	WeightReductionSelection m_WeightReductionSelection = WeightReductionSelection::StockWeight;
	TyresUpgradeSelection     m_TyresUpgradeSelection = TyresUpgradeSelection::StockTyres;
};