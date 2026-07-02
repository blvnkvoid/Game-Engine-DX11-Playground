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
#include "../Input/Input.h"
#include "../Environment/Time.h"
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

    D3D11_DEPTH_STENCIL_DESC depthOn = {};
    depthOn.DepthEnable = TRUE;
    depthOn.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthOn.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    D3D11_DEPTH_STENCIL_DESC depthOff = {};
    depthOff.DepthEnable = TRUE;
    depthOff.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // key part
    depthOff.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;


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
    hr = device->CreateDepthStencilState(&depthOff, m_depthWriteOffState.GetAddressOf());
    if (FAILED(hr)) return false;
    hr = device->CreateDepthStencilState(&depthOn, m_depthWriteOnState.GetAddressOf());
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

void GraphicsEngine::SetBrakeAmount(float amount)
{
    m_sceneData.brakeAmount = amount;
    OutputDebugStringA(std::to_string(amount).c_str());
}

void GraphicsEngine::SetTime(float time)
{
    m_sceneData.time = m_time.GetTime();
    OutputDebugStringA("time: ");
    OutputDebugStringA(std::to_string(time).c_str());
}

SharedSceneData GraphicsEngine::BuildSceneData(Camera* cam, GameObject* player, XMMATRIX world)
{
    SharedSceneData sd = {};

    sd.world = XMMatrixTranspose(world);
    sd.view = XMMatrixTranspose(cam->GetViewMatrix());
    sd.projection = XMMatrixTranspose(cam->GetProjectionMatrix());

    sd.lightDirection = m_lightDir;
    sd.lightColor = m_lightColor;

    sd.ambientIntensity = m_sceneData.ambientIntensity;
    sd.headlightIntensity = m_sceneData.headlightIntensity;

    XMFLOAT3 camPos = cam->GetPosition();
    sd.cameraPosition = XMFLOAT4(camPos.x, camPos.y, camPos.z, 1.0f);

    XMVECTOR camForward = XMVector3Normalize(cam->GetForwardVector());
    XMStoreFloat4(&sd.cameraDirection, camForward);
    sd.cameraDirection.w = 0.0f;

    sd.brakeAmount = m_sceneData.brakeAmount;
    sd.time = m_sceneData.time;

    if (player)
    {
        XMMATRIX carWorld = player->GetWorldMatrix();

        // Your current test says car nose is local +X.
        XMVECTOR localForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

        XMVECTOR worldForward = XMVector3TransformNormal(localForward, carWorld);
        worldForward = XMVector3Normalize(worldForward);

        XMStoreFloat4(&sd.carForward, worldForward);
        sd.carForward.w = 0.0f;

        XMVECTOR localHeadlightPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); 

        XMVECTOR worldHeadlightPos = XMVector3TransformCoord(localHeadlightPos, carWorld);

        XMStoreFloat4(&sd.carPosition, worldHeadlightPos);
        sd.carPosition.w = 1.0f;
    }

    return sd;
}

void GraphicsEngine::ApplyEnvironmentDefinition(const EnvironmentDefinition& def)
{
    m_time.SetTime(def.startTime);
    m_time.SetTimeScale(def.timeScale);
    m_time.PauseTime(def.dynamicTime);
}

EnvironmentDefinition GraphicsEngine::DefaultEnvironment()
{
    EnvironmentDefinition defenv;
    defenv.startTime = 0.0f;
    defenv.timeScale = 1.0f;
    defenv.dynamicTime = true;
    return defenv;
}



void GraphicsEngine::BeginFrame(HWND hWnd, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, float deltaTime)
{
    bool gIsDown = GetAsyncKeyState('G') & 0x8000;

    if (gIsDown && !m_gWasPressed)
        m_isWireframe = !m_isWireframe;

    m_gWasPressed = gIsDown;

    m_time.Update(deltaTime);

    m_sceneData.view = XMMatrixTranspose(view);
    m_sceneData.projection = XMMatrixTranspose(projection);
    m_sceneData.lightDirection = env.lightDirection;
    m_sceneData.lightColor = env.lightColor;
    m_sceneData.ambientIntensity = env.ambientIntensity;
    m_sceneData.headlightIntensity = env.headlightIntensity;    

    float envTime = std::fmod(m_time.GetTime(), m_timeCycle.GetCycleLength());

    if (envTime < 0.0f)
        envTime += m_timeCycle.GetCycleLength();

    m_timeCycle.Update(envTime, env);

    float blendFactor[4] = { 0, 0, 0, 0 };

    context->OMSetBlendState(m_alphaBlendState.Get(), blendFactor, 0xffffffff);
    context->ClearRenderTargetView(renderTargetView.Get(), env.clearColor);
    context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
    context->RSSetState(m_isWireframe ? rasterStateWireframe.Get() : rasterState.Get());

    RECT rc;
    GetClientRect(hWnd, &rc);

    D3D11_VIEWPORT vp = {
        0.0f, 0.0f,
        float(rc.right - rc.left),
        float(rc.bottom - rc.top),
        0.0f, 1.0f
    };

    context->RSSetViewports(1, &vp);
    context->IASetInputLayout(inputLayout.Get());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(vertexShader.Get(), nullptr, 0);
    context->PSSetShader(pixelShader.Get(), nullptr, 0);
}

void GraphicsEngine::RenderObject(GameObject* obj, Camera* cam)
{
    if (!obj)
        return;

    Model* model = obj->GetModel();

    if (!model)
    {
        OutputDebugStringA("ERROR: RenderObject called with no Model\n");
        return;
    }

    ID3D11ShaderResourceView* objectTextureRV = obj->GetTexture();
    context->PSSetShaderResources(0, 1, &objectTextureRV);
    context->PSSetSamplers(0, 1, m_samplerLinear.GetAddressOf());

    XMMATRIX world = obj->GetWorldMatrix();
    XMMATRIX view = cam->GetViewMatrix();
    XMMATRIX projection = cam->GetProjectionMatrix();

    model->BindAndDraw(
        context.Get(),
        sizeof(SharedVertex),
        world,
        view,
        projection,
        cam,
        m_sceneData.brakeAmount,
        m_depthWriteOnState.Get(),
        m_depthWriteOffState.Get(),
        m_sceneData.time
    );
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