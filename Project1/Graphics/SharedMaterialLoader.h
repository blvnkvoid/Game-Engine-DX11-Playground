#pragma once
#include "../SharedTypes.h"
#include <map>


class SharedMaterialLoader
{
public:
    SharedMaterialLoader();
    
    void LoadMaterialLibrary(
        const std::string& mtlFile
    );

    MaterialData ResolveMaterial(
        const std::string& materialName
    );

private:
    std::map<std::string, MaterialData> m_materialLib;
};