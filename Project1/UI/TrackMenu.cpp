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
        if (ImGui::Selectable("AutumnRing", GameConfig::activeTrack == TrackSelection::AutumnRing)) {

            GameConfig::activeTrack = TrackSelection::AutumnRing;
        }
        if (ImGui::Selectable("ElCapitan", GameConfig::activeTrack == TrackSelection::ElCapitan)) {

            GameConfig::activeTrack = TrackSelection::ElCapitan;
        }
        if (ImGui::Selectable("Circuit de Spa-Francorchamps ", GameConfig::activeTrack == TrackSelection::Spa)) {

            GameConfig::activeTrack = TrackSelection::Spa;
        }
        if (ImGui::Selectable("Grand Valley Speedway ", GameConfig::activeTrack == TrackSelection::GrandValley)) {

            GameConfig::activeTrack = TrackSelection::GrandValley;
        }
        if (ImGui::Selectable("Trial Mountain Circuit", GameConfig::activeTrack == TrackSelection::TrialMountain)) {

            GameConfig::activeTrack = TrackSelection::TrialMountain;
        }
        if (ImGui::Selectable("High Speed Ring ", GameConfig::activeTrack == TrackSelection::HighSpeedRing)) {

            GameConfig::activeTrack = TrackSelection::HighSpeedRing;
        }
        if (ImGui::Selectable("Mid-field Raceway ", GameConfig::activeTrack == TrackSelection::MidfieldRaceway)) {

            GameConfig::activeTrack = TrackSelection::MidfieldRaceway;
        }
        if (ImGui::Selectable("Test Course ", GameConfig::activeTrack == TrackSelection::TestCourse)) {

            GameConfig::activeTrack = TrackSelection::TestCourse;
        }
        if (ImGui::Selectable("Special Stage Route X ", GameConfig::activeTrack == TrackSelection::RouteX)) {

            GameConfig::activeTrack = TrackSelection::RouteX;
        }

        if (ImGui::Selectable("Nurburgring Nordschleife ", GameConfig::activeTrack == TrackSelection::Nordschleife)) {

            GameConfig::activeTrack = TrackSelection::Nordschleife;
        }

        if (ImGui::Selectable("Beginner Course", GameConfig::activeTrack == TrackSelection::BeginnerCourse)) {

            GameConfig::activeTrack = TrackSelection::BeginnerCourse;
        }
        if (ImGui::Selectable("Motor Sports Land ", GameConfig::activeTrack == TrackSelection::Motorland)) {

            GameConfig::activeTrack = TrackSelection::Motorland;
        }

        if (ImGui::Selectable("Tsukuba Circuit", GameConfig::activeTrack == TrackSelection::Tsukuba)) {

            GameConfig::activeTrack = TrackSelection::Tsukuba;
        }

        if (ImGui::Selectable("Circuit de la Sarthe", GameConfig::activeTrack == TrackSelection::LeMans)) {

            GameConfig::activeTrack = TrackSelection::LeMans;
        }

        if (ImGui::Selectable("Deep Forest Raceway", GameConfig::activeTrack == TrackSelection::DeepForest)) {

            GameConfig::activeTrack = TrackSelection::DeepForest;
        }

        if (ImGui::Selectable("Special Stage Route 5", GameConfig::activeTrack == TrackSelection::SSR5)) {

            GameConfig::activeTrack = TrackSelection::SSR5;
        }



        ImGui::End();
    }

}