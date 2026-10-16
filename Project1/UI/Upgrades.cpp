#include "Upgrades.h"
#include "../Imgui/imgui.h"

void Upgrades::Draw(const UIContext& ui)
{
    if (ImGui::Button("Engine Uprades Menu", ui.Size(300, 50))) {

        m_ShowEngineUpgrades = !m_ShowEngineUpgrades;
    }


    if (ImGui::Button("Weight Reduction Menu", ui.Size(300, 50))) {

        m_ShowWeightReductionUpgrades = !m_ShowWeightReductionUpgrades;
    }

    if (ImGui::Button("Tyres Upgrades Menu", ui.Size(300, 50))) {

        m_ShowTyresUpgrades = !m_ShowTyresUpgrades;
    }

    if (m_ShowEngineUpgrades) {
        ImGui::SetNextWindowPos(ui.P(400, 100)); // Place it next to the main menu
        ImGui::SetNextWindowSize(ui.Size(180, 0), ImGuiCond_Always);
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
        ImGui::SetNextWindowPos(ui.P(400, 100)); // Place it next to the main menu
        ImGui::SetNextWindowSize(ui.Size(300, 0), ImGuiCond_Always);
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

    if (m_ShowTyresUpgrades)
    {
        ImGui::SetNextWindowPos(ui.P(400, 100));
        ImGui::SetNextWindowSize(ui.Size(300, 0), ImGuiCond_Always);

        ImGui::Begin("Tyres Upgrades", nullptr, 0);

        // Normal tyres
        if (ImGui::Selectable("N1 - Economic",
            m_TyreCompoundSelection == TyreCompound::N1_Economic))
            m_TyreCompoundSelection = TyreCompound::N1_Economic;

        if (ImGui::Selectable("N2 - Comfort",
            m_TyreCompoundSelection == TyreCompound::N2_Comfort))
            m_TyreCompoundSelection = TyreCompound::N2_Comfort;

        if (ImGui::Selectable("N3 - Road",
            m_TyreCompoundSelection == TyreCompound::N3_Road))
            m_TyreCompoundSelection = TyreCompound::N3_Road;

        ImGui::Separator();

        // Sports tyres
        if (ImGui::Selectable("S1 - Sports Hard",
            m_TyreCompoundSelection == TyreCompound::S1_SportsHard))
            m_TyreCompoundSelection = TyreCompound::S1_SportsHard;

        if (ImGui::Selectable("S2 - Sports Medium",
            m_TyreCompoundSelection == TyreCompound::S2_SportsMedium))
            m_TyreCompoundSelection = TyreCompound::S2_SportsMedium;

        if (ImGui::Selectable("S3 - Sports Soft",
            m_TyreCompoundSelection == TyreCompound::S3_SportsSoft))
            m_TyreCompoundSelection = TyreCompound::S3_SportsSoft;

        ImGui::Separator();

        // Racing tyres
        if (ImGui::Selectable("R1 - Racing Super Hard",
            m_TyreCompoundSelection == TyreCompound::R1_RacingSuperHard))
            m_TyreCompoundSelection = TyreCompound::R1_RacingSuperHard;

        if (ImGui::Selectable("R2 - Racing Hard",
            m_TyreCompoundSelection == TyreCompound::R2_RacingHard))
            m_TyreCompoundSelection = TyreCompound::R2_RacingHard;

        if (ImGui::Selectable("R3 - Racing Medium",
            m_TyreCompoundSelection == TyreCompound::R3_RacingMedium))
            m_TyreCompoundSelection = TyreCompound::R3_RacingMedium;

        if (ImGui::Selectable("R4 - Racing Soft",
            m_TyreCompoundSelection == TyreCompound::R4_RacingSoft))
            m_TyreCompoundSelection = TyreCompound::R4_RacingSoft;

        if (ImGui::Selectable("R5 - Racing Super Soft",
            m_TyreCompoundSelection == TyreCompound::R5_RacingSuperSoft))
            m_TyreCompoundSelection = TyreCompound::R5_RacingSuperSoft;

        ImGui::Separator();

        // Prototype tyres
        if (ImGui::Selectable("P1 - Prototype",
            m_TyreCompoundSelection == TyreCompound::P1_Prototype))
            m_TyreCompoundSelection = TyreCompound::P1_Prototype;

        if (ImGui::Selectable("P2 - Prototype 2",
            m_TyreCompoundSelection == TyreCompound::P2_Prototype2))
            m_TyreCompoundSelection = TyreCompound::P2_Prototype2;

        ImGui::End();
    }

}