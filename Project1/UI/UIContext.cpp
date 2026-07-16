#include "UIContext.h"
#include <windows.h>

void UIContext::Update(float width, float height)
{
    constexpr float referenceWidth = 1920.0f;
    constexpr float referenceHeight = 1080.0f;

    const float scaleX = width / referenceWidth;
    const float scaleY = height / referenceHeight;

    scale = min(scaleX, scaleY);

    offsetX = (width - referenceWidth * scale) * 0.5f;
    offsetY = (height - referenceHeight * scale) * 0.5f;
}