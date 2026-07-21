#include "Clouds.h"
#include "SkyEngine.h"
#include "../Scene/Camera.h"
#include "../Graphics/GraphicsEngine.h"


void Clouds::Render(
    ID3D11DeviceContext* context,
    SkyEngine& skyEngine,
    GraphicsEngine& graphicsEngine,
    Camera* camera)
{
    if (!context)
    {
        return;
    }


    SkyEngine::CloudConstants data{};

    data.windDirection = { 1.0f, 0.0f };
    data.time = graphicsEngine.GetTime().GetTime();
    data.speed = 0.02f;
    data.coverage = 0.45f;
    data.density = 1.0f;
    data.scale = 0.5f;
    data.brightness = 1.0f;
    data.nearPlane = camera->m_nearPlane;
    data.farPlane = camera->m_farPlane;

    ID3D11Buffer* cloudConstantBuffer =
        skyEngine.GetCloudConstantBuffer();

    ID3D11ShaderResourceView* cloudNoiseSRV =
        skyEngine.GetCloudNoiseSRV();

    ID3D11ShaderResourceView* cloudDetailNoiseSRV =
        skyEngine.GetCloudDetailNoiseSRV();

    ID3D11SamplerState* cloudSampler =
        skyEngine.GetCloudSampler();


    context->IASetInputLayout(
        skyEngine.GetCloudInputLayout());

    context->VSSetShader(
        skyEngine.GetCloudVertexShader(),
        nullptr,
        0);

    context->PSSetShader(
        skyEngine.GetCloudPixelShader(),
        nullptr,
        0);

    // These slots must later match the HLSL register declarations.
    context->PSSetConstantBuffers(
        2,
        1,
        &cloudConstantBuffer);

    ID3D11ShaderResourceView* depthSRV =
        graphicsEngine.GetDepthShaderResourceView();

    ID3D11ShaderResourceView* cloudTextures[] =
    {
        cloudNoiseSRV,
        cloudDetailNoiseSRV,
        depthSRV
    };

    ID3D11RenderTargetView* rtv = graphicsEngine.GetRenderTargetView();


    context->OMSetRenderTargets(
        1,
        &rtv,
        nullptr);

    context->PSSetShaderResources(
        0,
        3,
        cloudTextures);

    context->PSSetSamplers(
        0,
        1,
        &cloudSampler);    



    D3D11_MAPPED_SUBRESOURCE mapped{};


    HRESULT hr = context->Map(
        skyEngine.GetCloudConstantBuffer(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapped);

    

    if (FAILED(hr))
        return;


    memcpy(
        mapped.pData,
        &data,
        sizeof(SkyEngine::CloudConstants));
    
    context->Unmap(
        skyEngine.GetCloudConstantBuffer(),
        0);
        

    UINT stride = sizeof(SkyEngine::SkyVertex);
    UINT offset = 0;

    ID3D11Buffer* billboard =
        skyEngine.GetBillboardVertexBuffer();

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->IASetVertexBuffers(
        0,
        1,
        &billboard,
        &stride,
        &offset);

    context->Draw(6, 0);
}