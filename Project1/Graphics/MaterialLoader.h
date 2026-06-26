#pragma once

#include "../SharedTypes.h"
#include <map>

class MaterialLoader {
public:
    MaterialLoader();
    static  void LoadMaterial(const std::string& path, const std::string& targetName, std::map<std::string, MaterialData>& m_materialLib);
};
