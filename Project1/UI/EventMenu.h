#pragma once
#include "../Events/EventList.h"

class EventMenu
{
public:
	bool m_ShowEventMenu = false;
	void Draw();
	EventSelection m_SelectedEvent;
};