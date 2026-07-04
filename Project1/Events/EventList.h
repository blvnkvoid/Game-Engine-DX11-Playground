#pragma once
#include <vector>

enum class EventSelection
{
    SundayCupTsukuba,
    SundayCupSpa
};

struct EventListEntry
{
    EventSelection selection;
    const char* displayName;
};

extern const std::vector<EventListEntry> g_EventList;