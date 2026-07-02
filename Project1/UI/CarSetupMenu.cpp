#include "CarSetupMenu.h"
#include "../Imgui/imgui.h"

void CarSetupMenu::Draw()
{
    if (ImGui::Button("Car Setup", ImVec2(300, 50))) {

        m_ShowCarSetup = !m_ShowCarSetup;
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
}