#pragma once
#include "../Imgui/imgui.h"
#include "../SharedVehicleTypes.h"
#include "../Physics/Tyres.h" // Change path if your tyre files live elsewhere
#include <algorithm>
#include <cmath>
#include <cstdio>

// Called inside the existing VehicleTelemetry tab bar.
// Uses real Tyres 4.0 evaluator, no second curve formula.
inline void DrawTyreCurveTab(const TelemetryData &t)
{
    static int wheel = 0;
    static bool signedView = true;
    const char *wheels[] = {"FL", "FR", "RL", "RR"};
    ImGui::Combo("Wheel", &wheel, wheels, 4);
    ImGui::SameLine();
    ImGui::Checkbox("Signed angles", &signedView);

    const int id = t.tyreCompoundId[wheel];
    const bool validCompound = id >= 0 && id <= static_cast<int>(TyreCompound::P2_Prototype2);
    if (!t.tyreCurveValid[wheel] || !validCompound ||
        t.maxFriction[wheel] <= 0.0f || t.tyreSideStiffness[wheel] <= 0.0f)
    {
        ImGui::TextDisabled("No valid tyre contact / curve data for this wheel.");
        return;
    }
    const auto compound = static_cast<TyreCompound>(id);
    const char *compoundNames[] = {"N1", "N2", "N3", "S1", "S2", "S3",
                                   "R1", "R2", "R3", "R4", "R5", "P1", "P2"};
    const float peak = t.tyrePeakSlipAngle[wheel];
    const float slip = t.tyreSlipAngle[wheel];
    const float maxForce = t.maxFriction[wheel];
    const float stiffness = t.tyreSideStiffness[wheel];
    const float actualForce = t.lateralRelaxedForce[wheel];
    const float targetForce = t.lateralTargetForce[wheel];
    constexpr float radToDeg = 57.29577951308232f;

    ImGui::Text("Compound: %s   Load: %.0f N   Peak: %.2f deg",
                compoundNames[id], t.wheelLoadSmoothed[wheel], peak * radToDeg);
    ImGui::Text("Slip: %+.2f deg   Actual Fy: %+.0f N   Target Fy: %+.0f N",
                slip * radToDeg, actualForce, targetForce);
    ImGui::Text("Free lateral: %+.0f N   Max grip: %.0f N",
                t.tyreFreeLateralForce[wheel], maxForce);

    const float halfRange = (std::max)(0.10f, (std::max)(peak * 2.5f, std::fabs(slip) * 1.20f));
    const float minX = signedView ? -halfRange : 0.0f;
    const float maxX = halfRange;
    const float maxY = maxForce * 1.15f;
    const ImVec2 canvas = ImGui::GetCursorScreenPos();
    const float width = (std::max)(300.0f, ImGui::GetContentRegionAvail().x);
    const float height = 300.0f;
    ImGui::InvisibleButton("##TyreCurveCanvas", ImVec2(width, height));
    ImDrawList *dl = ImGui::GetWindowDrawList();
    const ImVec2 p0(canvas.x + 55.0f, canvas.y + 18.0f);
    const ImVec2 p1(canvas.x + width - 18.0f, canvas.y + height - 34.0f);
    dl->AddRectFilled(canvas, ImVec2(canvas.x + width, canvas.y + height), IM_COL32(15, 19, 25, 215));
    const auto project = [&](float x, float y) -> ImVec2
    {
        return ImVec2(p0.x + (x - minX) / (maxX - minX) * (p1.x - p0.x),
                      p1.y - (y + maxY) / (2.0f * maxY) * (p1.y - p0.y));
    };
    for (int i = 0; i <= 4; ++i)
    {
        const float y = -maxY + 2.0f * maxY * i / 4.0f;
        const ImVec2 a = project(minX, y), b = project(maxX, y);
        dl->AddLine(a, b, IM_COL32(80, 90, 100, 75));
        char label[32];
        snprintf(label, sizeof(label), "%.0f", y);
        dl->AddText(ImVec2(canvas.x + 2.0f, a.y - 6.0f), IM_COL32(180, 190, 200, 255), label);
    }
    for (int i = 0; i <= 6; ++i)
    {
        const float x = minX + (maxX - minX) * i / 6.0f;
        const ImVec2 a = project(x, -maxY), b = project(x, maxY);
        dl->AddLine(a, b, IM_COL32(80, 90, 100, 55));
        char label[32];
        snprintf(label, sizeof(label), "%.1f", x * radToDeg);
        dl->AddText(ImVec2(a.x - 12.0f, p1.y + 5.0f), IM_COL32(180, 190, 200, 255), label);
    }
    dl->AddLine(project(minX, 0), project(maxX, 0), IM_COL32(190, 190, 190, 150));
    dl->AddLine(project(0, -maxY), project(0, maxY), IM_COL32(190, 190, 190, 150));
    constexpr int segments = 200;
    for (int i = 1; i <= segments; ++i)
    {
        const float a0 = minX + (maxX - minX) * (i - 1) / segments;
        const float a1 = minX + (maxX - minX) * i / segments;
        const float f0 = Tire::SampleLateralCurve(compound, a0, maxForce, stiffness);
        const float f1 = Tire::SampleLateralCurve(compound, a1, maxForce, stiffness);
        dl->AddLine(project(a0, f0), project(a1, f1), IM_COL32(65, 220, 135, 255), 2.3f);
    }
    // Actual output can be off the ideal curve due to combined slip and relaxation.
    if (slip >= minX && slip <= maxX)
    {
        const ImVec2 dot = project(slip, actualForce);
        dl->AddCircleFilled(dot, 5.5f, IM_COL32(245, 65, 65, 255));
        dl->AddCircle(dot, 6.5f, IM_COL32(255, 255, 255, 255));
        const ImVec2 target = project(slip, targetForce);
        dl->AddCircle(target, 4.0f, IM_COL32(250, 190, 65, 255), 0, 1.7f);
    }
    else
    {
        ImGui::TextDisabled("Current slip outside displayed angle range.");
    }
    ImGui::TextColored(ImVec4(.25f, .86f, .53f, 1), "Green: free lateral curve");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(.98f, .3f, .3f, 1), "Red: actual force");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1, .75f, .25f, 1), "Yellow: coupled target");
}
