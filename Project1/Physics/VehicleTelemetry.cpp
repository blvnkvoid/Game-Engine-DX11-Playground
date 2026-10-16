#include "VehicleTelemetry.h"
#include <type_traits>
#include "../Scene/Camera.h"
#include "../Graphics/CarLoader.h"
#include "../LogitechSteeringWheelLib.h"
#include "../SharedVehicleTypes.h"

void VehicleTelemetry::Draw(bool* p_open, Camera* cam, CarLoader* playerModel, MapLoader* map) {
    if (!*p_open) return;

    if (m_font) ImGui::PushFont(m_font);

    // UI-only telemetry snapshot. Physics and the CSV recorder still use raw g_DebugTelemetry.
    // A held snapshot prevents numbers from changing on every rendered frame.
    using TelemetrySnapshot = std::decay_t<decltype(g_DebugTelemetry)>;
    static TelemetrySnapshot shownTelemetry{};
    static bool shownInitialized = false;
    static double nextTelemetryRefresh = 0.0;
    const double now = ImGui::GetTime();
    const bool refreshTelemetry = !shownInitialized || now >= nextTelemetryRefresh;
    if (refreshTelemetry) {
        if (!shownInitialized) {
            shownTelemetry = g_DebugTelemetry;
            shownInitialized = true;
        }
        else {
            // Copy raw data first so discrete fields stay discrete. Blend only
            // numerical fields actually shown in the vehicle tabs.
            const auto previous = shownTelemetry;
            shownTelemetry = g_DebugTelemetry;
            const auto blend = [](float oldValue, float newValue, float alpha) {
                return oldValue + alpha * (newValue - oldValue);
            };
            // 0.25 for noisy force/torque/slip channels; 0.45 elsewhere.
#define SMOOTH_WHEEL(field, alpha) for (int wheel = 0; wheel < 4; ++wheel) { shownTelemetry.field[wheel] = blend(previous.field[wheel], g_DebugTelemetry.field[wheel], alpha); }
#define SMOOTH_GLOBAL(field, alpha) shownTelemetry.field = blend(previous.field, g_DebugTelemetry.field, alpha)
            SMOOTH_WHEEL(dampingForce, 0.25f);
            SMOOTH_WHEEL(wheelAngularAcceleration, 0.25f);
            SMOOTH_WHEEL(slip, 0.25f);
            SMOOTH_WHEEL(tyreTheta, 0.25f);
            SMOOTH_WHEEL(lateralForce, 0.25f);
            SMOOTH_WHEEL(longitudinalForce, 0.25f);
            SMOOTH_WHEEL(drivingForce, 0.25f);
            SMOOTH_WHEEL(tireReactionTorque, 0.25f);
            SMOOTH_WHEEL(wheelNetTorque, 0.25f);
            SMOOTH_WHEEL(reactionTorqueForceMismatch, 0.25f);
            SMOOTH_WHEEL(combinedTotalForce, 0.25f);
            SMOOTH_WHEEL(diffLockTorque, 0.25f);
            SMOOTH_WHEEL(wheelTorque, 0.25f);
            SMOOTH_WHEEL(driveTorque, 0.25f);
            SMOOTH_WHEEL(brakeTorque, 0.25f);
            SMOOTH_WHEEL(wheelAngularVelocity, 0.45f);
            SMOOTH_WHEEL(roadAngularVelocity, 0.45f);
            SMOOTH_WHEEL(wheelVx, 0.45f);
            SMOOTH_WHEEL(wheelVy, 0.45f);
            SMOOTH_WHEEL(localForwardVel, 0.45f);
            SMOOTH_WHEEL(localSideVel, 0.45f);
            SMOOTH_WHEEL(bodyForwardVel, 0.45f);
            SMOOTH_WHEEL(bodySideVel, 0.45f);
            SMOOTH_WHEEL(bodySlipAngle, 0.45f);
            SMOOTH_WHEEL(camberDeg, 0.45f);
            SMOOTH_WHEEL(camberCompression, 0.45f);
            SMOOTH_WHEEL(steeringAngle, 0.45f);
            SMOOTH_WHEEL(combinedMaxGrip, 0.45f);
            SMOOTH_WHEEL(longitudinalPeakForce, 0.45f);
            SMOOTH_WHEEL(tyreForceHeightError, 0.45f);
            SMOOTH_GLOBAL(lateralG, 0.45f);
            SMOOTH_GLOBAL(longitudinalG, 0.45f);
            SMOOTH_GLOBAL(yawRate, 0.45f);
            SMOOTH_GLOBAL(diffSpeed, 0.25f);
            SMOOTH_GLOBAL(diffTargetTorque, 0.25f);
            SMOOTH_GLOBAL(diffLoadBiasTorque, 0.25f);
            SMOOTH_GLOBAL(diffTotalTorque, 0.25f);
            SMOOTH_GLOBAL(dragMagnitude, 0.45f);
            SMOOTH_GLOBAL(downforceMagnitude, 0.45f);
            SMOOTH_GLOBAL(frontDownforce, 0.45f);
            SMOOTH_GLOBAL(rearDownforce, 0.45f);
#undef SMOOTH_WHEEL
#undef SMOOTH_GLOBAL
        }
        // Avoid displaying tiny +/- damping noise when the car is at rest.
        for (int wheel = 0; wheel < 4; ++wheel) {
            if (shownTelemetry.dampingForce[wheel] > -5.0f && shownTelemetry.dampingForce[wheel] < 5.0f)
                shownTelemetry.dampingForce[wheel] = 0.0f;
        }
        nextTelemetryRefresh = now + 0.20; // 10 Hz
    }

    if (refreshTelemetry) for (int i = 0; i < 4; i++) {
        m_displaySlipRatio[i] = m_slipRatioAvg[i].AddSample(shownTelemetry.slipRatio[i]);
        m_displayWheelLoad[i] = m_wheelLoadAvg[i].AddSample(shownTelemetry.wheelLoad[i]);
        m_displaywheelLoadSmoothed[i] = m_wheelLoadSmoothed[i].AddSample(shownTelemetry.wheelLoadSmoothed[i]);
        m_displayDiff[i] = m_diff[i].AddSample(shownTelemetry.diff[i]);
        m_displayComp[i] = m_comp[i].AddSample(shownTelemetry.compression[i]);
        m_displayLateralForce[i] = m_lateralForceAvg[i].AddSample(shownTelemetry.lateralForce[i]);
        m_displaylongitudinalForce[i] = m_longitudinalForce[i].AddSample(shownTelemetry.longitudinalForce[i]);
        m_displaydiffLockTorque[i] = m_diffLockTorque[i].AddSample(shownTelemetry.diffLockTorque[i]);
        m_displaydriveTorqueLeft = m_driveTorqueLeft.AddSample(shownTelemetry.driveTorque[2]);
        m_displaydriveTorqueRight = m_driveTorqueRight.AddSample(shownTelemetry.driveTorque[3]);
    }

    if (ImGui::Begin("911 Lab - Debug System", p_open)) {
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_WindowBg].w = 0.1f;




        if (ImGui::CollapsingHeader("Input State")) {
            ImGui::Text("Buttons Held:");
            for (int i = 0; i < 25; i++) {
                if (LogiButtonIsPressed(0, i)) {
                    ImGui::SameLine();
                    ImGui::Text("[%d]", i);
                }
            }
        }

        if (ImGui::CollapsingHeader("Hardware Performance")) {
            float fps = ImGui::GetIO().Framerate;
            ImGui::Text("Framerate: %.1f FPS", fps);
            ImGui::Text("Frame Time: %.3f ms", 1000.0f / fps);
            ImGui::ProgressBar((1000.0f / fps) / 16.67f, ImVec2(0.0f, 0.0f), "Frame Load");
        }

        if (ImGui::CollapsingHeader("Renderer Performance"))
        {
            const auto& stats = map->GetStats();

            ImGui::Text("Draw Calls: %u", stats.m_drawCalls);
            ImGui::Text("Culled: %u", stats.m_culledDrawCalls);
            ImGui::Text("Meshes: %u", stats.m_meshes);
            ImGui::Text("Vertices: %u", stats.m_totalVertices);
            ImGui::Text("Triangles: %u", stats.m_totalTriangles);
            ImGui::Text("Camera far plane: %.1f", cam->GetFarPlane());
            ImGui::Text("Camera near plane: %.1f", cam->GetNearPlane());
        }

        if (ImGui::CollapsingHeader("Lighting Performance"))
        {
            const auto& stats = map->GetStats();
            ImGui::Text("Total lamp count: %u", stats.lampCount);
            ImGui::Text("Active lamp count: %u", stats.activelampCount);
            ImGui::Text("Total Shadow Draw Calls: %u", stats.m_shadowdrawCalls);
            ImGui::Text("Culled Shadow Draw Calls: %u", stats.m_shadowDrawCallsCulled);
            ImGui::Text("Zero index subsets: %u", stats.zeroIndexSubsets);
            ImGui::Text("Culled Shadow Indices: %llu", stats.culledIndices);
            ImGui::Text("Rendered Shadow Indices: %llu", stats.renderedIndices);
            ImGui::Text("Shadow CPU: %.3f ms", stats.shadowCpuMs);
        }

        if (ImGui::CollapsingHeader("Car Telemetry")) {
            if (playerModel != nullptr) {
                const char* labels[] = { "FL", "FR", "RL", "RR" };
                if (ImGui::BeginTabBar("VehicleTelemetryTabs", ImGuiTabBarFlags_FittingPolicyScroll)) {
                    if (ImGui::BeginTabItem("Vehicle Dynamics")) {
                        ImGui::Text("Lateral G: %.2f", shownTelemetry.lateralG);
                        ImGui::Text("Longitudinal G: %.2f", shownTelemetry.longitudinalG);
                        ImGui::Text("Yaw Rate: %.2f", shownTelemetry.yawRate);
                        ImGui::Text("Mass (kg): %.0f", shownTelemetry.mass);
                        if (ImGui::BeginTable("VehicleDynamicsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX)) {
                            ImGui::TableSetupColumn("Wheel", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                            ImGui::TableSetupColumn("Body Slip (deg)");
                            ImGui::TableHeadersRow();
                            for (int i = 0; i < 4; ++i) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.bodySlipAngle[i]);
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Suspension")) {
                        if (ImGui::BeginTable("SuspensionTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX)) {
                            ImGui::TableSetupColumn("Wheel", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                            ImGui::TableSetupColumn("Compression (%)");
                            ImGui::TableSetupColumn("Travel Diff (m)");
                            ImGui::TableSetupColumn("Damping (N)");
                            ImGui::TableHeadersRow();
                            for (int i = 0; i < 4; ++i) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.1f", m_displayComp[i] * 100.0f);
                                ImGui::TableNextColumn(); ImGui::Text("%.3f", m_displayDiff[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", shownTelemetry.dampingForce[i]);
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Wheel Loads")) {
                        if (ImGui::BeginTable("WheelLoadsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX)) {
                            ImGui::TableSetupColumn("Wheel", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                            ImGui::TableSetupColumn("Load (N)");
                            ImGui::TableSetupColumn("Smoothed Load (N)");
                            ImGui::TableHeadersRow();
                            for (int i = 0; i < 4; ++i) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", m_displayWheelLoad[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", m_displaywheelLoadSmoothed[i]);
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Steering & Geometry")) {
                        if (ImGui::BeginTable("SteeringAndGeometryTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX)) {
                            ImGui::TableSetupColumn("Wheel", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                            ImGui::TableSetupColumn("Steering Angle (rad)");
                            ImGui::TableSetupColumn("Camber (deg)");
                            ImGui::TableSetupColumn("Camber Compression");
                            ImGui::TableHeadersRow();
                            for (int i = 0; i < 4; ++i) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.steeringAngle[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.camberDeg[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.3f", shownTelemetry.camberCompression[i]);
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Tyre Slip")) {
                        if (ImGui::BeginTable("TyreSlipTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX)) {
                            ImGui::TableSetupColumn("Wheel", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                            ImGui::TableSetupColumn("Slip Angle");
                            ImGui::TableSetupColumn("Slip Ratio");
                            ImGui::TableSetupColumn("Tyre Theta");
                            ImGui::TableHeadersRow();
                            for (int i = 0; i < 4; ++i) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.slip[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.3f", m_displaySlipRatio[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.tyreTheta[i]);
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Tyre Forces")) {
                        if (ImGui::BeginTable("TyreForcesTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX)) {
                            ImGui::TableSetupColumn("Wheel", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                            ImGui::TableSetupColumn("Lateral Force (N)");
                            ImGui::TableSetupColumn("Longitudinal Force (N)");
                            ImGui::TableSetupColumn("Driving Force (N)");
                            ImGui::TableHeadersRow();
                            for (int i = 0; i < 4; ++i) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", m_displayLateralForce[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", m_displaylongitudinalForce[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", shownTelemetry.drivingForce[i]);
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Grip & Friction")) {
                        if (ImGui::BeginTable("GripAndFrictionTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX)) {
                            ImGui::TableSetupColumn("Wheel", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                            ImGui::TableSetupColumn("Surface Mu");
                            ImGui::TableSetupColumn("Side Stiffness");
                            ImGui::TableSetupColumn("Combined Max Grip (N)");
                            ImGui::TableSetupColumn("Combined Total Force (N)");
                            ImGui::TableSetupColumn("Longitudinal Peak Force (N)");
                            ImGui::TableHeadersRow();
                            for (int i = 0; i < 4; ++i) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.surfaceMu[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", shownTelemetry.tyreSideStiffness[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", shownTelemetry.combinedMaxGrip[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", shownTelemetry.combinedTotalForce[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", shownTelemetry.longitudinalPeakForce[i]);
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Wheel Kinematics")) {
                        if (ImGui::BeginTable("WheelKinematicsTable", 10, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX)) {
                            ImGui::TableSetupColumn("Wheel", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                            ImGui::TableSetupColumn("Wheel Vx (m/s)");
                            ImGui::TableSetupColumn("Wheel Vy (m/s)");
                            ImGui::TableSetupColumn("Local Forward (m/s)");
                            ImGui::TableSetupColumn("Local Side (m/s)");
                            ImGui::TableSetupColumn("Body Forward (m/s)");
                            ImGui::TableSetupColumn("Body Side (m/s)");
                            ImGui::TableSetupColumn("Wheel Omega");
                            ImGui::TableSetupColumn("Road Omega");
                            ImGui::TableSetupColumn("Angular Acceleration");
                            ImGui::TableHeadersRow();
                            for (int i = 0; i < 4; ++i) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.wheelVx[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.wheelVy[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.localForwardVel[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.localSideVel[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.bodyForwardVel[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.bodySideVel[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.wheelAngularVelocity[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.roadAngularVelocity[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.2f", shownTelemetry.wheelAngularAcceleration[i]);
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Wheel Torque")) {
                        if (ImGui::BeginTable("WheelTorqueTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX)) {
                            ImGui::TableSetupColumn("Wheel", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                            ImGui::TableSetupColumn("Wheel Torque");
                            ImGui::TableSetupColumn("Drive Torque");
                            ImGui::TableSetupColumn("Brake Torque");
                            ImGui::TableSetupColumn("Tyre Reaction Torque");
                            ImGui::TableSetupColumn("Net Torque");
                            ImGui::TableHeadersRow();
                            for (int i = 0; i < 4; ++i) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", shownTelemetry.wheelTorque[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", shownTelemetry.driveTorque[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", shownTelemetry.brakeTorque[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", shownTelemetry.tireReactionTorque[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", shownTelemetry.wheelNetTorque[i]);
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Differential / LSD")) {
                        ImGui::Text("Diff Speed: %.2f", shownTelemetry.diffSpeed);
                        ImGui::Text("Target Torque: %.0f", shownTelemetry.diffTargetTorque);
                        ImGui::Text("Load Bias Torque: %.0f", shownTelemetry.diffLoadBiasTorque);
                        ImGui::Text("Total Diff Torque: %.0f", shownTelemetry.diffTotalTorque);
                        if (ImGui::BeginTable("DifferentialLSDTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX)) {
                            ImGui::TableSetupColumn("Wheel", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                            ImGui::TableSetupColumn("Diff Lock Torque");
                            ImGui::TableHeadersRow();
                            for (int i = 0; i < 4; ++i) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", shownTelemetry.diffLockTorque[i]);
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Contact Diagnostics")) {
                        if (ImGui::BeginTable("ContactDiagnosticsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX)) {
                            ImGui::TableSetupColumn("Wheel", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                            ImGui::TableSetupColumn("Ray Hit");
                            ImGui::TableSetupColumn("Tyre Force Height Error");
                            ImGui::TableSetupColumn("Reaction Torque Force Mismatch");
                            ImGui::TableHeadersRow();
                            for (int i = 0; i < 4; ++i) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.0f", shownTelemetry.raycastHitType[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.3f", shownTelemetry.tyreForceHeightError[i]);
                                ImGui::TableNextColumn(); ImGui::Text("%.3f", shownTelemetry.reactionTorqueForceMismatch[i]);
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Aerodynamics")) {
                        ImGui::Text("Drag Magnitude: %.0f", shownTelemetry.dragMagnitude);
                        ImGui::Text("Downforce Magnitude: %.0f", shownTelemetry.downforceMagnitude);
                        ImGui::Text("Front Downforce: %.0f", shownTelemetry.frontDownforce);
                        ImGui::Text("Rear Downforce: %.0f", shownTelemetry.rearDownforce);
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }
        }
        if (playerModel == nullptr)
        {

            ImGui::TextColored(ImVec4(1, 1, 0, 1), "No car model loaded.");
        }
        else {

            DirectX::XMFLOAT3 pos = playerModel->GetModelPosition();
            ImGui::Text("World Pos: X: %.2f | Y: %.2f | Z: %.2f", pos.x, pos.y, pos.z);
        }


        ImGui::End();

        if (m_font) ImGui::PopFont();
    }
}