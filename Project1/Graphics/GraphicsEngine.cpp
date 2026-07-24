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
#include "../UI/Settings.h"

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
    sd.SampleDesc.Count = 2;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = true;


    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
    descDepth.SampleDesc.Count = 2;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags =
        D3D11_BIND_DEPTH_STENCIL |
        D3D11_BIND_SHADER_RESOURCE;

    D3D11_RASTERIZER_DESC rasterSolidCullBack = {};
    rasterSolidCullBack.FillMode = D3D11_FILL_SOLID; // Or D3D11_FILL_WIREFRAME for a cool matrix look!
    rasterSolidCullBack.CullMode = D3D11_CULL_NONE;  // <--- THE CULL KILLER
    rasterSolidCullBack.AntialiasedLineEnable = true;
    rasterSolidCullBack.MultisampleEnable = true;
    rasterSolidCullBack.FrontCounterClockwise = true;      


    D3D11_RASTERIZER_DESC rasterWireframeCullBack = {};
    rasterWireframeCullBack.FillMode = D3D11_FILL_WIREFRAME; // Or D3D11_FILL_WIREFRAME for a cool matrix look!
    rasterWireframeCullBack.CullMode = D3D11_CULL_BACK;  // <--- THE CULL KIL6LER
    rasterWireframeCullBack.AntialiasedLineEnable = true;
    rasterWireframeCullBack.MultisampleEnable = true;
    rasterWireframeCullBack.FrontCounterClockwise = true;

    D3D11_RASTERIZER_DESC shadowrasterSolidCullBack = {};
    shadowrasterSolidCullBack.FillMode = D3D11_FILL_SOLID; // Or D3D11_FILL_WIREFRAME for a cool matrix look!
    shadowrasterSolidCullBack.CullMode = D3D11_CULL_BACK;  // <--- THE CULL KILLER
    shadowrasterSolidCullBack.DepthClipEnable = true;



    D3D11_BUFFER_DESC bd = {};
    D3D11_SUBRESOURCE_DATA init = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // Specifically allows Map()
    bd.ByteWidth = sizeof(SharedSceneData);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;    
    
    D3D11_BUFFER_DESC lbd = {};
    lbd.Usage = D3D11_USAGE_DYNAMIC;
    lbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // Specifically allows Map()
    lbd.ByteWidth = sizeof(LampInfo);
    lbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;


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

    constexpr UINT MAX_LAMPS = 1200;

    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = sizeof(LampData) * MAX_LAMPS;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufferDesc.StructureByteStride = sizeof(LampData);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = MAX_LAMPS;

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc2 = {};
    srvDesc2.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
    srvDesc2.Texture2D.MostDetailedMip = 0;
    srvDesc2.Texture2D.MipLevels = 1;


    D3D11_TEXTURE2D_DESC shadowTexDesc = {};
    shadowTexDesc.Width = 8192;
    shadowTexDesc.Height = 8192;
    shadowTexDesc.MipLevels = 1;
    shadowTexDesc.ArraySize = 1;
    shadowTexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    shadowTexDesc.SampleDesc.Count = 1;
    shadowTexDesc.SampleDesc.Quality = 0;
    shadowTexDesc.CPUAccessFlags = 0;
    shadowTexDesc.BindFlags =
        D3D11_BIND_DEPTH_STENCIL |
        D3D11_BIND_SHADER_RESOURCE;
    shadowTexDesc.Usage = D3D11_USAGE_DEFAULT;
    shadowTexDesc.MiscFlags = 0;

    D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSVDesc = {};
    shadowDSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    shadowDSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    shadowDSVDesc.Texture2D.MipSlice = 0;

    D3D11_SHADER_RESOURCE_VIEW_DESC shadowSRVDesc = {};
    shadowSRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    shadowSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shadowSRVDesc.Texture2D.MostDetailedMip = 0;
    shadowSRVDesc.Texture2D.MipLevels = 1;

    m_shadowViewport = {};
    m_shadowViewport.TopLeftX = 0.0f;
    m_shadowViewport.TopLeftY = 0.0f;
    m_shadowViewport.Width = 8192.0f;
    m_shadowViewport.Height = 8192.0f;
    m_shadowViewport.MinDepth = 0.0f;
    m_shadowViewport.MaxDepth = 1.0f;

    ID3DBlob* shadowVSBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    ID3DBlob* shadowDebugPSBlob = nullptr;
    ID3DBlob* errorPSBlob = nullptr;

    ID3DBlob* shadowDebugVSBlob = nullptr;
    ID3DBlob* errorVSBlob = nullptr;

    D3D11_INPUT_ELEMENT_DESC shadowLayoutDesc[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    D3D11_SAMPLER_DESC samplerDesc{};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;


    D3D11_DEPTH_STENCIL_DESC depthDesc{};
    depthDesc.DepthEnable = FALSE;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    depthDesc.StencilEnable = FALSE;




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
    hr = device->CreateDepthStencilView(pDepthStencil.Get(), &dsvDesc, &depthStencilView);
    if (FAILED(hr)) return false;     
    hr = device->CreateShaderResourceView(pDepthStencil.Get(), &srvDesc2, &m_depthStencilSRV);
    if (FAILED(hr)) return false; 
    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr)) return false;
    hr = device->CreateInputLayout(layout, 4, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    if (FAILED(hr)) return false;
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    if (FAILED(hr)) return false;
    hr = device->CreateBuffer(&bd, nullptr, &constantBuffer);    
    if (FAILED(hr)) return false;
    hr = device->CreateBuffer(&lbd, nullptr, &lampConstantBuffer);    
    if (FAILED(hr)) return false;
    hr = device->CreateBuffer(&bufferDesc, nullptr, &m_lampStructuredBuffer);
    if (FAILED(hr)) return false;
    hr = device->CreateRasterizerState(&rasterSolidCullBack, &rasterState);
    if (FAILED(hr)) return false;
    hr = device->CreateRasterizerState(&rasterWireframeCullBack, &rasterStateWireframe);
    if (FAILED(hr)) return false;    
    hr = device->CreateRasterizerState(&shadowrasterSolidCullBack, &shadowrasterState);
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
    hr = device->CreateShaderResourceView(m_lampStructuredBuffer.Get(), &srvDesc, m_lampSRV.GetAddressOf());
    if (FAILED(hr)) return false;    
    hr = device->CreateTexture2D(&shadowTexDesc, nullptr, &m_shadowMapTexture);
    if (FAILED(hr)) return false;
    hr = device->CreateDepthStencilView(m_shadowMapTexture, &shadowDSVDesc, &m_shadowMapDSV);
    if (FAILED(hr)) return false;
    hr = device->CreateShaderResourceView(m_shadowMapTexture, &shadowSRVDesc, &m_shadowMapSRV);
    if (FAILED(hr)) return false;
    hr = D3DCompileFromFile(L"Shadows.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &shadowVSBlob, &errorBlob);
    if (FAILED(hr)) return false;
    hr = device->CreateVertexShader(shadowVSBlob->GetBufferPointer(), shadowVSBlob->GetBufferSize(), nullptr, &m_shadowVertexShader);
    if (FAILED(hr)) return false;
    hr = device->CreateInputLayout(shadowLayoutDesc, ARRAYSIZE(shadowLayoutDesc), shadowVSBlob->GetBufferPointer(), shadowVSBlob->GetBufferSize(), &m_shadowInputLayout);
    if (FAILED(hr)) return false;
    hr = device->CreateSamplerState(&samplerDesc, &m_shadowDebugSampler);
    if (FAILED(hr)) return false;
    hr = D3DCompileFromFile(L"ShadowDebugPS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &shadowDebugPSBlob, &errorPSBlob);
    if (FAILED(hr)) return false;
    hr = D3DCompileFromFile(L"ShadowDebugVS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &shadowDebugVSBlob, &errorVSBlob);
    if (FAILED(hr)) return false;
    hr = device->CreatePixelShader(shadowDebugPSBlob->GetBufferPointer(), shadowDebugPSBlob->GetBufferSize(), nullptr, &m_shadowDebugPS);
    if (FAILED(hr)) return false;
    hr = device->CreateVertexShader(shadowDebugVSBlob->GetBufferPointer(), shadowDebugVSBlob->GetBufferSize(), nullptr, &m_shadowDebugVS);
    if (FAILED(hr)) return false;
    hr = device->CreateDepthStencilState(&depthDesc, &m_debugDepthDisabled);
    if (FAILED(hr)) return false;

    shadowVSBlob->Release();
    shadowVSBlob = nullptr;
    if (errorBlob)
    {
        errorBlob->Release();
        errorBlob = nullptr;
    }

    shadowDebugPSBlob->Release();
    shadowDebugVSBlob->Release();

    if (errorPSBlob)
    {
        errorPSBlob->Release();
    }

    if (errorVSBlob)
    {
        errorVSBlob->Release();
    }


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
}


void GraphicsEngine::DrawShadowDebugView()
{
    context->OMSetDepthStencilState(m_debugDepthDisabled, 0);
    D3D11_VIEWPORT debugViewport{};

    debugViewport.TopLeftX = 900.0f;
    debugViewport.TopLeftY = 20.0f;
    debugViewport.Width = 400.0f;
    debugViewport.Height = 400.0f;
    debugViewport.MinDepth = 0.0f;
    debugViewport.MaxDepth = 1.0f;

    context->RSSetViewports(1, &debugViewport);


    context->IASetVertexBuffers(
        0,
        0,
        nullptr,
        nullptr,
        nullptr
    );

    context->IASetIndexBuffer(
        nullptr,
        DXGI_FORMAT_UNKNOWN,
        0
    );

    context->IASetInputLayout(nullptr);

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    context->VSSetShader(
        m_shadowDebugVS,
        nullptr,
        0
    );

    context->PSSetShader(
        m_shadowDebugPS,
        nullptr,
        0
    );

    ID3D11ShaderResourceView* shadowSRV = m_shadowMapSRV;

    context->PSSetShaderResources(
        0,
        1,
        &shadowSRV
    );

    context->PSSetSamplers(
        0,
        1,
        &m_shadowDebugSampler
    );

    context->RSSetViewports(1, &debugViewport);

    context->Draw(3, 0);

    ID3D11ShaderResourceView* nullSRV = nullptr;

    context->PSSetShaderResources(
        0,
        1,
        &nullSRV
    );
}


Time& GraphicsEngine::GetTime()
{
    return m_time;
}

void GraphicsEngine::ConfigureUIScale(float width, float height)
{
    m_uiContext.Update(width, height);

    const float uiScale = m_uiContext.scale;

    ImGuiIO& io = ImGui::GetIO();


    io.FontGlobalScale = 1.0f;
    io.Fonts->Clear();

    m_telemetryFont = io.Fonts->AddFontFromFileTTF(
        "C:\\Windows\\Fonts\\consola.ttf",
        18.0f * uiScale);

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::GetStyle().ScaleAllSizes(uiScale);

    OutputDebugStringA(
        ("ItemSpacing.y = " + std::to_string(style.ItemSpacing.y) + "\n").c_str());

    OutputDebugStringA(
        ("FramePadding.y = " + std::to_string(style.FramePadding.y) + "\n").c_str());

    io.Fonts->Build();
}

SharedSceneData GraphicsEngine::BuildSceneData(Camera* cam, GameObject* player, XMMATRIX world)
{
    SharedSceneData sd = {};

    sd.world = XMMatrixTranspose(world);
    sd.view = XMMatrixTranspose(cam->GetViewMatrix());
    sd.projection = XMMatrixTranspose(cam->GetProjectionMatrix());

    sd.lightDirection = m_sceneData.lightDirection;
    sd.lightColor = m_sceneData.lightColor;

    sd.ambientIntensity = m_sceneData.ambientIntensity;
    sd.headlightIntensity = m_sceneData.headlightIntensity;

    sd.lightView = XMMatrixTranspose(m_lightView);
    sd.lightProjection = XMMatrixTranspose(m_lightProj);

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
    OutputDebugStringA(("Applying time: " + std::to_string(def.startTime) + "\n").c_str());
    m_time.SetTime(def.startTime);
    m_time.SetTimeScale(def.timeScale);
    m_time.SetShaderTime(def.shaderTime);
    m_time.SetShaderTimeScale(def.shaderTimeScale);
    m_time.PauseTime(!def.dynamicTime);
}

void GraphicsEngine::BeginShadowPass()
{
    context->ClearDepthStencilView(m_shadowMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
    context->OMSetRenderTargets(0, nullptr, m_shadowMapDSV);
    context->RSSetViewports(1, &m_shadowViewport);
}

void GraphicsEngine::PrepareShadowPass(SharedSceneData& sceneData)
{
    XMVECTOR lightDirection =
        XMVector3Normalize(
            XMLoadFloat4(
                &sceneData.lightDirection
            )
        );

    XMVECTOR carPosition =
        XMLoadFloat4(
            &sceneData.carPosition
        );

    XMVECTOR carForward =
        XMVector3Normalize(
            XMLoadFloat4(
                &sceneData.carForward
            )
        );

    constexpr float shadowForwardOffset = 350.0f;
    constexpr float shadowAreaSize = 800.0f;

    XMVECTOR target =
        XMVectorAdd(
            carPosition,
            XMVectorScale(
                carForward,
                shadowForwardOffset
            )
        );

    constexpr float lightDistance = 1500.0f;

    XMVECTOR lightPosition =
        XMVectorSubtract(
            target,
            XMVectorScale(
                lightDirection,
                lightDistance
            )
        );

    XMVECTOR up =
        XMVectorSet(
            0.0f,
            1.0f,
            0.0f,
            0.0f
        );

    m_lightView =
        XMMatrixLookAtLH(
            lightPosition,
            target,
            up
        );


    m_lightProj = XMMatrixOrthographicLH(
        shadowAreaSize,
        shadowAreaSize,
        50.0f,
        2000.0f
    );

    sceneData.world =
        XMMatrixTranspose(
            XMMatrixIdentity()
        );

    sceneData.view =
        XMMatrixTranspose(
            m_lightView
        );

    sceneData.projection =
        XMMatrixTranspose(
            m_lightProj
        );

    context->VSSetShader(
        m_shadowVertexShader,
        nullptr,
        0
    );

    context->PSSetShader(
        nullptr,
        nullptr,
        0
    );

    context->IASetInputLayout(
        m_shadowInputLayout
    );

    context->RSSetState(
        shadowrasterState.Get()
    );
}
void GraphicsEngine::BeginFrame(HWND hWnd, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, float deltaTime, Camera* cam)
{
    bool gIsDown = GetAsyncKeyState('G') & 0x8000;

    if (gIsDown && !m_gWasPressed) {
        m_isWireframe = !m_isWireframe; // Flip the switch
    }

    float envTime = std::fmod(m_time.GetTime(), m_timeCycle.GetCycleLength());
    if (envTime < 0.0f)
        envTime += m_timeCycle.GetCycleLength();
    m_time.Update(deltaTime);
    m_timeCycle.Update(envTime, env);
    //m_timeCycle.UpdateSun(m_time,cam, m_sun);
    m_timeCycle.UpdateClouds(envTime, m_clouds);
    m_gWasPressed = GetAsyncKeyState('G') & 0x8000;
    m_sceneData.time = m_time.GetShaderTime();
    m_sceneData.view = XMMatrixTranspose(view);
    m_sceneData.projection = XMMatrixTranspose(projection);
    m_sceneData.lightDirection = env.lightDirection;
    m_sceneData.lightColor = env.lightColor;
    m_sceneData.ambientIntensity = env.ambientIntensity;
    m_sceneData.headlightIntensity = env.headlightIntensity;    

    float blendFactor[4] = { 0, 0, 0, 0 };
    context->ClearRenderTargetView(renderTargetView.Get(), env.clearColor);
    context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    ID3D11ShaderResourceView* shadowSRV = m_shadowMapSRV;
    context->PSSetShaderResources(2, 1, &shadowSRV);


    if (m_isWireframe) {
        context->RSSetState(rasterStateWireframe.Get());
    }
    else {
        context->RSSetState(rasterState.Get());
    }


    context->OMSetBlendState(
        nullptr,
        nullptr,
        0xffffffff);

    context->OMSetDepthStencilState(
        m_depthWriteOnState.Get(),
        0);




    RECT rc;
    GetClientRect(hWnd, &rc);


    D3D11_VIEWPORT vp = {
        0.0f, 0.0f,
        settings.width,
        settings.height,
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
        m_alphaBlendState.Get(),
        m_sceneData.time,
        m_sceneData
    );
}

void GraphicsEngine::EndFrame()
{
    swapChain->Present(0, 0); // 1 = VSync enabled
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