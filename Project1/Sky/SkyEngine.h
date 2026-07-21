#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "../Scene/Camera.h"

class SkyEngine
{
public:


    bool Initialize(ID3D11Device* device);


    struct SunConstants
    {
        DirectX::XMFLOAT4X4 viewProjection;

        DirectX::XMFLOAT4 sunPosition;
        DirectX::XMFLOAT4 cameraRight;
        DirectX::XMFLOAT4 cameraUp;

        DirectX::XMFLOAT4 sunColor;

        float sunSize;
        float sunIntensity;
        float padding[2];
    };

    struct SkyVertex
    {
        float position[2];
        float texCoord[2];
    };

    struct CloudConstants
    {
        DirectX::XMFLOAT2 windDirection = { 1.0f, 0.0f };

        float time = 0.0f;
        float speed = 0.02f;

        float coverage = 0.5f;
        float density = 0.7f;

        float scale = 1.0f;
        float brightness = 1.0f;

        float nearPlane = 1.0f;
        float farPlane = 1.0f;
        DirectX::XMFLOAT2 paddingNearFar = { 0.0f, 0.0f };

        DirectX::XMFLOAT3 cloudColor = { 1.0f, 1.0f, 1.0f };
        float padding0 = 0.0f;

        DirectX::XMFLOAT3 cloudShadowColor = { 1.0f, 1.0f, 1.0f };
        float padding1 = 0.0f;
    };

    static_assert(sizeof(CloudConstants) == 80);


    struct CloudCameraConstants
    {
        XMFLOAT4X4 inverseViewProjection;
        XMFLOAT3 cameraWorldPosition;
        float cloudHeight;
    };
    static_assert(sizeof(CloudCameraConstants) % 16 == 0);


    ID3D11Buffer* GetBillboardVertexBuffer() const
    {
        return m_BillboardVertexBuffer.Get();
    }

    ID3D11Buffer* GetSunConstantBuffer()
    {
        return m_SunConstantBuffer.Get();
    }
    
    ID3D11VertexShader* GetSunVertexShader()
    {
        return m_SunVertexShader.Get();
    }

    ID3D11PixelShader* GetSunPixelShader()
    {
        return m_SunPixelShader.Get();
    }
    
    ID3D11InputLayout* GetSunInputLayout()
    {
        return m_SunInputLayout.Get();
    }    
    
    ID3D11BlendState* GetSunBlendState()
    {
        return m_AdditiveBlendState.Get();
    }    
    
    ID3D11DepthStencilState* GetDepthState()
    {
        return m_DepthState.Get();
    }
        
    ID3D11RasterizerState* GetRasterState()
    {
        return m_RasterizerState.Get();
    }

    ID3D11Buffer* GetCloudConstantBuffer()
    {
        return m_cloudConstantBuffer.Get();
    }    
    
    ID3D11Buffer* GetCloudCameraConstantBuffer()
    {
        return m_cloudcameraConstantBuffer.Get();
    }

    ID3D11ShaderResourceView* GetCloudNoiseSRV() const
    {
        return m_cloudNoiseSRV.Get();
    }

    ID3D11ShaderResourceView* GetCloudDetailNoiseSRV() const
    {
        return m_cloudDetailNoiseSRV.Get();
    }

    ID3D11SamplerState* GetCloudSampler() const
    {
        return m_cloudSampler.Get();
    }

    ID3D11VertexShader* GetCloudVertexShader() const
    {
        return m_cloudVertexShader.Get();
    }

    ID3D11PixelShader* GetCloudPixelShader() const
    {
        return m_cloudPixelShader.Get();
    }

    ID3D11InputLayout* GetCloudInputLayout() const
    {
        return m_cloudInputLayout.Get();
    }
        

            

private:

    Microsoft::WRL::ComPtr<ID3D11Buffer>       m_BillboardVertexBuffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_SunVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_SunPixelShader;

    Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_SunInputLayout;

    Microsoft::WRL::ComPtr<ID3D11BlendState>        m_AdditiveBlendState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_RasterizerState;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_SunConstantBuffer;


    Microsoft::WRL::ComPtr<ID3D11Buffer>             m_cloudConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>             m_cloudcameraConstantBuffer;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cloudNoiseSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cloudDetailNoiseSRV;

    Microsoft::WRL::ComPtr<ID3D11SamplerState>       m_cloudSampler;


    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_cloudVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_cloudPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_cloudInputLayout;
};