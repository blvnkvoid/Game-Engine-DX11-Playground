#include "Settings.h"

void Settings::Draw(const UIContext &ui)
{
    if (!m_ShowSettings)
        return;

    ImGui::SetNextWindowPos(
        ui.P(400, 100),
        ImGuiCond_FirstUseEver);

    ImGui::Begin(
        "Resolution Settings",
        &m_ShowSettings,
        ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button("1920 x 1080"))
    {
        width = 1920;
        height = 1080;
        m_ResolutionChanged = true;

        OutputDebugStringA("full hd yo");
    }

    if (ImGui::Button("3840 x 2160"))
    {
        width = 3840;
        height = 2160;
        m_ResolutionChanged = true;
    }

    if (ImGui::Button("6400 x 3600"))
    {
        width = 6400;
        height = 3600;
        m_ResolutionChanged = true;
    }    
    
    if (ImGui::Button("MAX RES"))
    {
        width = 16384;
        height = 9216;
        m_ResolutionChanged = true;
    }

    ImGui::End();
}