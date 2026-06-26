#include "Input.h"

#include <cmath>
#include <iostream>

#include "LogitechSteeringWheelLib.h"
#include "../Scene/Camera.h"
#include "../Imgui/imgui.h"

SharedVehicleInputs Input::Current = { 0.0f, 0.0f, 0.0f, false };

bool Input::m_prevButtonState[32] = { false };
int Input::m_prevPOV = -1;

WORD Input::m_prevXInputButtons = 0;
WORD Input::m_currentXInputButtons = 0;

bool Input::Init(HWND hWnd)
{
    return LogiSteeringInitialize(true);
}

void Input::Shutdown()
{
    LogiSteeringShutdown();
}

SharedVehicleInputs Input::GetCurrentInputs()
{
    return Current;
}

void Input::Update(Camera* cam)
{
    if (!LogiUpdate())
        return;

    for (int i = 0; i < 25; i++)
    {
        bool isPressed = LogiButtonIsPressed(0, i);

        if (i == 4)
        {
            cam->ReverseCamera(isPressed);
        }

        if (isPressed && !m_prevButtonState[i])
        {
            if (i == 3)
            {
                cam->CycleCameraMode();
            }
        }

        m_prevButtonState[i] = isPressed;
    }

    DIJOYSTATE2* state = LogiGetState(0);

    if (!state)
        return;

    static int lastGas = 0;
    static int lastBrake = 0;
    static long lastClutch = 0;

    int currentGas = state->lY;
    int currentBrake = state->lRz;
    long clutchRaw = state->rglSlider[0];
    int currentPOV = state->rgdwPOV[0];

    if (std::abs(currentGas - lastGas) > 500)
    {
        float gasPercentage =
            (32767 - currentGas) / 65535.0f * 100.0f;

        lastGas = currentGas;
    }

    if (std::abs(currentBrake - lastBrake) > 500)
    {
        float brakePercentage =
            (32767 - currentBrake) / 65535.0f * 100.0f;

        lastBrake = currentBrake;
    }

    if (std::abs(clutchRaw - lastClutch) > 1000)
    {
        float clutchPerc =
            (32767 - clutchRaw) / 65535.0f * 100.0f;

        lastClutch = clutchRaw;
    }

    static int lastSteer = 0;

    if (std::abs(state->lX - lastSteer) > 1000)
    {
        lastSteer = state->lX;
    }

    if (currentPOV != m_prevPOV)
    {
        if (currentPOV != -1)
        {
            float degrees = currentPOV / 100.0f;
        }
    }

    m_prevPOV = currentPOV;
}

void Input::GetControllerState()
{
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    if (XInputGetState(0, &state) != ERROR_SUCCESS)
        return;

    m_prevXInputButtons = m_currentXInputButtons;
    m_currentXInputButtons = state.Gamepad.wButtons;

    float rawLeftX = static_cast<float>(state.Gamepad.sThumbLX);

    if (std::abs(rawLeftX) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
        Current.steering = 0.0f;
    else
        Current.steering = rawLeftX / 32767.0f;

    float rawRightY = static_cast<float>(state.Gamepad.sThumbRY);
    float deadzoneY = 8000.0f;

    if (rawRightY > deadzoneY)
    {
        Current.throttle =
            (rawRightY - deadzoneY) / (32767.0f - deadzoneY);

        Current.brake = 0.0f;
    }
    else if (rawRightY < -deadzoneY)
    {
        Current.brake =
            (std::abs(rawRightY) - deadzoneY) / (32768.0f - deadzoneY);

        Current.throttle = 0.0f;
    }
    else
    {
        Current.throttle = 0.0f;
        Current.brake = 0.0f;
    }

    Current.handbrake =
        (state.Gamepad.wButtons & XINPUT_GAMEPAD_B);

    Current.shiftUp =
        (state.Gamepad.bRightTrigger > 220);

    Current.shiftDown =
        (state.Gamepad.bLeftTrigger > 220);
}

bool Input::IsPadButtonDown(WORD button)
{
    return (m_currentXInputButtons & button) != 0;
}

bool Input::IsPadButtonPressed(WORD button)
{
    return
        (m_currentXInputButtons & button) &&
        !(m_prevXInputButtons & button);
}

void Input::FeedImGuiGamepadNavigation()
{
    ImGuiIO& io = ImGui::GetIO();

    io.AddKeyEvent(
        ImGuiKey_GamepadDpadUp,
        IsPadButtonDown(XINPUT_GAMEPAD_DPAD_UP));

    io.AddKeyEvent(
        ImGuiKey_GamepadDpadDown,
        IsPadButtonDown(XINPUT_GAMEPAD_DPAD_DOWN));

    io.AddKeyEvent(
        ImGuiKey_GamepadDpadLeft,
        IsPadButtonDown(XINPUT_GAMEPAD_DPAD_LEFT));

    io.AddKeyEvent(
        ImGuiKey_GamepadDpadRight,
        IsPadButtonDown(XINPUT_GAMEPAD_DPAD_RIGHT));

    io.AddKeyEvent(
        ImGuiKey_GamepadFaceDown,
        IsPadButtonDown(XINPUT_GAMEPAD_A));

    io.AddKeyEvent(
        ImGuiKey_GamepadFaceRight,
        IsPadButtonDown(XINPUT_GAMEPAD_B));

    io.AddKeyEvent(
        ImGuiKey_GamepadL1,
        IsPadButtonDown(XINPUT_GAMEPAD_LEFT_SHOULDER));

    io.AddKeyEvent(
        ImGuiKey_GamepadR1,
        IsPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER));
}

bool Input::IsTelemetryTogglePressed()
{
    bool keyboard =
        (GetAsyncKeyState(VK_F1) & 0x8000);

    bool gamepad =
        IsPadButtonDown(XINPUT_GAMEPAD_LEFT_THUMB) &&
        IsPadButtonDown(XINPUT_GAMEPAD_RIGHT_THUMB);

    static bool wasPressed = false;

    bool current = keyboard || gamepad;

    if (current && !wasPressed)
    {
        wasPressed = true;
        return true;
    }

    if (!current)
    {
        wasPressed = false;
    }

    return false;
}

bool Input::IsMenuDownPressed()
{

    static bool wasDown = false;


    bool down = IsPadButtonDown(XINPUT_GAMEPAD_DPAD_DOWN);
    bool pressed = down && !wasDown;

    wasDown = down;

    return pressed;
}


bool Input::IsMenuUpPressed()
{
    static bool wasUp = false;

    bool up = IsPadButtonDown(XINPUT_GAMEPAD_DPAD_UP);
    bool pressed = up && !wasUp;

    wasUp = up;

    return pressed;
}

std::string Input::GetButtonName(int id)
{
    switch (id)
    {
    case 0: return "Cross (X)";
    case 1: return "Square";
    case 2: return "Circle";
    case 3: return "Triangle";
    case 4: return "Right Paddle (Upshift)";
    case 5: return "Left Paddle (Downshift)";
    case 6: return "R2 Button";
    case 7: return "L2 Button";
    case 8: return "Share";
    case 9: return "Options";
    case 10: return "R3";
    case 11: return "L3";
    case 12: return "Gear 1";
    case 13: return "Gear 2";
    case 14: return "Gear 3";
    case 15: return "Gear 4";
    case 16: return "Gear 5";
    case 17: return "Gear 6";
    case 18: return "Reverse Gear (Shifter)";
    case 19: return "Plus (+)";
    case 20: return "Minus (-)";
    case 21: return "Dial Right";
    case 22: return "Dial Left";
    case 23: return "Return (Dial Center)";
    case 24: return "PS Button";
    default:
        return "Button " + std::to_string(id);
    }
}