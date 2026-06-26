#include "VehicleTelemetry.h"
#include "../Scene/Camera.h"
#include "../Graphics/Model.h"
#include "../LogitechSteeringWheelLib.h"
#include "../SharedVehicleTypes.h"


void VehicleTelemetry::Draw(bool* p_open, Camera* cam, Model* playerModel) {
    if (!*p_open) return;

    if (m_font) ImGui::PushFont(m_font);

    for (int i = 0; i < 4; i++) {
        m_displaySlipRatio[i] = m_slipRatioAvg[i].AddSample(g_DebugTelemetry.slipRatio[i]);
        m_displayWheelLoad[i] = m_wheelLoadAvg[i].AddSample(g_DebugTelemetry.wheelLoad[i]);
        m_displaywheelLoadSmoothed[i] = m_wheelLoadSmoothed[i].AddSample(g_DebugTelemetry.wheelLoadSmoothed[i]);
        m_displayDiff[i] = m_diff[i].AddSample(g_DebugTelemetry.diff[i]);
        m_displayComp[i] = m_comp[i].AddSample(g_DebugTelemetry.compression[i]);
        m_displayslipGrip[i] = m_slipGrip[i].AddSample(g_DebugTelemetry.slipGrip[i]);
        m_displayLateralForce[i] = m_lateralForceAvg[i].AddSample(g_DebugTelemetry.lateralForce[i]);
        m_displaylongitudinalForce[i] = m_longitudinalForce[i].AddSample(g_DebugTelemetry.longitudinalForce[i]);
        m_displaydiffLockTorque[i] = m_diffLockTorque[i].AddSample(g_DebugTelemetry.diffLockTorque[i]);
        m_displaydriveTorqueLeft = m_driveTorqueLeft.AddSample(g_DebugTelemetry.driveTorque[2]);
        m_displaydriveTorqueRight = m_driveTorqueRight.AddSample(g_DebugTelemetry.driveTorque[3]);
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

        if (ImGui::CollapsingHeader("Car Telemetry")) {
            if (playerModel != nullptr) {
                const char* labels[] = { "FL", "FR", "RL", "RR" };

                // ==========================================
                // 1. SUSPENSION TELEMETRY
                // ==========================================
                ImGui::TextColored(ImVec4(0.3f, 0.8f, 1.0f, 1.0f), "--- SUSPENSION OVERVIEW ---");

                // 5 kolumn: Wheel, Compression, Diff, Load, Steering Angle
                if (ImGui::BeginTable("SuspensionTable", 11, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                    ImGui::TableSetupColumn("Wheel", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                    ImGui::TableSetupColumn("Comp (%)");
                    ImGui::TableSetupColumn("Diff (M)");
                    ImGui::TableSetupColumn("Load (N)");
                    ImGui::TableSetupColumn("Smoothed Load (N)");
                    ImGui::TableSetupColumn("Steering Angle (rad)");
                    ImGui::TableSetupColumn("Wheel Torque (N)");
                    ImGui::TableSetupColumn("Damping Force (N)");
                    ImGui::TableSetupColumn("Lateral G");
                    ImGui::TableSetupColumn("Longitudinal G");
                    ImGui::TableSetupColumn("Yaw rate");

                    ImGui::TableHeadersRow();

                    for (int i = 0; i < 4; i++) {
                        ImGui::TableNextRow();

                        ImGui::TableNextColumn();
                        ImGui::Text("%s", labels[i]);

                        ImGui::TableNextColumn();
                        ImGui::Text("%.1f%%", m_displayComp[i] * 100.0f);

                        ImGui::TableNextColumn();
                        ImGui::Text("%.3f", m_displayDiff[i]);

                        ImGui::TableNextColumn();
                        ImGui::Text("%.0f", m_displayWheelLoad[i]);

                        ImGui::TableNextColumn();
                        ImGui::Text("%.0f", m_displaywheelLoadSmoothed[i]);

                        ImGui::TableNextColumn();
                        ImGui::Text("%.1f", g_DebugTelemetry.steeringAngle[i]);

                        ImGui::TableNextColumn();
                        ImGui::Text("%.0f", g_DebugTelemetry.wheelTorque[i]);

                        ImGui::TableNextColumn();
                        ImGui::Text("%.0f", g_DebugTelemetry.dampingForce[i]);

                        ImGui::TableNextColumn();
                        ImGui::Text("%.1f", g_DebugTelemetry.lateralG);

                        ImGui::TableNextColumn();
                        ImGui::Text("%.1f", g_DebugTelemetry.longitudinalG);                        
                        
                        ImGui::TableNextColumn();
                        ImGui::Text("%.1f", g_DebugTelemetry.yawRate);
                    }
                    ImGui::EndTable();
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                // ==========================================
                // 2. TYRES TELEMETRY (Tyres and Grip)
                // ==========================================
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "--- TYRES & FRICTION LAB ---");

                // 9 columns: Koło, Slip Angle, Theta, Mu, Lat Force, Max Friction, Stiffness, Vx, Vy
                if (ImGui::BeginTable("TyresTable", 21, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                    ImGui::TableSetupColumn("Wheel", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                    ImGui::TableSetupColumn("Slip Angle");
                    ImGui::TableSetupColumn("Tire Theta");
                    ImGui::TableSetupColumn("Friction Coefficient");
                    ImGui::TableSetupColumn("Lat Force (N)");
                    ImGui::TableSetupColumn("Slip Grip");
                    ImGui::TableSetupColumn("Side Stiffness");
                    ImGui::TableSetupColumn("Longitudinal Vel (m/s)");
                    ImGui::TableSetupColumn("Lateral Vel (m/s)");
                    ImGui::TableSetupColumn("Slip Ratio");
                    ImGui::TableSetupColumn("Longitudinal Force");
                    ImGui::TableSetupColumn("Local Forward Vel");
                    ImGui::TableSetupColumn("Local Side Vel");
                    ImGui::TableSetupColumn("Body Side Vel");
                    ImGui::TableSetupColumn("Body Forward Vel");
                    ImGui::TableSetupColumn("Body Slip Angle");
                    ImGui::TableSetupColumn("Camber Angle");
                    ImGui::TableSetupColumn("Camber Compression ");
                    ImGui::TableSetupColumn("Combined Grip Scale ");
                    ImGui::TableSetupColumn("Combined Max Grip ");
                    ImGui::TableSetupColumn("Combined Total Force ");
                    ImGui::TableHeadersRow();

                    for (int i = 0; i < 4; i++) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.1f", g_DebugTelemetry.slip[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.1f", g_DebugTelemetry.tyreTheta[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.1f", g_DebugTelemetry.surfaceMu[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.1f", m_displayLateralForce[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.1f", g_DebugTelemetry.slipGrip[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.tyreSideStiffness[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.1f", g_DebugTelemetry.wheelVx[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.1f", g_DebugTelemetry.wheelVy[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.3f", m_displaySlipRatio[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", m_displaylongitudinalForce[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.localForwardVel[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.localSideVel[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.bodySideVel[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.bodyForwardVel[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.bodySlipAngle[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.2f", g_DebugTelemetry.camberDeg[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.2f", g_DebugTelemetry.camberCompression[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.2f", g_DebugTelemetry.combinedGripScale[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.2f", g_DebugTelemetry.combinedMaxGrip[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.2f", g_DebugTelemetry.combinedTotalForce[i]);
                    }
                    ImGui::EndTable();
                }

                // ==========================================
                // 3. LSD / Engine / Gearbox Data 
                // ==========================================
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "--- LSD / Engine / Gearbox Data ---");
                if (ImGui::BeginTable("EngineTable", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                    ImGui::TableSetupColumn("Wheel");
                    ImGui::TableSetupColumn("Differential Lock Torque");
                    ImGui::TableSetupColumn("Diff Speed");
                    ImGui::TableSetupColumn("Target Torque");
                    ImGui::TableSetupColumn("diffLoadBiasTorque");
                    ImGui::TableSetupColumn("diffTotalTorque");
                    ImGui::TableSetupColumn("Car Weight");
                    ImGui::TableHeadersRow();
                    for (int i = 0; i < 4; i++) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.diffLockTorque[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.diffSpeed);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.diffTargetTorque);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.diffLoadBiasTorque);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.diffTotalTorque);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.mass);
                    }
                    ImGui::EndTable();
                }

                // ==========================================
                // 4. Aero Department
                // ==========================================
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "--- Aero data ---");
                if (ImGui::BeginTable("EngineTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                    ImGui::TableSetupColumn("Wheel");
                    ImGui::TableSetupColumn("Drag Magnitude");
                    ImGui::TableSetupColumn("Downforce Magnitude");
                    ImGui::TableSetupColumn("Front Downforce");
                    ImGui::TableSetupColumn("Rear Downforce");
                    ImGui::TableHeadersRow();
                    for (int i = 2; i < 4; i++) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::Text("%s", labels[i]);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.dragMagnitude);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.downforceMagnitude);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.frontDownforce);
                        ImGui::TableNextColumn(); ImGui::Text("%.0f", g_DebugTelemetry.rearDownforce);
                    }
                    ImGui::EndTable();
                }
            }
        }
    }
        if(playerModel == nullptr)
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