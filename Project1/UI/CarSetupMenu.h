#pragma once
#include "../Cars/CarSetup.h"
#include "../SharedVehicleTypes.h"

class CarSetupMenu
{
public:
	void Draw();
	bool m_ShowCarSetup = false;
	CarSetupState m_CarSetupState;
};