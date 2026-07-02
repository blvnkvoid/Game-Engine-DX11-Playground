#pragma once
#include "../SharedTypes.h"

class TrackMenu
{
public:
	bool m_TrackSelection = false;
	void Draw();
	TrackSelection m_selectedTrack = TrackSelection::Spa;
};