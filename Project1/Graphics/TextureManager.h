#pragma once

#include "GraphicsEngine.h"

class TextureManager {
public:
    TextureManager(ID3D11Device* device);
    ~TextureManager();
    ID3D11ShaderResourceView* GetTexture(std::wstring filename, ID3D11DeviceContext* context);

private:
    ID3D11Device* m_device;
    std::map<std::wstring, ID3D11ShaderResourceView*> m_textures;
};
