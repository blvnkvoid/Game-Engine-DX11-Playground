#pragma once

struct TelemetryData;
class LapTimer;

class RacingHUD
{
public:

    void SetVehicleDefinition(VehicleDefinition& car);
    void Draw(const TelemetryData& telemetry);

private:
    void DrawSpeedGearRPM(const TelemetryData& telemetry);

    float throttleHeight = 0.0f;
    float brakeHeight = 0.0f;
    VehicleDefinition m_hudvehicle;
};