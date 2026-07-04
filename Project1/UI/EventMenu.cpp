#include "EventMenu.h"
#include "../Imgui/imgui.h"
void EventMenu::Draw()
{
    if (ImGui::Button("EVENT SELECTION", ImVec2(300, 50)))
    {
        m_ShowEventMenu = !m_ShowEventMenu;
    }

    if (!m_ShowEventMenu)
        return;

    ImGui::SetNextWindowPos(ImVec2(400, 100));
    ImGui::Begin("Event List", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    for (const auto& event : g_EventList)
    {
        if (ImGui::Selectable(
            event.displayName,
            m_SelectedEvent == event.selection))
        {
            m_SelectedEvent = event.selection;
        }
    }

    ImGui::Separator();

    ImGui::TextDisabled("More events coming soon...");

    ImGui::End();
}