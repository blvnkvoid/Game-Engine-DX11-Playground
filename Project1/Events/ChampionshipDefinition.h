#pragma once

#include <vector>
#include "EventList.h"

struct ChampionshipDefinition
{
    std::vector<EventSelection> events;
};

ChampionshipDefinition CreateSundayCup();