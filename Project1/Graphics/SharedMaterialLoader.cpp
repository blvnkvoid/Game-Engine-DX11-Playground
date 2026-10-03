#include "SharedMaterialLoader.h"
#include "MaterialParser.h"
#include "TextureManager.h"
#include "MaterialClassifier.h"

SharedMaterialLoader::SharedMaterialLoader()
{
}

void SharedMaterialLoader::LoadMaterialLibrary(const std::string& mtlFile)
{
    int debug = 0; // BP
    MaterialParser parser;

    parser.LoadMaterial(
        mtlFile,
        "",
        m_materialLib
    );

    for (auto& [materialName, material] : m_materialLib)
    {
        material.gpuMaterial.materialType =
            static_cast<float>(
                MaterialClassifier::Classify(materialName)
                );
    }
}

const MaterialData& SharedMaterialLoader::ResolveMaterial(
    const std::string& materialName)
{
    auto it = m_materialLib.find(materialName);

    if (it != m_materialLib.end())
    {
        return it->second;
    }

    int FUCK = 0;

    static const MaterialData fallback{};
    return fallback;
}
    
const MaterialData& SharedMaterialLoader::ResolveCarMaterial(
    const std::string& materialName)
{
    int op = 0;
    return ResolveMaterial(materialName);
}

const MaterialData& SharedMaterialLoader::ResolveMapMaterial(
    const std::string& materialName)
{
    return ResolveMaterial(materialName);
}


void SharedMaterialLoader::ResolveCarTextures(
    TextureManager* textureManager,
    ID3D11DeviceContext* context,
    const std::wstring& textureFolder)
{
    for (auto& [materialName, material] : m_materialLib)
    {
        if (!material.diffuseTextureName.empty())
        {
            std::wstring textureName(
                material.diffuseTextureName.begin(),
                material.diffuseTextureName.end());

            material.carTextures.diffuse =
                textureManager->GetTexture(
                    textureFolder + textureName,
                    context);
        }

        if (!material.normalTextureName.empty())
        {
            std::wstring textureName(
                material.normalTextureName.begin(),
                material.normalTextureName.end());

            material.carTextures.normal =
                textureManager->GetTexture(
                    textureFolder + textureName,
                    context);
        }

        if (!material.detailTextureName.empty())
        {
            std::wstring textureName(
                material.detailTextureName.begin(),
                material.detailTextureName.end());

            material.carTextures.detail =
                textureManager->GetTexture(
                    textureFolder + textureName,
                    context);
        }

        if (!material.normalDetailTextureName.empty())
        {
            std::wstring textureName(
                material.normalDetailTextureName.begin(),
                material.normalDetailTextureName.end());

            material.carTextures.normalDetail =
                textureManager->GetTexture(
                    textureFolder + textureName,
                    context);
        }

        if (!material.mapsTextureName.empty())
        {
            std::wstring textureName(
                material.mapsTextureName.begin(),
                material.mapsTextureName.end());

            material.carTextures.maps =
                textureManager->GetTexture(
                    textureFolder + textureName,
                    context);
        }
    }
}



void SharedMaterialLoader::ResolveMapTextures(
    TextureManager* textureManager,
    ID3D11DeviceContext* context,
    const std::wstring& textureFolder)
{
    for (auto& [materialName, material] : m_materialLib)
    {
        if (!material.diffuseTextureName.empty())
        {
            std::wstring textureName(
                material.diffuseTextureName.begin(),
                material.diffuseTextureName.end());

            std::wstring fullPath =
                textureFolder + textureName;

            material.mapTextures.diffuse =
                textureManager->GetTexture(fullPath, context);
        }

        if (!material.normalTextureName.empty())
        {
            std::wstring textureName(
                material.normalTextureName.begin(),
                material.normalTextureName.end());

            material.mapTextures.normal =
                textureManager->GetTexture(
                    textureFolder + textureName,
                    context);
        }

        if (!material.detailTextureName.empty())
        {
            std::wstring textureName(
                material.detailTextureName.begin(),
                material.detailTextureName.end());

            material.mapTextures.detail =
                textureManager->GetTexture(
                    textureFolder + textureName,
                    context);
        }

        if (!material.normalDetailTextureName.empty())
        {
            std::wstring textureName(
                material.normalDetailTextureName.begin(),
                material.normalDetailTextureName.end());

            material.mapTextures.normalDetail =
                textureManager->GetTexture(
                    textureFolder + textureName,
                    context);
        }

        if (!material.mapsTextureName.empty())
        {
            std::wstring textureName(
                material.mapsTextureName.begin(),
                material.mapsTextureName.end());

            material.mapTextures.maps =
                textureManager->GetTexture(
                    textureFolder + textureName,
                    context);
        }

    }
}