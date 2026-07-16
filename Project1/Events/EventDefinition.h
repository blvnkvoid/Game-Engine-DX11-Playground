#pragma once
#include <vector>
#include "../SharedTypes.h"
#include "../Environment/EnvironmentDefinition.h"
#include "../Events/EventCarEntry.h"

struct EventDefinition
{
    TrackSelection track;
    EnvironmentDefinition environment;

    int totalLaps;
    std::vector<EventCarEntry> cars;
};

EventDefinition CreateSundayCup_Tsukuba();
EventDefinition CreateSundayCup_Spa();
EventDefinition CreateSundayCup_TrialMountain();