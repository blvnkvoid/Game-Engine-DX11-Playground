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

    SkyEngine::CloudCameraConstants cameraData = {};

    data.windDirection = { 1.0f, 0.0f };
    data.time = graphicsEngine.GetTime().GetTime();
    data.speed = m_speed;
    data.coverage = m_coverage;
    data.density = m_density;
    data.scale = m_scale;
    data.brightness = m_brightness;
    data.nearPlane = camera->m_nearPlane;
    data.farPlane = camera->m_farPlane;
    data.cloudColor = m_cloudColor;
    data.cloudShadowColor = m_cloudshadowColor;

    XMMATRIX viewProjection =
        XMMatrixMultiply(camera->viewMatrix, camera->projectionMatrix);

    XMMATRIX inverseViewProjection =
        XMMatrixInverse(nullptr, viewProjection);

    XMStoreFloat4x4(
        &cameraData.inverseViewProjection,
        XMMatrixTranspose(inverseViewProjection));

    cameraData.cameraWorldPosition =
        camera->position;

    cameraData.cloudHeight = 500.0f;

    ID3D11Buffer* cloudConstantBuffer =
        skyEngine.GetCloudConstantBuffer();

    ID3D11ShaderResourceView* cloudNoiseSRV =
        skyEngine.GetCloudNoiseSRV();

    ID3D11ShaderResourceView* cloudDetailNoiseSRV =
        skyEngine.GetCloudDetailNoiseSRV();

    ID3D11SamplerState* cloudSampler =
        skyEngine.GetCloudSampler();

    ID3D11Buffer* cloudCameraConstantBuffer =
        skyEngine.GetCloudCameraConstantBuffer();


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


    D3D11_MAPPED_SUBRESOURCE mappedResource = {};

    hr = context->Map(
        skyEngine.GetCloudCameraConstantBuffer(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mappedResource);

    if (FAILED(hr))
    {
        return;
    }

    memcpy(
        mappedResource.pData,
        &cameraData,
        sizeof(SkyEngine::CloudCameraConstants));

    context->Unmap(
        skyEngine.GetCloudCameraConstantBuffer(),
        0);

    context->PSSetConstantBuffers(3, 1, &cloudCameraConstantBuffer);

    context->Draw(6, 0);
}