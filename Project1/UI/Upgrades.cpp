#include "Upgrades.h"
#include "../Imgui/imgui.h"

void Upgrades::Draw()
{
    if (ImGui::Button("Engine Uprades Menu", ImVec2(300, 50))) {

        m_ShowEngineUpgrades = !m_ShowEngineUpgrades;
    }


    if (ImGui::Button("Weight Reduction Menu", ImVec2(300, 50))) {

        m_ShowWeightReductionUpgrades = !m_ShowWeightReductionUpgrades;
    }

    if (ImGui::Button("Tyres Upgrades Menu", ImVec2(300, 50))) {

        m_ShowTyresUpgrades = !m_ShowTyresUpgrades;
    }

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
}