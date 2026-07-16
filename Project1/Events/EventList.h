#pragma once
#include <vector>

enum class EventSelection
{
    SundayCupTsukuba,
    SundayCupSpa,
    SundayCupTrialMountain
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

enum class ChampionshipSelection
{
    SundayCup,
    ClubmanCup,
    GTWorldChampionship
};

struct ChampionshipListEntry
{
    ChampionshipSelection selection;
    const char* displayName;
};

extern const std::vector<ChampionshipListEntry> g_ChampionshipList;

extern const std::vector<EventListEntry> g_EventList;