#include "MainMenu.h"
#include "../Graphics/GraphicsEngine.h"
#include "../Audio/FmodManager.h"
#include "../Imgui/imgui.h"
#include "../Cars/CarSetup.h"
#include "../SharedTypes.h"

void MainMenu::Draw(GraphicsEngine& engine, FMODManager& audio)
{
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
    m_eventmenu.Draw();

    if (ImGui::Button("EXIT", ImVec2(300, 50))) {
        PostQuitMessage(0);
    }
    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}


void MainMenu::HandleInput(FMODManager& audio)
{
    static bool aWasDown = false;
    static bool bWasDown = false;

    bool aDown = Input::IsPadButtonDown(XINPUT_GAMEPAD_A);
    bool bDown = Input::IsPadButtonDown(XINPUT_GAMEPAD_B);

    if (aDown && !aWasDown)
    {
        if (m_StartSimulationTriggered)
            audio.PlayMenuStart();
        else
            audio.PlayMenuConfirm();
    }

    if (bDown && !bWasDown)
    {
        audio.PlayMenuCancel();

        m_garage.m_ShowGarage = false;
        m_carsetup.m_ShowCarSetup = false;

        m_upgrades.m_ShowTyresUpgrades = false;
        m_upgrades.m_ShowEngineUpgrades = false;
        m_upgrades.m_ShowWeightReductionUpgrades = false;

        m_trackmenu.m_TrackSelection = false;
        m_eventmenu.m_ShowEventMenu = false;
    }

    if (Input::IsMenuDownPressed() || Input::IsMenuUpPressed())
    {
        audio.PlayMenuClick();
    }

    audio.PlayMenuMusic();

    aWasDown = aDown;
    bWasDown = bDown;
}