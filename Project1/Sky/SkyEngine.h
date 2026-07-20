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


    ID3D11Buffer* GetBillboardVertexBuffer() 
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



private:

    //=========================================================
    // Resources
    //=========================================================

    Microsoft::WRL::ComPtr<ID3D11Buffer>       m_BillboardVertexBuffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_SunVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_SunPixelShader;

    Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_SunInputLayout;

    //=========================================================
    // Render States
    //=========================================================

    Microsoft::WRL::ComPtr<ID3D11BlendState>        m_AdditiveBlendState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_RasterizerState;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_SunConstantBuffer;
};