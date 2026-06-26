#include "LapTimer.h"
#include "../Imgui/imgui.h"

void LapTimer::Update(float deltaTime) {
    if (m_timerRunning) {
        m_currentLapTime += deltaTime;
    }

    if (m_isNewBestLap) {
        m_bestFlashTimer -= deltaTime;
        if (m_bestFlashTimer <= 0.0f) {
            m_isNewBestLap = false;
        }
    }
}

void LapTimer::DrawUI() {
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250, 110), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize;

    if (ImGui::Begin("Toasty Lap Timer", nullptr, flags)) {
        if (m_isNewBestLap) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "!! NEW BEST LAP !!");
        }
        else {
            ImGui::Text("SPA-FRANCORCHAMPS Debug HUD");
        }

        ImGui::Separator();
        ImGui::Text("CURRENT: %s", FormatTime(m_currentLapTime).c_str());
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "BEST   : %s", FormatTime(m_bestLapTime).c_str());
        ImGui::Text("LAST   : %s", FormatTime(m_lastLapTime).c_str());
        ImGui::Text("SECTOR : %d", m_currentSector + 1);

        ImGui::End();
    }
}

void LapTimer::TriggerStartMeta() {
    if (!m_timerRunning) {
        m_timerRunning = true;
        m_currentLapTime = 0.0f;
        m_currentSector = 0;
        return;
    }

    if (m_currentSector == 2) {
        m_lastLapTime = m_currentLapTime;
        if (m_bestLapTime == 0.0f || m_lastLapTime < m_bestLapTime) {
            m_bestLapTime = m_lastLapTime;
            m_isNewBestLap = true;
            m_bestFlashTimer = 3.0f; // Błyskaj na zielono przez 3 sekundy
        }
    }
    m_currentLapTime = 0.0f;
    m_currentSector = 0;
}

void LapTimer::TriggerSector1() { if (m_currentSector == 0) m_currentSector = 1; }
void LapTimer::TriggerSector2() { if (m_currentSector == 1) m_currentSector = 2; }

std::string LapTimer::FormatTime(float timeInSeconds) {
    if (timeInSeconds <= 0.0f) return "00:00.000";
    int minutes = static_cast<int>(timeInSeconds) / 60;
    int seconds = static_cast<int>(timeInSeconds) % 60;
    int milliseconds = static_cast<int>((timeInSeconds - static_cast<int>(timeInSeconds)) * 1000);
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%02d:%02d.%03d", minutes, seconds, milliseconds);
    return std::string(buffer);
}