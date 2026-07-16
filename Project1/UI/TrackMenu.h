#pragma once
#include "../SharedTypes.h"
#include "../UI/UIContext.h"

class TrackMenu
{
public:
	bool m_TrackSelection = false;
	void Draw(const UIContext& ui);
	TrackSelection m_selectedTrack = TrackSelection::Spa;
};