#include "../SharedVehicleTypes.h"
#include "LapTimer.h"
#include "RacingHUD.h"
#include "../Imgui/imgui.h"

static float Clamp01(float v)
{
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

void RacingHUD::SetVehicleDefinition(
    VehicleDefinition& car)
{
    m_hudvehicle = car;
}

void RacingHUD::Draw(const TelemetryData& telemetry)
{
    DrawSpeedGearRPM(telemetry);
}

void RacingHUD::DrawSpeedGearRPM(const TelemetryData& telemetry)
{
    ImGuiIO& io = ImGui::GetIO();

    const float width = 400.0f;
    const float height = 140.0f;

    const float pedalBarWidth = 20.0f;
    const float pedalBarHeight = 100.0f;

    const ImVec2 gBoxPos(80.0f, io.DisplaySize.y - 145.0f);
    const float gBoxSize = 70.0f;
    const float maxG = 2.0f;



    ImDrawList* drawList = ImGui::GetForegroundDrawList();

    ImVec2 brakePos(1180.0f, io.DisplaySize.y - 180.0f);
    ImVec2 throttlePos(1150.0f, io.DisplaySize.y - 180.0f);

    float brakeFill = pedalBarHeight * Clamp01(telemetry.brake);
    float throttleFill = pedalBarHeight * Clamp01(telemetry.throttle);

   drawList->AddRectFilled(
        brakePos,
        ImVec2(brakePos.x + pedalBarWidth, brakePos.y + pedalBarHeight),
        IM_COL32(40, 40, 40, 180));

    drawList->AddRectFilled(
        throttlePos,
        ImVec2(throttlePos.x + pedalBarWidth, throttlePos.y + pedalBarHeight),
        IM_COL32(40, 40, 40, 180));

    drawList->AddRectFilled(
        ImVec2(brakePos.x, brakePos.y + pedalBarHeight - brakeFill),
        ImVec2(brakePos.x + pedalBarWidth, brakePos.y + pedalBarHeight),
        IM_COL32(255, 0, 0, 255));

    drawList->AddRectFilled(
        ImVec2(throttlePos.x, throttlePos.y + pedalBarHeight - throttleFill),
        ImVec2(throttlePos.x + pedalBarWidth, throttlePos.y + pedalBarHeight),
        IM_COL32(0, 80, 255, 255));
    
    drawList->AddRect(
        gBoxPos,
        ImVec2(gBoxPos.x + gBoxSize, gBoxPos.y + gBoxSize),
        IM_COL32(255, 255, 255, 120));

    ImVec2 center(
        gBoxPos.x + gBoxSize * 0.5f,
        gBoxPos.y + gBoxSize * 0.5f);

    float x = Clamp01((telemetry.lateralG / maxG) * 0.5f + 0.5f);
    float y = Clamp01((-telemetry.longitudinalG / maxG) * 0.5f + 0.5f);

    ImVec2 dot(
        gBoxPos.x + x * gBoxSize,
        gBoxPos.y + y * gBoxSize);

    drawList->AddCircleFilled(
        dot,
        5.0f,
        IM_COL32(120, 170, 255, 255));



    ImGui::SetNextWindowPos(
        ImVec2(
            io.DisplaySize.x * 0.5f - width * 0.5f,
            io.DisplaySize.y - height - 40.0f),
        ImGuiCond_Always);

    ImGui::SetNextWindowSize(
        ImVec2(width, height),
        ImGuiCond_Always);

    ImGui::Begin(
        "RACE_HUD_SPEED",
        nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBackground);

    ImGui::SetWindowFontScale(2.5f);

    ImGui::SetCursorPos(ImVec2(10.0f, 10.0f));
    ImGui::Text("%d", telemetry.gear);

    ImGui::SetCursorPos(ImVec2(155.0f, 10.0f));
    ImGui::Text("%.0f", telemetry.speed);

    ImGui::SetWindowFontScale(1.0f);

    ImGui::SetCursorPos(ImVec2(40.0f, 20.0f));
    ImGui::Text("gear");

    ImGui::SetCursorPos(ImVec2(235.0f, 20.0f));
    ImGui::Text("km/h");

    float rpmPercent = Clamp01(telemetry.rpm / m_hudvehicle.engine.redlineRPM);
    ImGui::SetCursorPos(ImVec2(0.0f, 50.0f));
    ImGui::ProgressBar(
        rpmPercent,
        ImVec2(350.0f, 20.0f));


    ImGui::SetCursorPos(ImVec2(10.0f, 80.0f));
    ImGui::Text(
        "RPM: %.0f",
        telemetry.rpm);

    ImGui::End();
}