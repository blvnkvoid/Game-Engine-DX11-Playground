#pragma once

#include "../Audio/FmodManager.h"
#include "../Imgui/imgui.h"
#include "../Cars/CarSetup.h"
#include "../SharedTypes.h"
#include "../UI/TrackMenu.h"
#include "../UI/Garage.h"
#include "../UI/Upgrades.h"
#include "../UI/CarSetupMenu.h"

class GraphicsEngine;
class FMODManager;
class MainMenu
{
public:
    void Draw(GraphicsEngine& engine, FMODManager& audio);


    bool m_StartSimulationTriggered = false;
    Garage& GetGarage() { return m_garage; }
    Upgrades& GetUpgrades() { return m_upgrades; }
    CarSetupMenu& GetCarSetup() { return m_carsetup; }

    EngineState g_CurrentState = EngineState::MAIN_MENU;

    TrackMenu m_trackmenu;
    Garage m_garage;
    Upgrades m_upgrades;
    CarSetupMenu m_carsetup;

};