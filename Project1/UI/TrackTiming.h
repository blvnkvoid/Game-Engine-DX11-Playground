#pragma once
#include "../SharedTypes.h"
#include <bullet/btBulletDynamicsCommon.h>

struct TrackTimingEntry
{
    TrackSelection track;

    btVector3 startFinish;
    btVector3 sector1;
    btVector3 sector2;

    float radius = 15.0f;

};

extern const std::vector<TrackTimingEntry> g_TrackTimingTable;