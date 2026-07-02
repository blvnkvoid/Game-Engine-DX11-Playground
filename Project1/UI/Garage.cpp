#include "Garage.h"
#include "../Imgui/imgui.h"

void Garage::Draw()
{
    if (ImGui::Button("GARAGE / SELECT CAR", ImVec2(300, 50))) {

        m_ShowGarage = !m_ShowGarage;
    }

    if (!m_ShowGarage)
        return;

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
}