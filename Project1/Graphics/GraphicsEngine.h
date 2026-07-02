#pragma once
#include "../Scene/GameObject.h"
#include "../Cars/CarSetup.h"
#include "../Audio/FmodManager.h"
#include "../Environment/Timecycle.h"
#include "../Environment/Time.h"
#include "../Environment/EnvironmentDefinition.h"
#include "../UI/MainMenu.h"

class Scene; // Forward declaration (keeps the header light!)
class Camera;
class Input;





class GraphicsEngine {
public:
    GraphicsEngine();
    ~GraphicsEngine();
    void BeginFrame(HWND hWnd,DirectX::XMMATRIX view, DirectX::XMMATRIX projection, float deltaTime);
    void RenderObject(GameObject* obj, Camera* cam);
    void EndFrame();
    void SetActiveCamera(Camera* camera) { activeCamera = camera; }
    void SetScene(Scene* scene) { m_activeScene = scene; }
    bool m_isWireframe = false;
    bool m_gWasPressed = false;
    bool Init(HWND hWnd, int width, int height);
    ID3D11DeviceContext* GetContext() { return context.Get(); }
    ID3D11Device* GetDevice() { return device.Get(); }
    ID3D11Buffer* GetConstantBuffer() { return constantBuffer.Get(); }
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

private:
    SharedSceneData m_sceneData;
    SharedSceneData cb; // [cite: 2026-01-03]
    Camera* activeCamera = nullptr;
    Scene* m_activeScene = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Device>  device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaderTor;    
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> materialConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterStateWireframe;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlobTor;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerLinear;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaBlendState;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_defaultSRV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthWriteOnState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthWriteOffState;
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mProj;
    DirectX::XMFLOAT4 m_lightDir = { 0.0f, -1.0f, 1.0f, 0.0f }; // Directional light
    std::unique_ptr<TextureManager> m_textureManager;
    DirectX::XMFLOAT4 m_lightColor = { 1.0f, 1.0f, 1.0f, 1.0f }; // White light
    TimeCycle m_timeCycle;
    EnvironmentState env;
    Time m_time;
    UINT quality = 0;
    EnvironmentDefinition def;

};