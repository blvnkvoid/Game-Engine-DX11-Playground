// DevConsole.cpp
#include "DevConsole.h"
#include "../Graphics/GraphicsEngine.h"
#include "../Imgui/imgui.h"
#include <windows.h>
#include "../Physics/PhysicsEngine.h"

void DevConsole::Draw()
{
    if (GetAsyncKeyState(VK_OEM_3) & 0x1)
        m_open = !m_open;

    if (!m_open)
        return;

    ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Dev Console", &m_open);

    for (const auto& line : m_log)
        ImGui::TextUnformatted(line.c_str());

    ImGui::Separator();

    if (ImGui::InputText(
        "Command",
        m_input,
        sizeof(m_input),
        ImGuiInputTextFlags_EnterReturnsTrue))
    {
        SubmitCommand(m_input);
        m_input[0] = '\0';
    }

    ImGui::End();
}

void DevConsole::SubmitCommand(const char* command)
{
    m_pendingCommand = command;
    m_log.push_back(std::string("> ") + command);
}

void DevConsole::ExecuteCommand(GraphicsEngine& engine, PhysicsEngine& physics)
{
    if (m_pendingCommand.empty())
        return;

    std::string cmd = m_pendingCommand;
    m_pendingCommand.clear();

    if (cmd == "help")
    {
        m_log.push_back("Commands: help, night, time 180");
    }
    else if (cmd == "night")
    {
        m_log.push_back("Setting time to night...");
        engine.GetTime().SetTime(180.0f);
    }
    else if (cmd.rfind("time ", 0) == 0)
    {
        float value = std::stof(cmd.substr(5));
        m_log.push_back("Setting time to " + std::to_string(value));
        engine.GetTime().SetTime(value);
    }
    else if (cmd.rfind("teleport ", 0) == 0)
    {
        std::istringstream iss(cmd);

        std::string command;
        float x, y, z;

        iss >> command >> x >> y >> z;

        physics.TeleportCar(x, y, z);
       

        m_log.push_back(
            "Teleporting to " +
            std::to_string(x) + ", " +
            std::to_string(y) + ", " +
            std::to_string(z));
    }
    else
    {
        m_log.push_back("Unknown command.");
    }
}