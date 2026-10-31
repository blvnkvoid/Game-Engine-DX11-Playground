#pragma once
#include "../SharedTypes.h"
#include "../UI/UIContext.h"

class Settings
{
public:
		void Draw(const UIContext &ui);
		float width = 3840.0f;
		float height = 2160.0f;
        bool m_ShowSettings = false;
        bool m_ResolutionChanged = false;
};
