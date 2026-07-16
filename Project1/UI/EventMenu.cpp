#include "EventMenu.h"
#include "../Imgui/imgui.h"
void EventMenu::Draw(const UIContext& ui)
{
    if (ImGui::Button("EVENT SELECTION", ui.Size(300, 50)))
    {
        m_ShowEventMenu = !m_ShowEventMenu;
    }

    if (!m_ShowEventMenu)
        return;

    ImGui::SetNextWindowPos(ui.P(400, 100));
    ImGui::Begin("Event List", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Events");

    for (const auto& event : g_EventList)
    {
        if (ImGui::Selectable(
            event.displayName,
            m_LaunchType == EventLaunchType::SingleEvent &&
            m_SelectedEvent == event.selection))
        {
            m_SelectedEvent = event.selection;
            m_LaunchType = EventLaunchType::SingleEvent;
        }
    }

    ImGui::Separator();
    ImGui::Text("Championships");

    for (const auto& championship : g_ChampionshipList)
    {
        if (ImGui::Selectable(
            championship.displayName,
            m_LaunchType == EventLaunchType::Championship &&
            m_SelectedChampionship == championship.selection))
        {
            m_SelectedChampionship = championship.selection;
            m_LaunchType = EventLaunchType::Championship;
        }
    }


    ImGui::Separator();

    ImGui::TextDisabled("More events coming soon...");

    ImGui::End();
}