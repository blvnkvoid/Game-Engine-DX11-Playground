#include "TextureManager.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <d3d11.h>
#include <map>

TextureManager::TextureManager(ID3D11Device* device) : m_device(device) {

}

ID3D11ShaderResourceView* TextureManager::GetTexture(std::wstring filename, ID3D11DeviceContext* context) {

    auto it = m_textures.find(filename);
    if (it != m_textures.end()) {
        return it->second;
    }


    ID3D11ShaderResourceView* srv = nullptr;
    HRESULT hr = E_FAIL; 

    bool isDDS = filename.find(L".dds") != std::wstring::npos;

    if (isDDS) {
        hr = DirectX::CreateDDSTextureFromFile(
            m_device,
            filename.c_str(),
            nullptr,
            &srv
        );
    }
    else {
        hr = DirectX::CreateWICTextureFromFileEx(
            m_device,
            context,
            filename.c_str(),
            0,
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
            0,
            D3D11_RESOURCE_MISC_GENERATE_MIPS,
            DirectX::WIC_LOADER_DEFAULT,
            nullptr,
            &srv
        );
    }

    if (SUCCEEDED(hr) && srv != nullptr) {
        if (!isDDS) {
            context->GenerateMips(srv);
        }

        m_textures[filename] = srv;
    }   
    else {     
        return nullptr; 
    }

    return srv;
}


TextureManager::~TextureManager() {
    for (auto& pair : m_textures) {
        if (pair.second) {
            pair.second->Release();
        }
    }
    m_textures.clear();
}