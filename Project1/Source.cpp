#pragma warning(disable : 26812) // Prefer 'enum class' over 'enum'
#pragma warning(disable : 26451) // Arithmetic overflow
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_impl_dx11.h"
#pragma warning(pop)
#include <windows.h>
#include <DirectXMath.h>
#include "Graphics/GraphicsEngine.h"
#include "Scene/Camera.h"
#include "Scene/GameObject.h"
#include "Graphics/TextureManager.h"
#pragma warning(push)
#pragma warning(disable : 26451) 
#pragma warning(disable : 26495) 
#pragma warning(disable : 26812) 
#pragma warning(disable : 4099)  
#pragma warning(disable : 4244)  
#pragma warning(disable : 26439) 
#include <bullet/btBulletCollisionCommon.h>
#pragma warning(pop)
#include "Graphics/Model.h"
#include <iostream>
#include "Scene/Scene.h"
#include "LogitechSteeringWheelLib.h"
#include "Input/Input.h"
#include "Physics/PhysicsEngine.h"  
#include "Physics/Handling.h"
#include "Graphics/MapLoader.h"
#include <string>
#include "SharedTypes.h"
#include "Physics/TorqueManager.h"
#include <bullet/btBulletDynamicsCommon.h>
#include "SharedVehicleTypes.h"
#include "Physics/VehicleTelemetry.h"
#include "UI/LapTimer.h"
#include "Physics/Spawner.h"
#include "Audio/FmodManager.h" 
#include "Cars/CarSetup.h"
#include "Cars/CarUpgrades.h"
#include "UI/RacingHUD.h"
#include "Cars/VehicleAsset.h"
#include "Cars/VehicleRegistry.h"
#include "Tracks/TrackTable.h"		
using namespace DirectX;

// --- GLOBALS ---
XMVECTOR g_CameraPos = XMVectorSet(0.0f, 3.0f, -7.0f, 0.0f);
XMVECTOR currentUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
TelemetryData g_DebugTelemetry = { 0 };

Spawner spawn;

FMODManager audio;
GraphicsEngine* engine = new GraphicsEngine();
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// --- WNDPROC ---
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) { PostQuitMessage(0); return 0; }
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    if (message == WM_KEYDOWN) {
        if (wParam == VK_ESCAPE) {
            if (engine->g_CurrentState == EngineState::GAMEPLAY) engine->g_CurrentState = EngineState::MAIN_MENU;
        }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// --- WINMAIN ---
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    // 1. WINDOW SETUP
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_OWNDC, WndProc, 0L, 0L, hInstance, NULL, NULL, NULL, NULL, L"DX11Window", NULL };
    RegisterClassEx(&wc);
    HWND hWnd = CreateWindow(L"DX11Window", L"911 Lab - DirectX 11", WS_OVERLAPPEDWINDOW, 0, 0, 1920, 1080, NULL, NULL, hInstance, NULL);

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    ShowWindow(hWnd, nCmdShow);
    //ShowCursor(FALSE);

    // 2. ENGINE INIT
    engine->Init(hWnd, 1920, 1080);
    audio.InitAudio();
    audio.LoadMenuSounds();
    audio.LoadJukebox();
    // 3. UI & TELEMETRY SETUP
    VehicleTelemetry telemetryUI;
    RacingHUD racingHUD;
    bool g_ShowDebugUI = false;
    LapTimer g_LapTimer;
    ImGuiIO& io = ImGui::GetIO();   
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImFont* telemetryFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 18.0f);
    io.Fonts->Build();
    telemetryUI.SetFont(telemetryFont);

    // 4. RESOURCE MANAGERS
    TextureManager* texMgr = new TextureManager(engine->GetDevice());
    Camera* camera = new Camera();
    Scene* mainScene = new Scene();
    PhysicsEngine* physics = new PhysicsEngine();
    Handling* handling = new Handling();
    MapLoader* m_mapTrack = nullptr;
    VehicleRegistry vehicleRegistry;
    // 5. VEHICLE & ASSET STATE
    bool assetsLoaded = false;
    Model* playerModel = nullptr;
    GameObject* playerObject = nullptr;

    ID3D11Buffer* cb = engine->GetConstantBuffer();
    camera->CycleCameraMode();
    engine->SetActiveCamera(camera);

    // 6. TIMING
    LARGE_INTEGER frequency, timeStart;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&timeStart);

    POINT centerPos = { 1920 / 2, 1080 / 2 };
    ClientToScreen(hWnd, &centerPos);
   

    // --- MAIN LOOP ---
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {

                LARGE_INTEGER timeCur;
                QueryPerformanceCounter(&timeCur);
                float deltaTime = (float)(timeCur.QuadPart - timeStart.QuadPart) / (float)frequency.QuadPart;
                timeStart = timeCur;

            // MOUSE & CAMERA
            if (!io.WantCaptureMouse) {
                POINT currentMousePos;
                GetCursorPos(&currentMousePos);
                float dx = (float)(currentMousePos.x - centerPos.x);
                float dy = (float)(currentMousePos.y - centerPos.y);

                if (camera->IsInManualMode()) {
                    if (dx != 0 || dy != 0) {
                        camera->AdjustRotation(dx, dy);
                        SetCursorPos(centerPos.x, centerPos.y);
                    }
                }
                while (::ShowCursor(FALSE) >= 0);
            }
            else {
                while (::ShowCursor(TRUE) < 0);
            }

            Input::Update(camera);
            engine->BeginFrame(hWnd, camera->viewMatrix, camera->projectionMatrix, deltaTime);

            // UI TOGGLE
            if (Input::IsTelemetryTogglePressed())
            {
                g_ShowDebugUI = !g_ShowDebugUI;
            }

            ImGuiIO& io = ImGui::GetIO();

            Input::GetControllerState();


            engine->SetBrakeAmount(Input::GetBrake());

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();

            Input::FeedImGuiGamepadNavigation();
            ImGui::NewFrame();


            telemetryUI.Draw(&g_ShowDebugUI, camera, playerModel);
            racingHUD.Draw(g_DebugTelemetry);

    
            // --- GAMEPLAY LOGIC ---
            if (engine->g_CurrentState == EngineState::MAIN_MENU) {
                if (assetsLoaded) {
                    mainScene->Clear();        
                    camera->SetFollowTarget(nullptr);

           

                    assetsLoaded = false;
                }

                engine->m_StartSimulationTriggered = false;
                engine->MainMenu(audio);

                static bool aWasDown = false;
                bool aDown = Input::IsPadButtonDown(XINPUT_GAMEPAD_A);

                if (aDown && !aWasDown && engine->m_StartSimulationTriggered)
                {
                    audio.PlayMenuStart();
                }                    
                else if (aDown && !aWasDown && !engine->m_StartSimulationTriggered)
                {
                    audio.PlayMenuConfirm();
                }
                aWasDown = aDown;

                static bool bWasDown = false;
                bool bDown = Input::IsPadButtonDown(XINPUT_GAMEPAD_B);

                if (bDown && !bWasDown)
                {
                    OutputDebugStringA("B CLICK\n");
                    audio.PlayMenuCancel();
                    engine->m_ShowGarage = false;
                    engine->m_ShowCarSetup = false;
                    engine->m_ShowTyresUpgrades = false;
                    engine->m_ShowEngineUpgrades = false;
                    engine->m_ShowWeightReductionUpgrades = false;
                    engine->m_TrackSelection = false;
                }

                bWasDown = bDown;

                if (Input::IsMenuDownPressed() || Input::IsMenuUpPressed())
                {
                    audio.PlayMenuClick();
                }

                audio.PlayMenuMusic();
   
            }
            else if (engine->g_CurrentState == EngineState::GAMEPLAY) {
                if (deltaTime > 0.033f) deltaTime = 0.033f;

                g_LapTimer.DrawUI();

                audio.StopMenuMusic();

                audio.UpdateJukebox();
           
                if (!assetsLoaded) {

                    physics->SetHandling(handling);
                    LogiSteeringInitialize(true);

                    m_mapTrack = new MapLoader();
                    m_mapTrack->m_texMgr = texMgr;
                    std::string trackPath = "";

                    for (const auto& track : g_TrackTable)
                    {
                        if (track.selection == GameConfig::activeTrack)
                        {
                            trackPath = track.path;
                            break;
                        }
                    }

                    handling->SetPhysicsPointers(physics->GetCarBody());
                        
                    vehicleRegistry.RegisterAllVehicles(
                        engine->GetDevice(),
                        engine->GetContext(),
                        engine->GetTextureManager());

                    VehicleAsset& vehicle =
                        vehicleRegistry.GetVehicle(engine->m_PreviewSelection);
                    playerModel = vehicle.model.get();
                    playerObject = vehicle.object.get();
                    VehicleDefinition car =
                        vehicleRegistry.CreateDefinition(engine->m_PreviewSelection);

                    CameraDefinition cam = vehicleRegistry.CreateCameraDefinition(engine->m_PreviewSelection);

                    ApplyEngineUpgrade(car, GetEngineUpgrade(engine->m_EngineUpgradeSelection));
                    ApplyWeightReductionUpgrade(car, GetWeightReductionUpgrade(engine->m_WeightReductionSelection));
                    ApplyTyresUpgrade(car, GetTyresUpgrade(engine->m_TyresUpgradeSelection));
                    ApplySetup(car, engine->m_CarSetupState);

                    audio.SetVehicleAudioDefinition(car.audio);
                    camera->SetVehicleCameraDefinition(cam);
                    racingHUD.SetVehicleDefinition(car);
                    handling->SetVehicleDefinition(car);
                    physics->SetVehicleDefinition(car);                                                  
                    physics->Initialize();
                    physics->CreatePhysicsWorld();

                    if (m_mapTrack->OpenAndLoad(trackPath, engine->GetDevice(), engine->GetContext())) {
                        physics->AddTriangleMeshCollider(m_mapTrack->GetVertices(), m_mapTrack->GetIndices());
                    }

                    mainScene->SetChaseTarget(playerModel);
                    engine->SetScene(mainScene);
                    camera->SetFollowTarget(playerModel);

                    assetsLoaded = true;
                }
                // PHYSICS & TRANSFORMS
                physics->Update(deltaTime, Input::GetCurrentInputs());
                g_LapTimer.Update(deltaTime);

                // --- TOASTY LINKER FIZYKI ZE STOPEREM ---
                if (physics->CheckAndResetPassedStartMeta()) g_LapTimer.TriggerStartMeta();
                if (physics->CheckAndResetPassedSector1())   g_LapTimer.TriggerSector1();
                if (physics->CheckAndResetPassedSector2())   g_LapTimer.TriggerSector2();
                // ----------------------------------------

                btTransform trans;
                physics->GetCarTransform(trans);
                XMMATRIX physicsWorld = physics->btTransformToXMMATRIX(trans);

                if (playerModel && playerObject) {
                    // Combine Scale -> Physics (Order Matters: Scale first!)
                    XMMATRIX finalWorld = XMMatrixTranslation(0.0f, -0.64f, 0.0f) * physicsWorld;

                    // Update the Object (The thing that Renders)
                    playerObject->SetWorldMatrix(finalWorld);

                    // Update the Model data (The thing the Camera follows)
                    playerModel->SetModelPosition(trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z());
                    playerModel->SetModelRotation(physicsWorld);
                }
                camera->Update(deltaTime); 

                // RENDER OBJECTS
                engine->RenderObject(playerObject, camera);
                if (m_mapTrack) {
                    SharedSceneData trackSD = engine->BuildSceneData(camera, playerObject, XMMatrixIdentity());
                    trackSD.view = XMMatrixTranspose(camera->viewMatrix);
                    trackSD.projection = XMMatrixTranspose(camera->projectionMatrix);
                    trackSD.world = XMMatrixTranspose(XMMatrixIdentity());
                    m_mapTrack->Draw(engine->GetContext(), cb, trackSD);     
                }
            }

            audio.Update(engine->g_CurrentState, g_DebugTelemetry.rpm, g_DebugTelemetry.throttle, g_DebugTelemetry.speed, g_DebugTelemetry.avgSlipRatio, g_DebugTelemetry.avgSlipAngle);
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            engine->EndFrame();

            // Title Bar Telemetry
            char titleBuf[256];
            sprintf_s(titleBuf, "DX11_LAB >> Speed: %f | RPM: %f | GEAR: %d | FPS: %f | Y Normal: %f | Driving Force: %f", g_DebugTelemetry.speed, g_DebugTelemetry.rpm, g_DebugTelemetry.gear, io.Framerate, g_DebugTelemetry.yNormal, g_DebugTelemetry.drivingForce);
            SetWindowTextA(hWnd, titleBuf);
        }
    }

    // --- SHUTDOWN ---
    audio.ShutdownAudio();
    LogiSteeringShutdown();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (m_mapTrack) delete m_mapTrack;
    delete physics; delete engine; delete camera; delete texMgr;

    CoUninitialize();
    return (int)msg.wParam;
}