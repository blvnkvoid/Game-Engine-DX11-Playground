#include "TorqueManager.h"
#include "../Input/Input.h"



Input input;


static bool R2_Pressed = false;
static bool L2_Pressed = false;

float triggerThreshold = 0.9f;

TorqueManager::TorqueManager()
{

}

void TorqueManager::SetVehicleDefinition(const VehicleDefinition& definition)
{
    m_engine = definition.engine;
    m_gearbox = definition.gearbox;

    m_idleRPM = m_engine.idleRPM;
    m_maxRPM = m_engine.redlineRPM;
    m_maxTorque = m_engine.maxTorque;

    m_finalDrive = m_gearbox.finalDrive;

    for (int i = 0; i < 8; i++)
        m_gearRatios[i] = m_gearbox.gearRatios[i];

    m_currentRPM = m_idleRPM;
}

TorqueOutput TorqueManager::Update(float throttle, float drivenWheelAngularVel, float dt) {

    input.GetControllerState();
    ShiftUp();
    ShiftDown();

    TorqueOutput out;   
    float wheelAlpha = 1.0f - exp(-25.0f * dt);

    m_filteredDrivenWheelAngularVel +=
        (drivenWheelAngularVel - m_filteredDrivenWheelAngularVel) * wheelAlpha;

    float rawRPM =
        (m_filteredDrivenWheelAngularVel * m_gearRatios[m_currentGear] * m_finalDrive)
        * 60.0f / (2.0f * 3.14159f);

    float targetRPM = max(m_idleRPM, min(m_engine.limiterRPM, rawRPM));

    float rpmAlpha = 1.0f - exp(-18.0f * dt);

    m_currentRPM += (targetRPM - m_currentRPM) * rpmAlpha;
    float engineTorque = GetTorqueAtRPM(m_currentRPM) * throttle;

    float limiterStart = m_engine.limiterRPM - 150.0f;


    if (m_currentRPM > limiterStart)
    {
        float over = (m_currentRPM - limiterStart) / 150.0f;
        over = max(0.0f, min(1.0f, over));

        engineTorque *= (1.0f - over);
    }
    float totalDriveTorque = engineTorque * m_gearRatios[m_currentGear] * m_finalDrive;
    out.wheelTorque[0] = 0.0f;
    out.wheelTorque[1] = 0.0f;
    out.wheelTorque[2] = totalDriveTorque * 0.5f;
    out.wheelTorque[3] = totalDriveTorque * 0.5f;

    out.engineRPM = m_currentRPM;
    out.currentGear = m_currentGear;
    out.driveTorque = totalDriveTorque;

    g_DebugTelemetry.rpm = m_currentRPM;
    g_DebugTelemetry.torque = engineTorque;


    return out;
}

void TorqueManager::ShiftUp()
{
    if (Input::GetCurrentInputs().shiftUp) {
        if(!R2_Pressed){
            m_currentGear++;
            if (m_currentGear > 7) m_currentGear = 7;
            R2_Pressed = true;
        }
        }
    else {
        R2_Pressed = false;
    }
}


void TorqueManager::ShiftDown()
{
    if (Input::GetCurrentInputs().shiftDown) {
         {
            if (!L2_Pressed) {
                m_currentGear--;
                if (m_currentGear < 0) m_currentGear = 0;
                L2_Pressed = true;
            }
        }
    }
    else {
        L2_Pressed = false;
    }    
}

float TorqueManager::GetTorqueAtRPM(float rpm)
{
    if (m_engine.torqueCurveCount <= 0)
        return m_maxTorque;

    if (rpm <= m_engine.torqueCurve[0].rpm)
        return m_maxTorque * m_engine.torqueCurve[0].torqueMultiplier;

    for (int i = 0; i < m_engine.torqueCurveCount - 1; i++)
    {
        auto a = m_engine.torqueCurve[i];
        auto b = m_engine.torqueCurve[i + 1];

        if (rpm <= b.rpm)
        {
            float t = (rpm - a.rpm) / (b.rpm - a.rpm);
            return m_maxTorque * (a.torqueMultiplier + t * (b.torqueMultiplier - a.torqueMultiplier));
        }
    }

    return m_maxTorque * m_engine.torqueCurve[m_engine.torqueCurveCount - 1].torqueMultiplier;
}

int TorqueManager::GetCurrentGear()
{

    g_DebugTelemetry.gear = (m_currentGear-1);
    return m_currentGear;
}