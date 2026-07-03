#include "Time.h"


    void Time::Update(float dt)
    {
        if (!m_timePaused)
            m_time += dt * m_timeScale;
        m_shadertime += dt * m_shadertimeScale;
    }

    void Time::SetTime(float time)
    {
        m_time = time;
    }

    void Time::AddTime(float seconds)
    {
        m_time += seconds;
    }

    void Time::SetShaderTime(float time)
    {
        m_shadertime = time;
    }

    void Time::AddShaderTime(float seconds)
    {
        m_shadertime += seconds;
    }

    void Time::SetTimeScale(float scale)
    {
        m_timeScale = scale;
    }

    void Time::SetShaderTimeScale(float shaderscale)
    {
        m_shadertimeScale = shaderscale;
    }

    void Time::PauseTime(bool paused)
    {
        m_timePaused = paused;
    }

    float Time::GetTime() const
    {
        return m_time;
    }

    float Time::GetShaderTime() const
    {
        return m_shadertime;
    }