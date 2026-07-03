    #pragma once

    class Time
    {
    public:
        void Update(float dt);

        void SetTime(float seconds);
        void SetShaderTime(float seconds);
        void AddTime(float seconds);
        void AddShaderTime(float seconds);

        void PauseTime(bool pause);
        void SetTimeScale(float scale);
        void SetShaderTimeScale(float shaderscale);

        float GetTime() const;
        float GetShaderTime() const;
        

    private:
        float m_time = 0.0f;
        float m_shadertime = 0.0f;
        float m_timeScale = 1.0f;
        float m_shadertimeScale = 1.0f;
        bool m_timePaused = false;

    };