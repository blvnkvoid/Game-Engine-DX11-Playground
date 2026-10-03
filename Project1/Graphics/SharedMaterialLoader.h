    #pragma once
    #include "../SharedTypes.h"
    #include <map>

    class TextureManager;
    class MaterialClassifier;

    class SharedMaterialLoader
    {
    public:
        SharedMaterialLoader();
    
        void LoadMaterialLibrary(
            const std::string& mtlFile
        );

        const MaterialData& ResolveMaterial(const std::string& materialName);

        const MaterialData& ResolveCarMaterial(const std::string& materialName);

        const MaterialData& ResolveMapMaterial(const std::string& materialName);

        void ResolveCarTextures(TextureManager* textureManager, ID3D11DeviceContext* context, const std::wstring& textureFolder);
        void ResolveMapTextures(TextureManager* textureManager, ID3D11DeviceContext* context, const std::wstring& textureFolder);

    private:
        std::map<std::string, MaterialData> m_materialLib;

    };