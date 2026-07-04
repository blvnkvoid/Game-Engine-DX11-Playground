#pragma once
#include <vector>
#include "../SharedTypes.h"
#include "../Environment/EnvironmentDefinition.h"
#include "../Events/EventCarEntry.h"

struct EventDefinition
{
    TrackSelection track;
    EnvironmentDefinition environment;
    std::vector<EventCarEntry> cars;
};

EventDefinition CreateSundayCup_Tsukuba();
EventDefinition CreateSundayCup_Spa();