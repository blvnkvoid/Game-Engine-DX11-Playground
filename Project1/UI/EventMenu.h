#pragma once
#include "../Events/EventList.h"
#include "../UI/UIContext.h"

class EventMenu
{
public:
	bool m_ShowEventMenu = false;
	void Draw(const UIContext& ui);
	EventSelection m_SelectedEvent = EventSelection::SundayCupTsukuba;
	EventLaunchType m_LaunchType = EventLaunchType::SingleEvent;
	ChampionshipSelection m_SelectedChampionship =
		ChampionshipSelection::SundayCup;
};