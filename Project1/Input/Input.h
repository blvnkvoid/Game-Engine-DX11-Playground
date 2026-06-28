#pragma once

#include <string>
#include <Windows.h>
#include <Xinput.h>

#include "../SharedVehicleTypes.h"

#pragma comment(lib, "xinput.lib")

class Camera;

class Input
{
public:
    static bool Init(HWND hWnd);
    static void Shutdown();

    static void Update(Camera* cam);
    static void GetControllerState();

    static SharedVehicleInputs GetCurrentInputs();

    static bool IsPadButtonDown(WORD button);
    static bool IsPadButtonPressed(WORD button);

    static void FeedImGuiGamepadNavigation();

    static bool IsTelemetryTogglePressed();

    static bool IsMenuDownPressed();
    static bool IsMenuUpPressed();
    static bool IsMenuBPressed();
    static float GetBrake();

private:
    static std::string GetButtonName(int id);

private:
    static SharedVehicleInputs Current;

    static bool m_prevButtonState[32];
    static int m_prevPOV;

    static WORD m_prevXInputButtons;
    static WORD m_currentXInputButtons;
};