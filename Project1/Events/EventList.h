#pragma once
#include <vector>

enum class EventSelection
{
    SundayCupTsukuba,
    SundayCupSpa
};

enum class EventLaunchType
{
    SingleEvent,
    Championship
};

struct EventListEntry
{
    EventSelection selection;
    const char* displayName;
};

struct ChampionshipListEntry
{
    EventSelection firstEvent;
    const char* displayName;
};

extern const std::vector<ChampionshipListEntry> g_ChampionshipList;

extern const std::vector<EventListEntry> g_EventList;