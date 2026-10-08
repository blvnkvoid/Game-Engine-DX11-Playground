#pragma once
#include "../Scene/GameObject.h"
#include "../Cars/CarSetup.h"
#include "../Audio/FmodManager.h"
#include "../Environment/Timecycle.h"
#include "../Environment/Time.h"
#include "../Environment/EnvironmentDefinition.h"
#include "../UI/MainMenu.h"
#include "../UI/Settings.h"
#include "../UI/UIContext.h"
#include "../Sky/Sun.h"
#include "../Sky/Clouds.h"
#include "../Tracks/TrackTable.h"
#include <tracy/TracyD3D11.hpp>
#include "SharedMaterialLoader.h"

class Scene; // Forward declaration (keeps the header light!)
class Camera;
class Input;




class GraphicsEngine {
public:
    GraphicsEngine();
    ~GraphicsEngine();
    void BeginFrame(HWND hWnd,DirectX::XMMATRIX view, DirectX::XMMATRIX projection, float deltaTime, Camera* cam);
    void RenderObject(GameObject* obj, Camera* cam);
    void EndFrame();
    void SetActiveCamera(Camera* camera) { activeCamera = camera; }
    void SetScene(Scene* scene) { m_activeScene = scene; }
    void BeginShadowPass(int cascade);
    bool m_isWireframe = false;
    bool m_gWasPressed = false;
    bool Init(HWND hWnd, int width, int height);
    ID3D11DeviceContext* GetContext() { return context.Get(); }
    ID3D11Device* GetDevice() { return device.Get(); }
    ID3D11Buffer* GetConstantBuffer() { return constantBuffer.Get(); }
    ID3D11Buffer* GetLampConstantBuffer() { return lampConstantBuffer.Get(); }
    SharedSceneData& GetSharedSceneData() { return m_sceneData; }
    DirectX::XMMATRIX GetView() { return activeCamera ? activeCamera->GetViewMatrix() : DirectX::XMMatrixIdentity(); }
    DirectX::XMMATRIX GetProj() { return mProj; }
    TextureManager* GetTextureManager() { return m_textureManager.get(); }
    void SetBrakeAmount(float amount);
    void SetTime(float time);
    SharedSceneData BuildSceneData(Camera* cam, GameObject* player, XMMATRIX world);
    void UpdateEnvironment(float time,
        SharedSceneData& scene,
        float(&clearColor)[4]);
    void ApplyEnvironmentDefinition(const EnvironmentDefinition& def);
    EnvironmentDefinition DefaultEnvironment();


    Time& GetTime();
    void ConfigureUIScale(float renderWidth, float renderHeight);
    Settings settings;

    const UIContext& GetUIContext() const
    {
        return m_uiContext;
    }

    ImFont* GetTelemetryFont() const
    {
        return m_telemetryFont;
    }

    ID3D11DepthStencilState* GetDepthStencilState() const
    {
        return m_depthWriteOnState.Get();
    }

    ID3D11ShaderResourceView* GetLampResourceView() const
    {
        return m_lampSRV.Get();
    }

    ID3D11Buffer* GetLampStructuredBuffer() const
    {
        return m_lampStructuredBuffer.Get();
    }

    SharedSceneData GetSceneData() const
    {
        return m_sceneData;
    }

    Sun GetSun() const
    {
        return m_sun;
    }

    ID3D11ShaderResourceView* GetDepthShaderResourceView() const
    {
        return m_depthStencilSRV.Get();
    }

    ID3D11RenderTargetView* GetRenderTargetView() const
    {
        return renderTargetView.Get();
    }

    Clouds GetClouds() const
    {
        return m_clouds;
    }

    const BoundingFrustum& GetLightFrustum(int cascade) const
    {
        return m_lightFrustum[cascade];
    }

    void PrepareShadowPass(SharedSceneData& sceneData, TrackEntry& track, int cascade, Camera& camera);
    void DrawShadowDebugView();

    SharedMaterialLoader& GetMaterialLoader()
    {
        return loader;
    }

    const UINT shadowResolution[3] =
    {
        4096,
        2048,
        2048
    };

    TracyD3D11Ctx GetTracyGpuContext() const
    {
        return m_tracyGpu;
    }

    void CollectTracyGpu()
    {
        TracyD3D11Collect(m_tracyGpu);
    }
private:
    Sun m_sun;
    Clouds m_clouds;
    TracyD3D11Ctx m_tracyGpu = nullptr;
    SharedSceneData m_sceneData;
    SharedSceneData cb; // [cite: 2026-01-03]
    Camera* activeCamera = nullptr;
    Scene* m_activeScene = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Device>  device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaderTor;    
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> materialConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> lampConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_lampStructuredBuffer;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterStateWireframe;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowrasterState;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;    
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_depthStencilSRV;
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlobTor;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerLinear;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_shadowComparisonSampler;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaBlendState;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_defaultSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_lampSRV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthWriteOnState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthWriteOffState;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_shadowTexture[3];
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_shadowDSV[3];
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowSRV[3];
    D3D11_VIEWPORT m_shadowViewport[3];

    ID3D11VertexShader* m_shadowVertexShader = nullptr;
    ID3D11InputLayout* m_shadowInputLayout = nullptr;
    // Shadow map resources


    ID3D11VertexShader* m_shadowDebugVS = nullptr;
    ID3D11PixelShader* m_shadowDebugPS = nullptr;
    ID3D11SamplerState* m_shadowDebugSampler = nullptr;

    ID3D11DepthStencilState* m_debugDepthDisabled = nullptr;

    // Shadow rendering

    // Sun camera
    XMMATRIX m_lightView[3];
    XMMATRIX m_lightProj[3];
    XMMATRIX m_lightViewProj = XMMatrixIdentity();
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mProj;
    DirectX::XMFLOAT4 m_lightDir = { 0.0f, -1.0f, 1.0f, 0.0f }; // Directional light
    std::unique_ptr<TextureManager> m_textureManager;
    DirectX::XMFLOAT4 m_lightColor = { 1.0f, 0.0f, 0.0f, 1.0f }; // White light
    TimeCycle m_timeCycle;
    EnvironmentState env;
    Time m_time;
    UINT quality = 0;
    EnvironmentDefinition def;

    BoundingFrustum m_lightFrustum[3];

    ImFont* m_telemetryFont = nullptr;
    UIContext m_uiContext;

    SharedMaterialLoader loader;    
};