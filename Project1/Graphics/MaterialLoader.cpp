#include "MaterialLoader.h"
#include "Model.h"

MaterialLoader::MaterialLoader()
{

}
void MaterialLoader::LoadMaterial(const std::string& filename, const std::string& targetName, std::map<std::string, MaterialData>& m_materialLib) {
    std::ifstream file(filename);
    if (!file.is_open()) return;
    std::string line;
    std::string currentMatName;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;
        if (prefix == "newmtl") {
            ss >> currentMatName;
            m_materialLib[currentMatName].gpuMaterial.d = 1.0f;
        }
        if (prefix == "Kd") {   
            ss >> m_materialLib[currentMatName].gpuMaterial.diffuseColor.x >> m_materialLib[currentMatName].gpuMaterial.diffuseColor.y >> m_materialLib[currentMatName].gpuMaterial.diffuseColor.z;
        }
        else if (prefix == "Ns") {
            ss >> m_materialLib[currentMatName].gpuMaterial.specularPower;
        }
        else if (prefix == "Ks") {
            ss >> m_materialLib[currentMatName].gpuMaterial.specularColor.x >> m_materialLib[currentMatName].gpuMaterial.specularColor.y >> m_materialLib[currentMatName].gpuMaterial.specularColor.z;
        }
        else if (prefix == "Ka") {
            ss >> m_materialLib[currentMatName].gpuMaterial.ambientColor.x >> m_materialLib[currentMatName].gpuMaterial.ambientColor.y >> m_materialLib[currentMatName].gpuMaterial.ambientColor.z;
        }

        if (prefix == "d") {
            ss >> m_materialLib[currentMatName].gpuMaterial.d;
        }
        else if (prefix == "Tr") {
            float transparency;
            ss >> transparency;
            m_materialLib[currentMatName].gpuMaterial.d = 1.0f - transparency;
        }    
        else if (prefix == "map_Kd")
        {
            std::string fullPath;
            std::getline(ss, fullPath);

            while (!fullPath.empty() && fullPath[0] == ' ')
                fullPath.erase(fullPath.begin());

            size_t lastSlash = fullPath.find_last_of("/\\");
            if (lastSlash != std::string::npos)
                m_materialLib[currentMatName].diffuseTextureName = fullPath.substr(lastSlash + 1);
            else
                m_materialLib[currentMatName].diffuseTextureName = fullPath;
        }       
        
        else if (prefix == "map_Detail")
        {
            std::string fullPath;
            std::getline(ss, fullPath);

            while (!fullPath.empty() && fullPath[0] == ' ')
                fullPath.erase(fullPath.begin());

            size_t lastSlash = fullPath.find_last_of("/\\");

            if (lastSlash != std::string::npos)
                m_materialLib[currentMatName].detailTextureName =
                fullPath.substr(lastSlash + 1);
            else
                m_materialLib[currentMatName].detailTextureName =
                fullPath;
        }        
        
        else if (prefix == "map_NormalDetail")
        {
            std::string fullPath;
            std::getline(ss, fullPath);

            while (!fullPath.empty() && fullPath[0] == ' ')
                fullPath.erase(fullPath.begin());

            size_t lastSlash = fullPath.find_last_of("/\\");

            if (lastSlash != std::string::npos)
                m_materialLib[currentMatName].normalDetailTextureName =
                fullPath.substr(lastSlash + 1);
            else
                m_materialLib[currentMatName].normalDetailTextureName =
                fullPath;
        }

        else if (prefix == "map_Bump")
        {
            std::string fullPath;
            std::getline(ss, fullPath);

            while (!fullPath.empty() && fullPath[0] == ' ')
                fullPath.erase(fullPath.begin());

            // Blender puts "-bm 1.000000" before the filename
            if (fullPath.rfind("-bm ", 0) == 0)
            {
                size_t firstSpace = fullPath.find(' ');
                size_t secondSpace = fullPath.find(' ', firstSpace + 1);

                if (secondSpace != std::string::npos)
                    fullPath = fullPath.substr(secondSpace + 1);
            }

            size_t lastSlash = fullPath.find_last_of("/\\");
            if (lastSlash != std::string::npos)
                m_materialLib[currentMatName].normalTextureName =
                fullPath.substr(lastSlash + 1);
            else
                m_materialLib[currentMatName].normalTextureName = fullPath;
        }
    } 
}