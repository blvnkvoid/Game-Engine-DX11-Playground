#include "Time.h"


    void Time::Update(float dt)
    {
        if (!m_timePaused)
            m_time += dt * m_timeScale;
    }

    void Time::SetTime(float time)
    {
        m_time = time;
    }

    void Time::AddTime(float seconds)
    {
        m_time += seconds;
    }

    void Time::SetTimeScale(float scale)
    {
        m_timeScale = scale;
    }

    void Time::PauseTime(bool paused)
    {
        m_timePaused = paused;
    }

    float Time::GetTime() const
    {
        return m_time;
    }