#pragma once
#include "../SharedTypes.h"
#include "../Imgui/imgui.h"
#include <deque>

class VehicleTelemetry {
public:
    VehicleTelemetry() : m_font(nullptr) {}
    void SetFont(ImFont* font) { m_font = font; }

    // This handles the entire Debug UI logic
    void Draw(bool* p_open, class Camera* cam, class Model* playerModel);

private:
    struct TelemetryAverage {
        std::deque<float> samples;
        float sum = 0.0f;

        float AddSample(float value) {
            samples.push_back(value);
            sum += value;

            while (samples.size() > 500) {
                sum -= samples.front();
                samples.pop_front();
            }

            return sum / static_cast<float>(samples.size());
        }
    };

    TelemetryAverage m_slipRatioAvg[4];
    TelemetryAverage m_wheelLoadAvg[4];
    TelemetryAverage m_wheelLoadSmoothed[4];
    TelemetryAverage m_lateralForceAvg[4];
    TelemetryAverage m_diff[4];
    TelemetryAverage m_comp[4];
    TelemetryAverage m_frictionForce[4];
    TelemetryAverage m_slipGrip[4];
    TelemetryAverage m_longitudinalForce[4];
    TelemetryAverage m_diffLockTorque[4];
    TelemetryAverage m_driveTorqueLeft;
    TelemetryAverage m_driveTorqueRight;



    float m_displaySlipRatio[4] = {};
    float m_displayWheelLoad[4] = {};
    float m_displaywheelLoadSmoothed[4] = {};
    float m_displayLateralForce[4] = {};
    float m_displayDiff[4] = {};
    float m_displayComp[4] = {};
    float m_displayfrictionForce[4] = {};
    float m_displayslipGrip[4] = {};
    float m_displaylongitudinalForce[4] = {};
    float m_displaydiffLockTorque[4] = {};
    float m_displaydriveTorqueLeft = {};
    float m_displaydriveTorqueRight = {};
    ImFont* m_font;
};