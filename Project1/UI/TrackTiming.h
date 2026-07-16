#pragma once
#include "../SharedTypes.h"
#include <bullet/btBulletDynamicsCommon.h>
#include "../Graphics/MapLoader.h"

struct TrackTimingEntry
{
    TrackSelection track;

    btVector3 startFinish;
    btVector3 sector1;
    btVector3 sector2;

    float radius = 15.0f;

};

TrackTimingEntry CreateTrackTiming(
    TrackSelection track,
    const std::vector<MapMarker>& markers);