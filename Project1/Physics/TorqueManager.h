#pragma once
#include "../SharedTypes.h"
#include "../SharedVehicleTypes.h"

class TorqueManager {
public:
    TorqueManager();
    TorqueOutput Update(float throttle, float currentSpeed, float dt);
    void ShiftUp();
    void ShiftDown();
    int GetCurrentGear();
    void SetVehicleDefinition(const VehicleDefinition& definition);
private:
    float m_maxTorque;
    float m_idleRPM;
    float m_maxRPM;
    float m_gearRatios[8];
    float m_finalDrive=0.0f;
    int m_currentGear = 2;
    float m_currentRPM = 800.0f;
    float m_filteredDrivenWheelAngularVel = 0.0f;
    float GetTorqueAtRPM(float rpm);
    EngineDefinition  m_engine;
    GearboxDefinition  m_gearbox;
};


