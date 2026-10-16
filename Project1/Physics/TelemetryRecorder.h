#pragma once

#include <fstream>
#include <string>
#include "../SharedVehicleTypes.h"

class TelemetryRecorder
{
public:
    bool Start(const std::string& filename);
    void Stop();

    void Toggle(const std::string& filename);

    void Record(
        float deltaTime,
        const SharedVehicleInputs& inputs
    );

    bool IsRecording() const { return m_recording; }
    float GetDuration() const { return m_elapsedTime; }
    unsigned long long GetSampleCount() const { return m_sampleCount; }

private:
    std::ofstream m_file;

    bool m_recording = false;
    float m_elapsedTime = 0.0f;
    unsigned long long m_sampleCount = 0;
};