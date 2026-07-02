#pragma once
#include "../SharedTypes.h"

class Garage
{
public:
	void Draw();
	bool m_ShowGarage = false; // Persistent state
	VehicleSelection m_PreviewSelection = VehicleSelection::PORSCHE_911;
};