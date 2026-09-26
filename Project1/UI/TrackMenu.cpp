#include "TrackMenu.h"
#include "../Imgui/imgui.h"

void TrackMenu::Draw(const UIContext& ui)
{
    if (ImGui::Button("TRACK SELECTION", ui.Size(300, 50))) {

        m_TrackSelection = !m_TrackSelection;
    }

    if (!m_TrackSelection)
        return;


    if (m_TrackSelection)
    {
        ImGui::SetNextWindowPos(ui.P(400, 100)); // Place it next to the main menu
        ImGui::SetNextWindowSize(ui.Size(300, 0), ImGuiCond_Always);
        ImGui::Begin("Track List", nullptr, 0);
        if (ImGui::Selectable("AutumnRing", m_selectedTrack == TrackSelection::AutumnRing))
        {
            m_selectedTrack = TrackSelection::AutumnRing;
        }

        if (ImGui::Selectable("ElCapitan", m_selectedTrack == TrackSelection::ElCapitan))
        {
            m_selectedTrack = TrackSelection::ElCapitan;
        }

        if (ImGui::Selectable("Circuit de Spa-Francorchamps", m_selectedTrack == TrackSelection::Spa))
        {
            m_selectedTrack = TrackSelection::Spa;
        }

        if (ImGui::Selectable("Grand Valley Speedway", m_selectedTrack == TrackSelection::GrandValley))
        {
            m_selectedTrack = TrackSelection::GrandValley;
        }

        if (ImGui::Selectable("Trial Mountain Circuit", m_selectedTrack == TrackSelection::TrialMountain))
        {
            m_selectedTrack = TrackSelection::TrialMountain;
        }

        if (ImGui::Selectable("High Speed Ring", m_selectedTrack == TrackSelection::HighSpeedRing))
        {
            m_selectedTrack = TrackSelection::HighSpeedRing;
        }

        if (ImGui::Selectable("Mid-field Raceway", m_selectedTrack == TrackSelection::MidfieldRaceway))
        {
            m_selectedTrack = TrackSelection::MidfieldRaceway;
        }

        if (ImGui::Selectable("Test Course", m_selectedTrack == TrackSelection::TestCourse))
        {
            m_selectedTrack = TrackSelection::TestCourse;
        }

        if (ImGui::Selectable("Special Stage Route X", m_selectedTrack == TrackSelection::RouteX))
        {
            m_selectedTrack = TrackSelection::RouteX;
        }

        if (ImGui::Selectable("Nurburgring Nordschleife", m_selectedTrack == TrackSelection::Nordschleife))
        {
            m_selectedTrack = TrackSelection::Nordschleife;
        }

        if (ImGui::Selectable("Beginner Course", m_selectedTrack == TrackSelection::BeginnerCourse))
        {
            m_selectedTrack = TrackSelection::BeginnerCourse;
        }

        if (ImGui::Selectable("Motor Sports Land", m_selectedTrack == TrackSelection::Motorland))
        {
            m_selectedTrack = TrackSelection::Motorland;
        }

        if (ImGui::Selectable("Tsukuba Circuit", m_selectedTrack == TrackSelection::Tsukuba))
        {
            m_selectedTrack = TrackSelection::Tsukuba;
        }

        if (ImGui::Selectable("Circuit de la Sarthe", m_selectedTrack == TrackSelection::LeMans))
        {
            m_selectedTrack = TrackSelection::LeMans;
        }

        if (ImGui::Selectable("Deep Forest Raceway", m_selectedTrack == TrackSelection::DeepForest))
        {
            m_selectedTrack = TrackSelection::DeepForest;
        }

        if (ImGui::Selectable("Special Stage Route 5", m_selectedTrack == TrackSelection::SSR5))
        {
            m_selectedTrack = TrackSelection::SSR5;
        }

        if (ImGui::Selectable("Suzuka Circuit", m_selectedTrack == TrackSelection::Suzuka))
        {
            m_selectedTrack = TrackSelection::Suzuka;
        }

        if (ImGui::Selectable("San Andreas", m_selectedTrack == TrackSelection::SanAndreas))
        {
            m_selectedTrack = TrackSelection::SanAndreas;
        }

        if (ImGui::Selectable("Bathurst", m_selectedTrack == TrackSelection::Bathurst))
        {
            m_selectedTrack = TrackSelection::Bathurst;
        }

        if (ImGui::Selectable("Bayview", m_selectedTrack == TrackSelection::Bayview))
        {
            m_selectedTrack = TrackSelection::Bayview;
        }





        ImGui::End();
    }

}