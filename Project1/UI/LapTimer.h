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
    int m_currentLap = 0;
    int m_totalLaps = 0;
    std::string FormatTime(float timeInSeconds);
    bool  m_raceFinished = false;
public:
    LapTimer() = default;
    ~LapTimer() = default;
    

    void Update(float deltaTime);
    int GetCurrentLap() const { return m_currentLap; }
    void DrawUI();
    void TriggerStartMeta();
    void TriggerSector1();
    void TriggerSector2();

    void SetTotalLaps(int laps)
    {
        m_totalLaps = laps;
    }

    bool HasFinishedRace() const
    {
        return m_totalLaps > 0 && m_currentLap > m_totalLaps;
    }   
    void Reset();
};