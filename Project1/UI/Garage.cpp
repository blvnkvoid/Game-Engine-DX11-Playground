#include "Garage.h"
#include "../Imgui/imgui.h"
#include "../Cars/VehicleRegistry.h"

void Garage::Draw(const UIContext& ui)
{
    if (ImGui::Button("GARAGE / SELECT CAR", ui.Size(300, 50))) {

        m_ShowGarage = !m_ShowGarage;
    }
    if (!m_ShowGarage)
        return;

    if (m_ShowGarage) {
        ImGui::SetNextWindowPos(ui.P(400, 100)); // Place it next to the main menu
        ImGui::Begin("Car List", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        for (const auto& vehicle : VehicleRegistry::GetVehicleTable())
        {
            if (ImGui::Selectable(
                vehicle.displayName,
                m_PreviewSelection == vehicle.selection))
            {
                m_PreviewSelection = vehicle.selection;
            }
        }
        ImGui::End();
    }
}