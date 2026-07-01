    #pragma once

    class Time
    {
    public:
        void Update(float dt);

        void SetTime(float seconds);
        void AddTime(float seconds);

        void PauseTime(bool pause);
        void SetTimeScale(float scale);

        float GetTime() const;

        

    private:
        float m_time = 180.0f;
        float m_timeScale = 1.0f;
        bool m_timePaused = false;

    };