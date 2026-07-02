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
    m_garage.Draw();
    m_upgrades.Draw();
    m_carsetup.Draw();
    m_trackmenu.Draw();

    if (ImGui::Button("EXIT", ImVec2(300, 50))) {
        PostQuitMessage(0);
    }
    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}
