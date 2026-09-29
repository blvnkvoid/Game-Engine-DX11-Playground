#include "SharedMaterialLoader.h"
#include "MaterialParser.h"

SharedMaterialLoader::SharedMaterialLoader()
{
}

void SharedMaterialLoader::LoadMaterialLibrary(const std::string& mtlFile)
{
    MaterialParser parser;

    parser.LoadMaterial(
        mtlFile,
        "",
        m_materialLib
    );
}

MaterialData SharedMaterialLoader::ResolveMaterial(const std::string& materialName)
{
  auto it = m_materialLib.find(materialName);

    if (it != m_materialLib.end())
    {
        return it->second;
    }

    return MaterialData{};
}
