#pragma once
#include "../Imgui/imgui.h"

class UIContext
{
public:
    float scale = 1.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;

    float S(float value) const
    {
        return value * scale;
    }

    ImVec2 P(float x, float y) const
    {
        return {
            offsetX + x * scale,
            offsetY + y * scale
        };
    }

    ImVec2 Size(float width, float height) const
    {
        return {
            width * scale,
            height * scale
        };
    }
    void Update(float width, float height);
};