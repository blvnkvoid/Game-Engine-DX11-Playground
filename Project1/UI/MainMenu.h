#pragma once

#include "../Audio/FmodManager.h"
#include "../Imgui/imgui.h"
#include "../Cars/CarSetup.h"
#include "../SharedTypes.h"

class GraphicsEngine;
class FMODManager;

class MainMenu
{
public:
    void Draw(GraphicsEngine& engine, FMODManager& audio);
    bool m_ShowEngineUpgrades = false;
    bool m_ShowWeightReductionUpgrades = false;
    bool m_ShowTyresUpgrades = false;
    bool m_ShowCarSetup = false;
    bool m_StartSimulationTriggered = false;
    bool m_ShowGarage = false; // Persistent state
    bool m_TrackSelection = false;
    EngineState g_CurrentState = EngineState::MAIN_MENU;
    VehicleSelection m_PreviewSelection = VehicleSelection::PORSCHE_911;
    TrackSelection TrackSelection = TrackSelection::Spa;
    EngineUpgradeSelection   m_EngineUpgradeSelection = EngineUpgradeSelection::StockEngine;
    WeightReductionSelection m_WeightReductionSelection = WeightReductionSelection::StockWeight;
    TyresUpgradeSelection     m_TyresUpgradeSelection = TyresUpgradeSelection::StockTyres;
    CarSetupState m_CarSetupState;
};