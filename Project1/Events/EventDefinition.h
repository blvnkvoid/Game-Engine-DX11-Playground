#pragma once
#include <vector>
#include "../SharedTypes.h"
#include "../Environment/EnvironmentDefinition.h"
#include "../Events/EventCarEntry.h"

struct EventDefinition
{
    TrackSelection track = TrackSelection::Tsukuba;
    EnvironmentDefinition environment;
    std::vector<EventCarEntry> cars;
};