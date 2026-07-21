#include "SkyEngine.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <filesystem>
#include <d3dcompiler.h>
bool SkyEngine::Initialize(ID3D11Device* device)
{
    if (!device)
        return false;

    //=========================================================
    // Billboard
    //=========================================================

    const SkyVertex vertices[] =
    {
        { {-1.0f, -1.0f}, {0.0f, 1.0f} },
        { {-1.0f,  1.0f}, {0.0f, 0.0f} },
        { { 1.0f, -1.0f}, {1.0f, 1.0f} },

        { { 1.0f, -1.0f}, {1.0f, 1.0f} },
        { {-1.0f,  1.0f}, {0.0f, 0.0f} },
        { { 1.0f,  1.0f}, {1.0f, 0.0f} }
    };

    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;

    D3D11_SUBRESOURCE_DATA vbData{};
    vbData.pSysMem = vertices;

    HRESULT hr = device->CreateBuffer(
        &vbDesc,
        &vbData,
        m_BillboardVertexBuffer.GetAddressOf());

    if (FAILED(hr))
        return false;

    //=========================================================
    // Load shaders
    //=========================================================

    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;


    hr = D3DCompileFromFile(L"Sun.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, nullptr);
    if (FAILED(hr)) return false;

    hr = D3DCompileFromFile(L"Sun.hlsl", nullptr, nullptr, "SunPS", "ps_5_0", 0, 0, &psBlob, nullptr);

    if (FAILED(hr)) return false;

    hr = device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        m_SunVertexShader.GetAddressOf());

    if (FAILED(hr))
        return false;

    hr = device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        m_SunPixelShader.GetAddressOf());

    if (FAILED(hr))
        return false;

    //=========================================================
    // Input layout
    //=========================================================

    const D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            8,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    hr = device->CreateInputLayout(
        layout,
        ARRAYSIZE(layout),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        m_SunInputLayout.GetAddressOf());

    if (FAILED(hr))
        return false;

    //=========================================================
    // Additive blend
    //=========================================================

    D3D11_BLEND_DESC blendDesc{};

    auto& rt = blendDesc.RenderTarget[0];

    rt.BlendEnable = TRUE;

    rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    rt.DestBlend = D3D11_BLEND_ONE;
    rt.BlendOp = D3D11_BLEND_OP_ADD;

    rt.SrcBlendAlpha = D3D11_BLEND_ONE;
    rt.DestBlendAlpha = D3D11_BLEND_ZERO;
    rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;

    rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = device->CreateBlendState(
        &blendDesc,
        m_AdditiveBlendState.GetAddressOf());

    if (FAILED(hr))
        return false;

    //=========================================================
    // Depth
    //=========================================================

    D3D11_DEPTH_STENCIL_DESC depthDesc{};

    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    depthDesc.StencilEnable = FALSE;

    hr = device->CreateDepthStencilState(
        &depthDesc,
        m_DepthState.GetAddressOf());

    if (FAILED(hr))
        return false;

    //=========================================================
    // Rasterizer
    //=========================================================

    D3D11_RASTERIZER_DESC rasterDesc{};

    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthClipEnable = TRUE;

    hr = device->CreateRasterizerState(
        &rasterDesc,
        m_RasterizerState.GetAddressOf());

    if (FAILED(hr))
        return false;

    //=========================================================
    // Constant Buffer
    //=========================================================

    D3D11_BUFFER_DESC cbDesc{};
    cbDesc.ByteWidth = sizeof(SunConstants);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = device->CreateBuffer(
        &cbDesc,
        nullptr,
        m_SunConstantBuffer.GetAddressOf());

    if (FAILED(hr))
        return false;


    //=====================================================
// Cloud Vertex Shader
//=====================================================

    Microsoft::WRL::ComPtr<ID3DBlob> cloudVSBlob;

    hr = (D3DCompileFromFile(
        L"Clouds.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS,
        0,
        cloudVSBlob.GetAddressOf(),
        nullptr));

   hr = (device->CreateVertexShader(
        cloudVSBlob->GetBufferPointer(),
        cloudVSBlob->GetBufferSize(),
        nullptr,
        m_cloudVertexShader.GetAddressOf()));

   hr = (device->CreateInputLayout(
        layout,
        ARRAYSIZE(layout),
        cloudVSBlob->GetBufferPointer(),
        cloudVSBlob->GetBufferSize(),
        m_cloudInputLayout.GetAddressOf()));


    //=====================================================
    // Cloud Pixel Shader
    //=====================================================

    Microsoft::WRL::ComPtr<ID3DBlob> cloudPSBlob;

    hr = (D3DCompileFromFile(
        L"Clouds.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PSMain",
        "ps_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS,
        0,
        cloudPSBlob.GetAddressOf(),
        nullptr));

    hr = (device->CreatePixelShader(
        cloudPSBlob->GetBufferPointer(),
        cloudPSBlob->GetBufferSize(),
        nullptr,
        m_cloudPixelShader.GetAddressOf()));

    //====================================================
    // Cloud Constant Buffer
    //====================================================

    D3D11_BUFFER_DESC cloudCBDesc = {};
    cloudCBDesc.Usage = D3D11_USAGE_DYNAMIC;
    cloudCBDesc.ByteWidth = sizeof(CloudConstants);
    cloudCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cloudCBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


    hr = device->CreateBuffer(
        &cloudCBDesc,
        nullptr,
        m_cloudConstantBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }


    hr = CreateWICTextureFromFile(
        device,
        L"Assets\\Sky\\CloudNoise.png",
        nullptr,
        m_cloudNoiseSRV.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }


    hr = CreateWICTextureFromFile(
        device,
        L"Assets\\Sky\\CloudDetailNoise.png",
        nullptr,
        m_cloudDetailNoiseSRV.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }


    D3D11_SAMPLER_DESC samplerDesc = {};

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(
        &samplerDesc,
        m_cloudSampler.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    D3D11_BUFFER_DESC cloudcameraCBDesc = {};
    cloudcameraCBDesc.Usage = D3D11_USAGE_DYNAMIC;
    cloudcameraCBDesc.ByteWidth = sizeof(CloudCameraConstants);
    cloudcameraCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cloudcameraCBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


    hr = device->CreateBuffer(
        &cloudcameraCBDesc,
        nullptr,
        m_cloudcameraConstantBuffer.GetAddressOf());


    if (FAILED(hr))
    {
        return false;
    }




    return true;
}

