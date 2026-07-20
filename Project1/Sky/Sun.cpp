#include <d3dcompiler.h>
#include "Sun.h"

void Sun::Render(
    ID3D11DeviceContext* context,
    Camera* camera,
    SkyEngine& skyEngine)
{
    SkyEngine::SunConstants cb{};

    DirectX::XMMATRIX view =
        camera->GetViewMatrix();

    DirectX::XMMATRIX projection =
        camera->GetProjectionMatrix();

    DirectX::XMMATRIX viewProjection =
        view * projection;

    DirectX::XMStoreFloat4x4(
        &cb.viewProjection,
        DirectX::XMMatrixTranspose(viewProjection));

    DirectX::XMVECTOR cameraRight =
        DirectX::XMVector3Normalize(
            camera->GetRight());

    DirectX::XMVECTOR cameraUp =
        DirectX::XMVector3Normalize(
            camera->GetUp());

    DirectX::XMStoreFloat4(
        &cb.cameraRight,
        DirectX::XMVectorSetW(cameraRight, 0.0f));

    DirectX::XMStoreFloat4(
        &cb.cameraUp,
        DirectX::XMVectorSetW(cameraUp, 0.0f));


    cb.sunColor =
    {
        m_ColorTint.x,
        m_ColorTint.y,
        m_ColorTint.z,
        1.0f
    };

    DirectX::XMFLOAT3 pos = camera->GetPosition();

    float sunDistance =
        camera->GetFarPlane() * 0.9f;

    DirectX::XMVECTOR cameraForward =
        DirectX::XMVector3Normalize(
            camera->GetForward());

    DirectX::XMVECTOR sunDirection =
        DirectX::XMVector3Normalize(
            DirectX::XMLoadFloat3(
                &m_Direction));

    DirectX::XMVECTOR cameraPositionVector =
        DirectX::XMLoadFloat3(&pos);

    DirectX::XMVECTOR sunPositionVector =
        DirectX::XMVectorMultiplyAdd(
            sunDirection,
            DirectX::XMVectorReplicate(sunDistance),
            cameraPositionVector);

    DirectX::XMStoreFloat4(
        &cb.sunPosition,
        DirectX::XMVectorSetW(sunPositionVector, 1.0f));


    cb.sunSize = m_Distance * 0.008f;
    cb.sunIntensity = m_Intensity;

    D3D11_MAPPED_SUBRESOURCE mapped{};

    HRESULT hr = context->Map(
        skyEngine.GetSunConstantBuffer(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapped);

    if (FAILED(hr))
        return;

    memcpy(
        mapped.pData,
        &cb,
        sizeof(SkyEngine::SunConstants));

    context->Unmap(
        skyEngine.GetSunConstantBuffer(),
        0);

    //----------------------------------------
    // Pipeline
    //----------------------------------------

    UINT stride = sizeof(SkyEngine::SkyVertex);
    UINT offset = 0;
    ID3D11Buffer* vb = skyEngine.GetBillboardVertexBuffer();


    context->IASetVertexBuffers(
        0,
        1,
        &vb,
        &stride,
        &offset);

    context->IASetInputLayout(
        skyEngine.GetSunInputLayout());

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->VSSetShader(
        skyEngine.GetSunVertexShader(),
        nullptr,
        0);

    context->PSSetShader(
        skyEngine.GetSunPixelShader(),
        nullptr,
        0);

    ID3D11Buffer* sunCB =
        skyEngine.GetSunConstantBuffer();

    context->VSSetConstantBuffers(
        0,
        1,
        &sunCB);

    context->PSSetConstantBuffers(
        0,
        1,
        &sunCB);

    float blendFactor[4] = { 0, 0, 0, 0 };

    context->OMSetBlendState(
        skyEngine.GetSunBlendState(),
        blendFactor,
        0xffffffff);

    context->OMSetDepthStencilState(
        skyEngine.GetDepthState(),
        0);

    context->RSSetState(
        skyEngine.GetRasterState());

    context->Draw(6, 0);
}