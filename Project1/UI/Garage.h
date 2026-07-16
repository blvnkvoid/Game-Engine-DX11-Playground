#pragma once
#include "../SharedTypes.h"
#include "../UI/UIContext.h"

class Garage
{
public:
	void Draw(const UIContext& ui);
	bool m_ShowGarage = false; // Persistent state
	VehicleSelection m_PreviewSelection;
};