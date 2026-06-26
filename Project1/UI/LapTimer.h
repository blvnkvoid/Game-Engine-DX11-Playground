#pragma once
#include <string>

class LapTimer {
private:
    float m_currentLapTime = 0.0f;
    float m_bestLapTime = 0.0f;
    float m_lastLapTime = 0.0f;
    int   m_currentSector = 0;
    bool  m_timerRunning = false;
    bool  m_isNewBestLap = false;
    float m_bestFlashTimer = 0.0f;

    std::string FormatTime(float timeInSeconds);

public:
    LapTimer() = default;
    ~LapTimer() = default;

    void Update(float deltaTime);

    void DrawUI();
    void TriggerStartMeta();
    void TriggerSector1();
    void TriggerSector2();
};