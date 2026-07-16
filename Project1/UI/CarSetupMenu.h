#pragma once
#include "../Cars/CarSetup.h"
#include "../SharedVehicleTypes.h"
#include "../UI/UIContext.h"

class CarSetupMenu
{
public:
	void Draw(const UIContext& ui);
	bool m_ShowCarSetup = false;
	CarSetupState m_CarSetupState;
};