#include "MainMenu.h"
#include "../Graphics/GraphicsEngine.h"
#include "../Audio/FmodManager.h"
#include "../Imgui/imgui.h"
#include "../Cars/CarSetup.h"
#include "../SharedTypes.h"

void MainMenu::Draw(GraphicsEngine& engine, FMODManager& audio)
{
    OutputDebugStringA("MainMenu::Draw\n");
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::SetNextWindowPos(ImVec2(50, 100));
    ImGui::Begin("MAIN_OPTIONS", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button("START SIMULATION", ImVec2(300, 50))) {

        m_StartSimulationTriggered = true;
        g_CurrentState = EngineState::GAMEPLAY;
    }

        
    if (ImGui::Button("GARAGE / SELECT CAR", ImVec2(300, 50))) {

        m_ShowGarage = !m_ShowGarage;
    }

    if (ImGui::Button("Engine Uprades Menu", ImVec2(300, 50))) {

        m_ShowEngineUpgrades = !m_ShowEngineUpgrades;
    }


    if (ImGui::Button("Weight Reduction Menu", ImVec2(300, 50))) {

        m_ShowWeightReductionUpgrades = !m_ShowWeightReductionUpgrades;
    }

    if (ImGui::Button("Tyres Upgrades Menu", ImVec2(300, 50))) {

        m_ShowTyresUpgrades = !m_ShowTyresUpgrades;
    }

    if (ImGui::Button("Car Setup", ImVec2(300, 50))) {

        m_ShowCarSetup = !m_ShowCarSetup;
    }

    if (ImGui::Button("TRACK SELECTION", ImVec2(300, 50))) {

        m_TrackSelection = !m_TrackSelection;
    }

    if (ImGui::Button("EXIT", ImVec2(300, 50))) {
        PostQuitMessage(0);
    }
    ImGui::End();

    // 3. The persistent Garage List
    if (m_ShowGarage) {
        ImGui::SetNextWindowPos(ImVec2(400, 100)); // Place it next to the main menu
        ImGui::Begin("Car List", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::Selectable("PORSCHE 911 GT3", m_PreviewSelection == VehicleSelection::PORSCHE_911)) {

            m_PreviewSelection = VehicleSelection::PORSCHE_911;
        }
        if (ImGui::Selectable("AUDI R8 COUPE", m_PreviewSelection == VehicleSelection::AUDI_R8)) {

            m_PreviewSelection = VehicleSelection::AUDI_R8;
        }
        if (ImGui::Selectable("BUGATTI CHIRON", m_PreviewSelection == VehicleSelection::BUGATTI_CHIRON)) {

            m_PreviewSelection = VehicleSelection::BUGATTI_CHIRON;
        }

        if (ImGui::Selectable("HONDA CIVIC", m_PreviewSelection == VehicleSelection::CIVIC)) {

            m_PreviewSelection = VehicleSelection::CIVIC;
        }

        if (ImGui::Selectable("TOYOTA MINOLTA", m_PreviewSelection == VehicleSelection::MINOLTA)) {

            m_PreviewSelection = VehicleSelection::MINOLTA;
        }

        if (ImGui::Selectable("Honda NSX GT-500", m_PreviewSelection == VehicleSelection::GT500)) {

            m_PreviewSelection = VehicleSelection::GT500;
        }

        if (ImGui::Selectable("Mazda Furai", m_PreviewSelection == VehicleSelection::FURAI)) {

            m_PreviewSelection = VehicleSelection::FURAI;
        }

        if (ImGui::Selectable("Mazda MX5", m_PreviewSelection == VehicleSelection::MX5)) {

            m_PreviewSelection = VehicleSelection::MX5;
        }

        if (ImGui::Selectable("Audi R10", m_PreviewSelection == VehicleSelection::AUDI_R10)) {

            m_PreviewSelection = VehicleSelection::AUDI_R10;
        }

        if (ImGui::Selectable("Citroen Xsara KitCar'98", m_PreviewSelection == VehicleSelection::XSARA)) {

            m_PreviewSelection = VehicleSelection::XSARA;
        }

        if (ImGui::Selectable("Daihatsu Copen", m_PreviewSelection == VehicleSelection::COPEN)) {

            m_PreviewSelection = VehicleSelection::COPEN;
        }

        if (ImGui::Selectable("Toyota Supra JGTC 2000", m_PreviewSelection == VehicleSelection::JGTCSUPRA2000)) {

            m_PreviewSelection = VehicleSelection::JGTCSUPRA2000;
        }

        if (ImGui::Selectable("Honda NSX Raybrig JGTC 2000", m_PreviewSelection == VehicleSelection::JGTCNSX2000)) {

            m_PreviewSelection = VehicleSelection::JGTCNSX2000;
        }

        if (ImGui::Selectable("Mercedes Benz SLS Pacecar '11", m_PreviewSelection == VehicleSelection::SLS_PACECAR)) {

            m_PreviewSelection = VehicleSelection::SLS_PACECAR;
        }

        ImGui::End();
    }

    // 4. The Upgrade Menu
    if (m_ShowEngineUpgrades) {
        ImGui::SetNextWindowPos(ImVec2(400, 100)); // Place it next to the main menu
        ImGui::SetNextWindowSize(ImVec2(180, 0), ImGuiCond_Always);
        ImGui::Begin("Engine Upgrades", nullptr, 0);

        if (ImGui::Selectable("Stock Engine", m_EngineUpgradeSelection == EngineUpgradeSelection::StockEngine)) {

            m_EngineUpgradeSelection = EngineUpgradeSelection::StockEngine;
        }
        if (ImGui::Selectable("NA Tune", m_EngineUpgradeSelection == EngineUpgradeSelection::NATune)) {

            m_EngineUpgradeSelection = EngineUpgradeSelection::NATune;
        }
        if (ImGui::Selectable("Turbo", m_EngineUpgradeSelection == EngineUpgradeSelection::Turbo)) {

            m_EngineUpgradeSelection = EngineUpgradeSelection::Turbo;
        }


        ImGui::End();
    }

    if (m_ShowWeightReductionUpgrades) {
        ImGui::SetNextWindowPos(ImVec2(400, 100)); // Place it next to the main menu
        ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);
        ImGui::Begin("Weight Reduction Upgrades", nullptr, 0);

        if (ImGui::Selectable("Stock Weight", m_WeightReductionSelection == WeightReductionSelection::StockWeight)) {

            m_WeightReductionSelection = WeightReductionSelection::StockWeight;
        }
        if (ImGui::Selectable("Weight Reduction Stage 1", m_WeightReductionSelection == WeightReductionSelection::WeightReductionStage1)) {

            m_WeightReductionSelection = WeightReductionSelection::WeightReductionStage1;
        }
        if (ImGui::Selectable("Weight Reduction Stage 2", m_WeightReductionSelection == WeightReductionSelection::WeightReductionStage2)) {

            m_WeightReductionSelection = WeightReductionSelection::WeightReductionStage2;
        }

        if (ImGui::Selectable("Weight Reduction Stage 3", m_WeightReductionSelection == WeightReductionSelection::WeightReductionStage3)) {

            m_WeightReductionSelection = WeightReductionSelection::WeightReductionStage3;
        }

        ImGui::End();
    }

    if (m_ShowTyresUpgrades) {
        ImGui::SetNextWindowPos(ImVec2(400, 100)); // Place it next to the main menu
        ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);
        ImGui::Begin("Tyres Upgrades", nullptr, 0);

        if (ImGui::Selectable("Stock Tyres", m_TyresUpgradeSelection == TyresUpgradeSelection::StockTyres)) {

            m_TyresUpgradeSelection = TyresUpgradeSelection::StockTyres;
        }
        if (ImGui::Selectable("Sports Tyres", m_TyresUpgradeSelection == TyresUpgradeSelection::SportTyres)) {

            m_TyresUpgradeSelection = TyresUpgradeSelection::SportTyres;
        }
        if (ImGui::Selectable("Semi Slicks", m_TyresUpgradeSelection == TyresUpgradeSelection::SemiSlicks)) {

            m_TyresUpgradeSelection = TyresUpgradeSelection::SemiSlicks;
        }

        if (ImGui::Selectable("Racing Tyres", m_TyresUpgradeSelection == TyresUpgradeSelection::RacingTyres)) {

            m_TyresUpgradeSelection = TyresUpgradeSelection::RacingTyres;
        }

        ImGui::End();
    }

    if (m_ShowCarSetup)
    {
        ImGui::SetNextWindowPos(ImVec2(400, 100));
        ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);

        ImGui::Begin("Car Setup", nullptr, 0);

        ImGui::SliderFloat(
            "Final Drive",
            &m_CarSetupState.finalDriveMultiplier,
            0.80f,
            1.20f);

        ImGui::SliderFloat(
            "Front ARB",
            &m_CarSetupState.frontARBMultiplier,
            0.50f,
            2.00f);

        ImGui::SliderFloat(
            "Rear ARB",
            &m_CarSetupState.rearARBMultiplier,
            0.50f,
            2.00f);

        ImGui::SliderFloat(
            "Front Springs",
            &m_CarSetupState.frontSpringMultiplier,
            0.50f,
            2.00f);

        ImGui::SliderFloat(
            "Rear Springs",
            &m_CarSetupState.rearSpringMultiplier,
            0.50f,
            2.00f);

        ImGui::SliderFloat(
            "Front Downforce",
            &m_CarSetupState.frontDownforceMultiplier,
            0.50f,
            2.00f);

        ImGui::SliderFloat(
            "Rear Downforce",
            &m_CarSetupState.rearDownforceMultiplier,
            0.50f,
            2.00f);

        ImGui::SliderFloat(
            "Brake Bias Front",
            &m_CarSetupState.brakeBiasFront,
            0.45f,
            0.75f);

        if (ImGui::Button("Reset"))
        {
            m_CarSetupState = CarSetupState{};
        }

        ImGui::End();
    }






    if (m_TrackSelection)
    {
        ImGui::SetNextWindowPos(ImVec2(400, 100)); // Place it next to the main menu
        ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);
        ImGui::Begin("Track List", nullptr, 0);
        if (ImGui::Selectable("AutumnRing", GameConfig::activeTrack == TrackSelection::AutumnRing)) {

            GameConfig::activeTrack = TrackSelection::AutumnRing;
        }
        if (ImGui::Selectable("ElCapitan", GameConfig::activeTrack == TrackSelection::ElCapitan)) {

            GameConfig::activeTrack = TrackSelection::ElCapitan;
        }
        if (ImGui::Selectable("Spa Francorshamps ", GameConfig::activeTrack == TrackSelection::Spa)) {

            GameConfig::activeTrack = TrackSelection::Spa;
        }
        if (ImGui::Selectable("Grand Valley Speedway ", GameConfig::activeTrack == TrackSelection::GrandValley)) {

            GameConfig::activeTrack = TrackSelection::GrandValley;
        }
        if (ImGui::Selectable("Trial Mountain ", GameConfig::activeTrack == TrackSelection::TrialMountain)) {

            GameConfig::activeTrack = TrackSelection::TrialMountain;
        }
        if (ImGui::Selectable("High Speed Ring ", GameConfig::activeTrack == TrackSelection::HighSpeedRing)) {

            GameConfig::activeTrack = TrackSelection::HighSpeedRing;
        }
        if (ImGui::Selectable("Midfield Raceway ", GameConfig::activeTrack == TrackSelection::MidfieldRaceway)) {

            GameConfig::activeTrack = TrackSelection::MidfieldRaceway;
        }
        if (ImGui::Selectable("Test Course ", GameConfig::activeTrack == TrackSelection::TestCourse)) {

            GameConfig::activeTrack = TrackSelection::TestCourse;
        }
        if (ImGui::Selectable("Route X ", GameConfig::activeTrack == TrackSelection::RouteX)) {

            GameConfig::activeTrack = TrackSelection::RouteX;
        }
        if (ImGui::Selectable("Road Atlanta ", GameConfig::activeTrack == TrackSelection::RoadAtlanta)) {

            GameConfig::activeTrack = TrackSelection::RoadAtlanta;
        }
        if (ImGui::Selectable("Nurburgring Nordschleife ", GameConfig::activeTrack == TrackSelection::Nordschleife)) {

            GameConfig::activeTrack = TrackSelection::Nordschleife;
        }

        if (ImGui::Selectable("Begginer Course ", GameConfig::activeTrack == TrackSelection::BeginnerCourse)) {

            GameConfig::activeTrack = TrackSelection::BeginnerCourse;
        }
        if (ImGui::Selectable("Motorland ", GameConfig::activeTrack == TrackSelection::Motorland)) {

            GameConfig::activeTrack = TrackSelection::Motorland;
        }

        if (ImGui::Selectable("Tsukuba ", GameConfig::activeTrack == TrackSelection::Tsukuba)) {

            GameConfig::activeTrack = TrackSelection::Tsukuba;
        }

        if (ImGui::Selectable("Le Mans ", GameConfig::activeTrack == TrackSelection::LeMans)) {

            GameConfig::activeTrack = TrackSelection::LeMans;
        }

        if (ImGui::Selectable("Deep Forest Raceway", GameConfig::activeTrack == TrackSelection::DeepForest)) {

            GameConfig::activeTrack = TrackSelection::DeepForest;
        }

        if (ImGui::Selectable("Special Stage Route 5", GameConfig::activeTrack == TrackSelection::SSR5)) {

            GameConfig::activeTrack = TrackSelection::SSR5;
        }

        ImGui::End();
    }




    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}