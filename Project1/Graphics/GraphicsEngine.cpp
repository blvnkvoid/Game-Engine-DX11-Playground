#include "GraphicsEngine.h"
#include <d3dcompiler.h>
#include "../Scene/Camera.h"
#include "TextureManager.h"
#include "../SharedTypes.h"
#include "../Scene/Scene.h"
#include "../Cars/CarSetup.h"
#include "../Audio/FmodManager.h"
#pragma warning(push)
#pragma warning(disable : 26451) // Arithmetic overflow (ImGui does a lot of 32/64 bit math)
#pragma warning(disable : 26812) // Prefer 'enum class' over 'enum'
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_win32.h"
#include "../Imgui/imgui_impl_dx11.h"
#pragma warning(pop)
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

GraphicsEngine::GraphicsEngine()
{

}

bool GraphicsEngine::Init(HWND hWnd, int width, int height)
{
    HRESULT hr;



    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 8;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = true;


    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 8;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    D3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.FillMode = D3D11_FILL_SOLID; // Or D3D11_FILL_WIREFRAME for a cool matrix look!
    rsDesc.CullMode = D3D11_CULL_NONE;  // <--- THE CULL KILLER
    rsDesc.AntialiasedLineEnable = true;
    rsDesc.MultisampleEnable = true;
    rsDesc.FrontCounterClockwise = false;


    D3D11_RASTERIZER_DESC rsDesc2 = {};
    rsDesc2.FillMode = D3D11_FILL_WIREFRAME; // Or D3D11_FILL_WIREFRAME for a cool matrix look!
    rsDesc2.CullMode = D3D11_CULL_NONE;  // <--- THE CULL KIL6LER
    rsDesc2.AntialiasedLineEnable = true;
    rsDesc2.MultisampleEnable = true;
    rsDesc2.FrontCounterClockwise = false;

    D3D11_BUFFER_DESC bd = {};
    D3D11_SUBRESOURCE_DATA init = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // Specifically allows Map()
    bd.ByteWidth = sizeof(SharedSceneData);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;


    D3D11_BUFFER_DESC matDesc = {};
    matDesc.Usage = D3D11_USAGE_DYNAMIC;
    matDesc.ByteWidth = sizeof(SharedMaterial); // Should be 48
    matDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


    D3D11_INPUT_ELEMENT_DESC layout[] ={
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // Changed R32G32 to R32G32B32
};

    D3D_FEATURE_LEVEL featureLevels[] = {
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    };


    D3D11_SAMPLER_DESC sampDesc = {};    
    sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    sampDesc.MaxAnisotropy = 16;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // This enables tiling/repeating
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    sampDesc.MipLODBias = 0.0f;
    
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;




    hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels, 2,
        D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context);
    if (FAILED(hr)) return false;

    hr = D3DCompileFromFile(L"Shaders.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, nullptr);
    if (FAILED(hr)) return false;
    hr = D3DCompileFromFile(L"Shaders.hlsl", nullptr, nullptr, "PS", "ps_5_0", 0, 0, &psBlob, nullptr);
    if (FAILED(hr)) return false;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr)) return false;
    hr = device->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &renderTargetView);
    if (FAILED(hr)) return false;
    hr = device->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);
    if (FAILED(hr)) return false;
    hr = device->CreateDepthStencilView(pDepthStencil.Get(), nullptr, &depthStencilView);
    if (FAILED(hr)) return false; 
    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr)) return false;
    hr = device->CreateInputLayout(layout, 4, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    if (FAILED(hr)) return false;
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    if (FAILED(hr)) return false;
    hr = device->CreateBuffer(&bd, nullptr, &constantBuffer);
    if (FAILED(hr)) return false;
    hr = device->CreateRasterizerState(&rsDesc, &rasterState);
    if (FAILED(hr)) return false;
    hr = device->CreateRasterizerState(&rsDesc2, &rasterStateWireframe);
    if (FAILED(hr)) return false;
    hr = device->CreateSamplerState(&sampDesc, &m_samplerLinear);
    if (FAILED(hr)) return false;
    hr = device->CreateBlendState(&blendDesc, &m_alphaBlendState);
    if (FAILED(hr)) return false;
    hr = device->CreateBuffer(&matDesc, nullptr, &materialConstantBuffer);
    if (FAILED(hr)) return false;
  
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Header] = ImVec4(0.8f, 0.0f, 0.0f, 1.0f);        // Red headers
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Bright red on hover
    style.WindowRounding = 5.0f;                                          // Sleek rounded corners
    style.FrameRounding = 3.0f;

    m_textureManager = std::make_unique<TextureManager>(device.Get());

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd); // Pass your window handle
    ImGui_ImplDX11_Init(device.Get(), context.Get());

    return true;

}

void GraphicsEngine::MainMenu(FMODManager& audio)
{


    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::SetNextWindowPos(ImVec2(50, 100));
    ImGui::Begin("MAIN_OPTIONS", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
   
    if (ImGui::Button("START SIMULATION", ImVec2(300, 50))) {

        m_StartSimulationTriggered = true;
        this->g_CurrentState = EngineState::GAMEPLAY;
    }


    if (ImGui::Button("GARAGE / SELECT CAR", ImVec2(300, 50))) {
         
        m_ShowGarage = !m_ShowGarage;
    }

    if (ImGui::Button("Engine Uprades Menu", ImVec2(300, 50))) {
         
        m_ShowEngineUpgrades = !m_ShowEngineUpgrades;
    }
        
    
    if (ImGui::Button("Weight Reduction Menu", ImVec2(300, 50))) {
         
        m_ShowWeightReductionUpgrades = !m_ShowWeightReductionUpgrades;
    }  
    
    if (ImGui::Button("Tyres Upgrades Menu", ImVec2(300, 50))) {
         
        m_ShowTyresUpgrades = !m_ShowTyresUpgrades;
    }  
    
    if (ImGui::Button("Car Setup", ImVec2(300, 50))) {
         
        m_ShowCarSetup = !m_ShowCarSetup;
    }

    if (ImGui::Button("TRACK SELECTION", ImVec2(300, 50))) {
         
        m_TrackSelection = !m_TrackSelection;
    }

    if (ImGui::Button("EXIT", ImVec2(300, 50))) {
        PostQuitMessage(0);
    }
    ImGui::End();

    // 3. The persistent Garage List
    if (m_ShowGarage) {
        ImGui::SetNextWindowPos(ImVec2(400, 100)); // Place it next to the main menu
        ImGui::Begin("Car List", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::Selectable("PORSCHE 911 GT3", m_PreviewSelection == VehicleSelection::PORSCHE_911)) {
             
            m_PreviewSelection = VehicleSelection::PORSCHE_911;
        }
        if (ImGui::Selectable("AUDI R8 COUPE", m_PreviewSelection == VehicleSelection::AUDI_R8)) {
             
            m_PreviewSelection = VehicleSelection::AUDI_R8;
        }
        if (ImGui::Selectable("BUGATTI CHIRON", m_PreviewSelection == VehicleSelection::BUGATTI_CHIRON)) {
             
            m_PreviewSelection = VehicleSelection::BUGATTI_CHIRON;
        }  
        
        if (ImGui::Selectable("HONDA CIVIC", m_PreviewSelection == VehicleSelection::CIVIC)) {
             
            m_PreviewSelection = VehicleSelection::CIVIC;
        }    
        
        if (ImGui::Selectable("TOYOTA MINOLTA", m_PreviewSelection == VehicleSelection::MINOLTA)) {
             
            m_PreviewSelection = VehicleSelection::MINOLTA;
        }       
        
        if (ImGui::Selectable("Honda NSX GT-500", m_PreviewSelection == VehicleSelection::GT500)) {
             
            m_PreviewSelection = VehicleSelection::GT500;
        }      
        
        if (ImGui::Selectable("Mazda Furai", m_PreviewSelection == VehicleSelection::FURAI)) {
             
            m_PreviewSelection = VehicleSelection::FURAI;
        }      
        
        if (ImGui::Selectable("Mazda MX5", m_PreviewSelection == VehicleSelection::MX5)) {
             
            m_PreviewSelection = VehicleSelection::MX5;
        }  
        
        if (ImGui::Selectable("Audi R10", m_PreviewSelection == VehicleSelection::AUDI_R10)) {
             
            m_PreviewSelection = VehicleSelection::AUDI_R10;
        }    
        
        if (ImGui::Selectable("Citroen Xsara KitCar'98", m_PreviewSelection == VehicleSelection::XSARA)) {
             
            m_PreviewSelection = VehicleSelection::XSARA;
        }   
        
        if (ImGui::Selectable("Daihatsu Copen", m_PreviewSelection == VehicleSelection::COPEN)) {
             
            m_PreviewSelection = VehicleSelection::COPEN;
        }      
        
        if (ImGui::Selectable("Toyota Supra JGTC 2000", m_PreviewSelection == VehicleSelection::JGTCSUPRA2000)) {
             
            m_PreviewSelection = VehicleSelection::JGTCSUPRA2000;
        }    
        
        if (ImGui::Selectable("Honda NSX Raybrig JGTC 2000", m_PreviewSelection == VehicleSelection::JGTCNSX2000)) {
             
            m_PreviewSelection = VehicleSelection::JGTCNSX2000;
        }

        ImGui::End();
    }

    // 4. The Upgrade Menu
        if (m_ShowEngineUpgrades) {
            ImGui::SetNextWindowPos(ImVec2(400, 100)); // Place it next to the main menu
            ImGui::SetNextWindowSize(ImVec2(180, 0), ImGuiCond_Always);
            ImGui::Begin("Engine Upgrades", nullptr,0);

            if (ImGui::Selectable("Stock Engine", m_EngineUpgradeSelection == EngineUpgradeSelection::StockEngine)) {
                 
                m_EngineUpgradeSelection = EngineUpgradeSelection::StockEngine;
            }
            if (ImGui::Selectable("NA Tune", m_EngineUpgradeSelection == EngineUpgradeSelection::NATune)) {
                 
                m_EngineUpgradeSelection = EngineUpgradeSelection::NATune;
            }
            if (ImGui::Selectable("Turbo", m_EngineUpgradeSelection == EngineUpgradeSelection::Turbo)) {
                 
                m_EngineUpgradeSelection = EngineUpgradeSelection::Turbo;
            }   
           

            ImGui::End();
        }        
        
        if (m_ShowWeightReductionUpgrades) {
            ImGui::SetNextWindowPos(ImVec2(400, 100)); // Place it next to the main menu
            ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);
            ImGui::Begin("Weight Reduction Upgrades", nullptr,0);

            if (ImGui::Selectable("Stock Weight", m_WeightReductionSelection == WeightReductionSelection::StockWeight)) {
                 
                m_WeightReductionSelection = WeightReductionSelection::StockWeight;
            }
            if (ImGui::Selectable("Weight Reduction Stage 1", m_WeightReductionSelection == WeightReductionSelection::WeightReductionStage1)) {
                   
                m_WeightReductionSelection = WeightReductionSelection::WeightReductionStage1;
            }
            if (ImGui::Selectable("Weight Reduction Stage 2", m_WeightReductionSelection == WeightReductionSelection::WeightReductionStage2)) {
                 
                m_WeightReductionSelection = WeightReductionSelection::WeightReductionStage2;
            }      
            
            if (ImGui::Selectable("Weight Reduction Stage 3", m_WeightReductionSelection == WeightReductionSelection::WeightReductionStage3)) {
                 
                m_WeightReductionSelection = WeightReductionSelection::WeightReductionStage3;
            }            

            ImGui::End();
        }   
        
        if (m_ShowTyresUpgrades) {
            ImGui::SetNextWindowPos(ImVec2(400, 100)); // Place it next to the main menu
            ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);
            ImGui::Begin("Tyres Upgrades", nullptr,0);

            if (ImGui::Selectable("Stock Tyres", m_TyresUpgradeSelection == TyresUpgradeSelection::StockTyres)) {
                 
                m_TyresUpgradeSelection = TyresUpgradeSelection::StockTyres;
            }
            if (ImGui::Selectable("Sports Tyres", m_TyresUpgradeSelection == TyresUpgradeSelection::SportTyres)) {
                 
                m_TyresUpgradeSelection = TyresUpgradeSelection::SportTyres;
            }
            if (ImGui::Selectable("Semi Slicks", m_TyresUpgradeSelection == TyresUpgradeSelection::SemiSlicks)) {
                 
                m_TyresUpgradeSelection = TyresUpgradeSelection::SemiSlicks;
            }      
            
            if (ImGui::Selectable("Racing Tyres", m_TyresUpgradeSelection == TyresUpgradeSelection::RacingTyres)) {
                 
                m_TyresUpgradeSelection = TyresUpgradeSelection::RacingTyres;
            }            

            ImGui::End();
        }   

        if (m_ShowCarSetup)
        {
            ImGui::SetNextWindowPos(ImVec2(400, 100));
            ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);

            ImGui::Begin("Car Setup", nullptr, 0);

            ImGui::SliderFloat(
                "Final Drive",
                &m_CarSetupState.finalDriveMultiplier,
                0.80f,
                1.20f);

            ImGui::SliderFloat(
                "Front ARB",
                &m_CarSetupState.frontARBMultiplier,
                0.50f,
                2.00f);

            ImGui::SliderFloat(
                "Rear ARB",
                &m_CarSetupState.rearARBMultiplier,
                0.50f,
                2.00f);

            ImGui::SliderFloat(
                "Front Springs",
                &m_CarSetupState.frontSpringMultiplier,
                0.50f,
                2.00f);

            ImGui::SliderFloat(
                "Rear Springs",
                &m_CarSetupState.rearSpringMultiplier,
                0.50f,
                2.00f);

            ImGui::SliderFloat(
                "Front Downforce",
                &m_CarSetupState.frontDownforceMultiplier,
                0.50f,
                2.00f);

            ImGui::SliderFloat(
                "Rear Downforce",
                &m_CarSetupState.rearDownforceMultiplier,
                0.50f,
                2.00f);

            ImGui::SliderFloat(
                "Brake Bias Front",
                &m_CarSetupState.brakeBiasFront,
                0.45f,
                0.75f);

            if (ImGui::Button("Reset"))
            {
                m_CarSetupState = CarSetupState{};
            }

            ImGui::End();
        }
        
     

    


    if (m_TrackSelection)
    {
        ImGui::SetNextWindowPos(ImVec2(400, 100)); // Place it next to the main menu
        ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);
        ImGui::Begin("Track List", nullptr, 0);
        if (ImGui::Selectable("AutumnRing", GameConfig::activeTrack == TrackSelection::AutumnRing)) {
             
            GameConfig::activeTrack = TrackSelection::AutumnRing;
        } 
        if (ImGui::Selectable("ElCapitan", GameConfig::activeTrack == TrackSelection::ElCapitan)) {
             
            GameConfig::activeTrack = TrackSelection::ElCapitan;
        }
        if (ImGui::Selectable("Spa Francorshamps ", GameConfig::activeTrack == TrackSelection::Spa)) {
             
            GameConfig::activeTrack = TrackSelection::Spa;
        }
        if (ImGui::Selectable("Grand Valley Speedway ", GameConfig::activeTrack == TrackSelection::GrandValley)) {
             
            GameConfig::activeTrack = TrackSelection::GrandValley;
        }    
        if (ImGui::Selectable("Trial Mountain ", GameConfig::activeTrack == TrackSelection::TrialMountain)) {
             
            GameConfig::activeTrack = TrackSelection::TrialMountain;
        }  
        if (ImGui::Selectable("High Speed Ring ", GameConfig::activeTrack == TrackSelection::HighSpeedRing)) {
             
            GameConfig::activeTrack = TrackSelection::HighSpeedRing;
        }  
        if (ImGui::Selectable("Midfield Raceway ", GameConfig::activeTrack == TrackSelection::MidfieldRaceway)) {
             
            GameConfig::activeTrack = TrackSelection::MidfieldRaceway;
        }
        if (ImGui::Selectable("Test Course ", GameConfig::activeTrack == TrackSelection::TestCourse)) {
             
            GameConfig::activeTrack = TrackSelection::TestCourse;
        }       
        if (ImGui::Selectable("Route X ", GameConfig::activeTrack == TrackSelection::RouteX)) {
             
            GameConfig::activeTrack = TrackSelection::RouteX;
        }  
        if (ImGui::Selectable("Road Atlanta ", GameConfig::activeTrack == TrackSelection::RoadAtlanta)) {
             
            GameConfig::activeTrack = TrackSelection::RoadAtlanta;
        }   
        if (ImGui::Selectable("Nurburgring Nordschleife ", GameConfig::activeTrack == TrackSelection::Nordschleife)) {
             
            GameConfig::activeTrack = TrackSelection::Nordschleife;
        }   
        
        if (ImGui::Selectable("Begginer Course ", GameConfig::activeTrack == TrackSelection::BeginnerCourse)) {
             
            GameConfig::activeTrack = TrackSelection::BeginnerCourse;
        }        
        if (ImGui::Selectable("Motorland ", GameConfig::activeTrack == TrackSelection::Motorland)) {
             
            GameConfig::activeTrack = TrackSelection::Motorland;
        }  
        
        if (ImGui::Selectable("Tsukuba ", GameConfig::activeTrack == TrackSelection::Tsukuba)) {
             
            GameConfig::activeTrack = TrackSelection::Tsukuba;
        }        
        
        if (ImGui::Selectable("Le Mans ", GameConfig::activeTrack == TrackSelection::LeMans)) {
             
            GameConfig::activeTrack = TrackSelection::LeMans;
        }        
        
        if (ImGui::Selectable("Deep Forest Raceway", GameConfig::activeTrack == TrackSelection::DeepForest)) {
             
            GameConfig::activeTrack = TrackSelection::DeepForest;
        }

        ImGui::End();
    }

    


    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void GraphicsEngine::BeginFrame(HWND hWnd,DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
{   

    bool gIsDown = GetAsyncKeyState('G') & 0x8000;

    if (gIsDown && !m_gWasPressed) {
        m_isWireframe = !m_isWireframe; // Flip the switch
    }
    m_gWasPressed = GetAsyncKeyState('G') & 0x8000;

    m_sceneData.view = XMMatrixTranspose(view);
    m_sceneData.projection = XMMatrixTranspose(projection);

    float clearColor[] = { 0.65f, 0.75f, 1.0f, 1.0f };
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    context->OMSetBlendState(m_alphaBlendState.Get(), blendFactor, 0xffffffff);

    context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    context->ClearRenderTargetView(renderTargetView.Get(), clearColor);
    context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    if (m_isWireframe) {
        context->RSSetState(rasterStateWireframe.Get());
    }
    else {
        context->RSSetState(rasterState.Get());
    }

    RECT rc;
    GetClientRect(hWnd, &rc);
    float width = (float)(rc.right - rc.left);  
    float height = (float)(rc.bottom - rc.top);

    D3D11_VIEWPORT vp = { 0.0f, 0.0f, width, height, 0.0f, 1.0f };
    context->RSSetViewports(1, &vp);
    context->IASetInputLayout(inputLayout.Get());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(vertexShader.Get(), nullptr, 0);
    context->PSSetShader(pixelShader.Get(), nullptr, 0);
}




void GraphicsEngine::RenderObject(GameObject* obj, Camera* cam) {
    SharedSceneData cb; // [cite: 2026-01-03]

    // 1. SET GLOBAL STATE (Same for everything)
    cb.lightDirection = this->m_lightDir;
    cb.lightColor = this->m_lightColor;
    DirectX::XMFLOAT3 pos = cam->GetPosition();
    cb.cameraPosition.x = pos.x;
    cb.cameraPosition.y = pos.y;
    cb.cameraPosition.z = pos.z;
    

    XMVECTOR forward = cam->GetForwardVector();
    XMStoreFloat4(&cb.cameraDirection, forward);
    cb.cameraDirection.w = 0.0f;

    // 2. BIND TEXTURES (Same for everything)
    ID3D11ShaderResourceView* objectTextureRV = obj->GetTexture();
    context->PSSetShaderResources(0, 1, &objectTextureRV);
    context->PSSetSamplers(0, 1, m_samplerLinear.GetAddressOf());



    Model* model = obj->GetModel();

    if (model) {


        OutputDebugStringA("MODEL BRANCH\n");

        // --- MODEL BRANCH ---
        // Let the model handle its own specialized plumbing internally
        XMMATRIX world = obj->GetWorldMatrix();
        XMMATRIX view = cam->GetViewMatrix();
        XMMATRIX projection = cam->GetProjectionMatrix();

        XMFLOAT4X4 m;
        XMStoreFloat4x4(&m, world);

        float modelX = m._41; // X is row 4, column 1
        float modelY = m._42; // Y is row 4, column 2
        float modelZ = m._43; // Z is row 4, column 3

        // 3. THE RAW CONSOLE LOG
        // We'll use a frame-counter to prevent flooding the console
        model->GetModelWorldMatrix();
        model->GetModelPosition();

        model->BindAndDraw(context.Get(), sizeof(SharedVertex), world,  view, projection, cam);
    }
    else {
        OutputDebugStringA("FLOOR BRANCH\n");
        // --- FLOOR BRANCH ---
        // 1. Get Material and populate the SHARED struct [cite: 2026-01-03]
        SharedSceneData floorSceneData = {}; // Use a fresh struct to avoid garbage [cite: 2026-01-03]
        floorSceneData.material = obj->GetMaterial();
        
        floorSceneData.material.diffuseColor = XMFLOAT3(1, 1, 1);
        floorSceneData.material.ambientColor = XMFLOAT3(0.5f, 0.5f, 0.5f);
        floorSceneData.material.specularColor = XMFLOAT3(0.2f, 0.2f, 0.2f);
        floorSceneData.material.specularPower = 32.0f;
        floorSceneData.material.d = 1.0f;

        // 2. Transpose ALL matrices (Models do this, so the Floor must too!) [cite: 2026-01-03]
        floorSceneData.world = XMMatrixTranspose(obj->GetWorldMatrix());
        floorSceneData.view = XMMatrixTranspose(cam->GetViewMatrix());
        floorSceneData.projection = XMMatrixTranspose(cam->GetProjectionMatrix());

        // 3. Set Global Lighting [cite: 2026-01-03]
        floorSceneData.lightDirection = XMFLOAT4(0.5f, -1.0f, 0.5f, 0.0f);
        floorSceneData.lightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

        // 4. Set Camera Position (Crucial for the Jump math!) [cite: 2026-01-03]
        DirectX::XMFLOAT3 pos = cam->GetPosition();
        XMVECTOR posVec = XMLoadFloat3(&pos);
        XMStoreFloat4(&floorSceneData.cameraPosition, posVec);
        XMStoreFloat4(&floorSceneData.cameraDirection, cam->GetForwardVector());

        // 5. Force the Update and Re-Bind to Slot 0 [cite: 2026-01-03]
        // This 'steals' the slot back from whatever the Model bound last!
        context->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &floorSceneData, 0, 0);
        context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
        context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());


        // 6. Bind Floor-specific Geometry [cite: 2026-01-03]
        UINT stride = sizeof(SharedVertex);
        UINT offset = 0;
        auto vb = obj->GetVertexBuffer();
        context->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(obj->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 7. FINAL DRAW
        context->DrawIndexed(obj->GetIndexCount(), 0, 0);
    }

}



void GraphicsEngine::EndFrame()
{
    swapChain->Present(1, 0); // 1 = VSync enabled
}

GraphicsEngine::~GraphicsEngine()
{

    // 1. Clear the state of the context so no resources are "bound"
    if (context) {
        context->ClearState();
        context->Flush();
    }

    // 2. Explicitly reset the major ComPtrs
    // This ensures they are destroyed BEFORE CoUninitialize
    device.Reset();
    context.Reset();
    swapChain.Reset();
    renderTargetView.Reset();
    depthStencilView.Reset();
    pDepthStencil.Reset();
    pBackBuffer.Reset();

    // Constant buffers & Shaders
    constantBuffer.Reset();
    materialConstantBuffer.Reset();
    vertexShader.Reset();
    pixelShader.Reset();
    pixelShaderTor.Reset();
    inputLayout.Reset();
    vsBlob.Reset();
    psBlob.Reset();
    psBlobTor.Reset();

    // States
    rasterState.Reset();
    rasterStateWireframe.Reset();
    m_samplerLinear.Reset();
    m_alphaBlendState.Reset();

    CoUninitialize();
}