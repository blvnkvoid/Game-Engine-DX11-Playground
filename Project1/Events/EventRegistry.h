#pragma once

#include "EventDefinition.h"
#include <stdexcept>
#include "EventList.h"
#include "EventSession.h"

class EventRegistry
{
public:
	EventDefinition Create(EventSelection selection);
	static const std::vector<EventListEntry> g_EventList;

	EventSession CreateSession(
		EventLaunchType launchType,
		EventSelection selectedEvent);
};