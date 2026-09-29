#pragma once

#include "../SharedTypes.h"
#include <map>

class MaterialParser {
public:
    MaterialParser();
    static  void LoadMaterial(const std::string& path, const std::string& targetName, std::map<std::string, MaterialData>& m_materialLib);
};
