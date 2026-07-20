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
#include "Audio/FmodManager.h" 
#include "Cars/CarSetup.h"
#include "Cars/CarUpgrades.h"
#include "UI/RacingHUD.h"
#include "Cars/VehicleAsset.h"
#include "Cars/VehicleRegistry.h"
#include "Tracks/TrackTable.h"		
#include "UI/MainMenu.h"
#include "Environment/EnvironmentDefinition.h"
#include "UI/TrackMenu.h"
#include "UI/Garage.h"
#include "UI/CarSetupMenu.h"
#include "DevConsole/DevConsole.h"
#include "Events/EventCarEntry.h"
#include "Events/EventDefinition.h"
#include "Events/EventRegistry.h"
#include "Events/ChampionshipDefinition.h"
#include "Events/EventSession.h"
#include "Events/RaceGrid.h"
#include "UI/Settings.h"
#include "UI/UIContext.h"
#include "Sky/SkyEngine.h"
#include "Sky/Sun.h"

using namespace DirectX;

XMVECTOR g_CameraPos = XMVectorSet(0.0f, 3.0f, -7.0f, 0.0f);
XMVECTOR currentUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
TelemetryData g_DebugTelemetry = { 0 };
FMODManager audio;
GraphicsEngine* engine = new GraphicsEngine();
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
MainMenu menu;
TrackMenu trackmenu;
Garage& garage = menu.GetGarage();
Upgrades& upgrades = menu.GetUpgrades();
CarSetupMenu& carsetup = menu.GetCarSetup();
DevConsole devConsole;
EventRegistry eventRegistry;
EventSession eventSession;
RaceGrid raceGrid;
Settings settings;
SkyEngine skyEngine;
Sun sun;

const TrackEntry* activeTrackEntry = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) { PostQuitMessage(0); return 0; }
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    if (message == WM_KEYDOWN) {
        if (wParam == VK_ESCAPE) {
            if (menu.g_CurrentState == EngineState::GAMEPLAY) menu.g_CurrentState = EngineState::MAIN_MENU;
        }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_OWNDC, WndProc, 0L, 0L, hInstance, NULL, NULL, NULL, NULL, L"DX11Window", NULL };
    RegisterClassEx(&wc);
    HWND hWnd = CreateWindow(L"DX11Window", L"911 Lab - DirectX 11", WS_OVERLAPPEDWINDOW, 0, 0, settings.width, settings.height, NULL, NULL, hInstance, NULL);

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    ShowWindow(hWnd, nCmdShow);

    engine->Init(hWnd, settings.width, settings.height);
    skyEngine.Initialize(engine->GetDevice());
    audio.InitAudio();
    audio.LoadMenuSounds();
    audio.LoadJukebox();
    VehicleTelemetry telemetryUI;
    RacingHUD racingHUD;
    bool g_ShowDebugUI = false;
    LapTimer g_LapTimer;

    engine->ConfigureUIScale(1920.0f, 1080.0f);

    ImGuiIO& io = ImGui::GetIO();   
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;




    TextureManager* texMgr = new TextureManager(engine->GetDevice());
    Camera* camera = new Camera();
    Scene* mainScene = new Scene();
    PhysicsEngine* physics = new PhysicsEngine();
    Handling* handling = new Handling();
    MapLoader* m_mapTrack = nullptr;
    VehicleRegistry vehicleRegistry;
    bool assetsLoaded = false;
    Model* playerModel = nullptr;
    GameObject* playerObject = nullptr;         
    std::vector<GameObject*> aiObjects;

    ID3D11Buffer* cb = engine->GetConstantBuffer();
    camera->CycleCameraMode();
    engine->SetActiveCamera(camera);

    LARGE_INTEGER frequency, timeStart;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&timeStart);

    POINT centerPos = { 1920 / 2, 1080 / 2 };
    ClientToScreen(hWnd, &centerPos);
   

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
            engine->BeginFrame(hWnd, camera->viewMatrix, camera->projectionMatrix, deltaTime, camera);  

            if (Input::IsTelemetryTogglePressed())
            {
                g_ShowDebugUI = !g_ShowDebugUI;
            }

            ImGuiIO& io = ImGui::GetIO();
            Input::GetControllerState();
            engine->SetBrakeAmount(Input::GetBrake());

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();


            io.DisplayFramebufferScale = ImVec2(
                settings.width / io.DisplaySize.x,
                settings.height / io.DisplaySize.y);

            Input::FeedImGuiGamepadNavigation();
            ImGui::NewFrame();


            racingHUD.Draw(g_DebugTelemetry);


            if (menu.g_CurrentState == EngineState::MAIN_MENU) {
                if (assetsLoaded) {
                    mainScene->Clear();        
                    camera->SetFollowTarget(nullptr);       
                    delete m_mapTrack;
                    m_mapTrack = nullptr;
                    assetsLoaded = false;
                }
   
                menu.m_StartSimulationTriggered = false;
                menu.Draw(*engine, audio, engine->GetUIContext());
                menu.HandleInput(audio);
            }
            else if (menu.g_CurrentState == EngineState::GAMEPLAY) {
                if (deltaTime > 0.033f) deltaTime = 0.033f;

                g_LapTimer.DrawUI();
                audio.StopMenuMusic();
                audio.UpdateJukebox();

                
           
                if (!assetsLoaded) {

                    physics->SetHandling(handling);
                    LogiSteeringInitialize(true);

                    if (!eventSession.IsActive())
                    {
                        eventSession = eventRegistry.CreateSession(menu.m_eventmenu.m_LaunchType, menu.m_eventmenu.m_SelectedEvent);
                    }

                    EventDefinition event =
                        eventRegistry.Create(eventSession.GetCurrentEvent());

                    m_mapTrack = new MapLoader();
                    m_mapTrack->m_texMgr = texMgr;
                    std::string trackPath = "";
                    

                    

                    for (const auto& track : g_TrackTable)
                    {
                        if (track.selection == event.track)
                        {
                            trackPath = track.path;
                            activeTrackEntry = &track;
                            break;
                        }
                    }    

               

                    GameConfig::activeTrack = event.track;
                    engine->ApplyEnvironmentDefinition(event.environment);
                    handling->SetPhysicsPointers(physics->GetCarBody());



                    VehicleSelection playerSelection =
                        raceGrid.GetPlayerSelection(event, garage.m_PreviewSelection);



                    VehicleDefinition car =
                        vehicleRegistry.CreateDefinition(playerSelection);

                    physics->SetVehicleDefinition(car);
                    physics->Initialize();



                    if (m_mapTrack->OpenAndLoad(trackPath, engine->GetDevice(), engine->GetContext())) {
                        physics->AddTriangleMeshCollider(m_mapTrack->GetVertices(), m_mapTrack->GetIndices());
                    }


                    const auto& markers = m_mapTrack->GetMarkers();

                    TrackTimingEntry timing =
                        CreateTrackTiming(event.track, markers);

                    physics->SetTrackTiming(timing);

           
                    raceGrid.Build(
                        event,
                        markers,
                        vehicleRegistry,
                        *physics,
                        engine->GetDevice(),
                        engine->GetContext(),
                        engine->GetTextureManager(),
                        garage.m_PreviewSelection);

                        physics->SetStartTransform(
                            raceGrid.GetPlayerSpawn());


                        physics->CreatePhysicsWorld();


                    vehicleRegistry.GetOrLoadVehicle(playerSelection, engine->GetDevice(), engine->GetContext(), engine->GetTextureManager());
                    VehicleAsset& vehicle = vehicleRegistry.GetVehicle(playerSelection);
                    playerModel = vehicle.model.get();
                    playerObject = vehicle.object.get();

                    CameraDefinition cam = vehicleRegistry.CreateCameraDefinition(playerSelection);

                    ApplyEngineUpgrade(car, GetEngineUpgrade(upgrades.m_EngineUpgradeSelection));
                    ApplyWeightReductionUpgrade(car, GetWeightReductionUpgrade(upgrades.m_WeightReductionSelection));
                    ApplyTyresUpgrade(car, GetTyresUpgrade(upgrades.m_TyresUpgradeSelection));
                    ApplySetup(car, carsetup.m_CarSetupState);

                    audio.SetVehicleAudioDefinition(car.audio);
                    camera->SetVehicleCameraDefinition(cam);
                    racingHUD.SetVehicleDefinition(car);
                    handling->SetVehicleDefinition(car);
          

                    mainScene->SetChaseTarget(playerModel);
                    engine->SetScene(mainScene);
                    camera->SetFollowTarget(playerModel);

                 

                    g_LapTimer.Reset();
                    g_LapTimer.SetTotalLaps(event.totalLaps);      

                    assetsLoaded = true;
                }



                telemetryUI.Draw(&g_ShowDebugUI, camera, playerModel, m_mapTrack);
                physics->Update(deltaTime, Input::GetCurrentInputs());
                g_LapTimer.Update(deltaTime);


                if (physics->CheckAndResetPassedStartMeta()) g_LapTimer.TriggerStartMeta();
                if (physics->CheckAndResetPassedSector1())   g_LapTimer.TriggerSector1();
                if (physics->CheckAndResetPassedSector2())   g_LapTimer.TriggerSector2();


                if (g_LapTimer.HasFinishedRace())
                {
                    if (eventSession.Advance())
                        assetsLoaded = false;
                    else
                        menu.g_CurrentState = EngineState::MAIN_MENU;
                }

                btTransform trans;
                physics->GetCarTransform(trans);
                XMMATRIX physicsWorld = physics->btTransformToXMMATRIX(trans);

               if (playerModel && playerObject) {
                    XMMATRIX finalWorld = XMMatrixTranslation(0.0f, -0.64f, 0.0f) * physicsWorld;
                    playerObject->SetWorldMatrix(finalWorld);
                    playerModel->SetModelPosition(trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z());
                    playerModel->SetModelRotation(physicsWorld);
                }
                camera->Update(deltaTime, *activeTrackEntry);

                devConsole.Draw();
                devConsole.ExecuteCommand(*engine, *physics);
        
                engine->RenderObject(playerObject, camera);


                

                raceGrid.Render(*engine, *camera);
                if (m_mapTrack)
                {
                    SharedSceneData trackSD =
                        engine->BuildSceneData(
                            camera,
                            playerObject,
                            XMMatrixIdentity()
                        );

                    trackSD.view =
                        XMMatrixTranspose(camera->viewMatrix);

                    trackSD.projection =
                        XMMatrixTranspose(camera->projectionMatrix);

                    trackSD.world =
                        XMMatrixTranspose(XMMatrixIdentity());

                    m_mapTrack->UpdateVisibleLights(
                        engine->GetContext(),
                        engine->GetLampStructuredBuffer(),
                        engine->GetLampConstantBuffer(),
                        camera->GetFrustum(),
                        camera
                        );

                    m_mapTrack->Draw(
                        engine->GetContext(),
                        cb,
                        engine->GetLampConstantBuffer(),                 
                        engine->GetLampResourceView(),
                        trackSD,
                        camera->GetFrustum(),
                        engine->GetDepthStencilState()
                    );
                }

                engine->GetSun().Render(engine->GetContext(), camera, skyEngine);
            }
                
            

            audio.Update(menu.g_CurrentState, g_DebugTelemetry.rpm, g_DebugTelemetry.throttle, g_DebugTelemetry.speed, g_DebugTelemetry.avgSlipRatio, g_DebugTelemetry.avgSlipAngle);
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            engine->EndFrame();
        }
    }

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