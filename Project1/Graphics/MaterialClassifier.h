#pragma once
#include <string>
#include <algorithm>
#include "../SharedTypes.h"

class MaterialClassifier
{
public:
    static MaterialType Classify(const std::string& materialName)
    {
        std::string n = materialName;
        std::transform(n.begin(), n.end(), n.begin(), ::tolower);

        if (ContainsAny(n, { "glass",
    "ext_glass",
    "int_glass",
    "glass_windows",
    "glass_light",
    "windscreen",
    "window",
    "damage_glass" }))
            return MaterialType::MATERIAL_GLASS;

        if (ContainsAny(n, { "tyre", "tire", "rubber" }))
            return MaterialType::MATERIAL_RUBBER;

        if (ContainsAny(n, { "livery"}))
            return MaterialType::MATERIAL_LIVERY;

        if (ContainsAny(n, { "alcantara", "alcnt" }))
            return MaterialType::MATERIAL_ALCANTARA;

        if (ContainsAny(n, { "brake_light" }))
            return MaterialType::MATERIAL_BRAKE_LIGHT;

        if (ContainsAny(n, { "head_light" }))
            return MaterialType::MATERIAL_HEAD_LIGHT;

        if (ContainsAny(n, { "pace_light" }))
            return MaterialType::MATERIAL_PACE_LIGHT;

        if (ContainsAny(n, { "body", "paint" }))
            return MaterialType::MATERIAL_SOLID_PAINT;        
        
        if (ContainsAny(n, { "decal", "scdecal","text"}))
            return MaterialType::MATERIAL_DECAL_TEXT;

        return MaterialType::MATERIAL_DEFAULT;
    }

private:
    static bool ContainsAny(const std::string& text,
        std::initializer_list<const char*> words)
    {
        for (auto word : words)
        {
            if (text.find(word) != std::string::npos)
                return true;
        }
        return false;
    }
};