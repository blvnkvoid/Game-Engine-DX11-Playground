#include "TrackTiming.h"
#include <stdexcept>

static const MapMarker& FindMarker(
    const std::vector<MapMarker>& markers,
    const std::string& name)
{
    for (const auto& marker : markers)
    {
        if (marker.name == name)
            return marker;
    }

    throw std::runtime_error("Missing timing marker: " + name);
}

TrackTimingEntry CreateTrackTiming(
    TrackSelection track,
    const std::vector<MapMarker>& markers)
{
    const auto& start = FindMarker(markers, "TIMING_START");
    const auto& sector1 = FindMarker(markers, "TIMING_SECTOR_1");
    const auto& sector2 = FindMarker(markers, "TIMING_SECTOR_2");

    return
    {
        track,
        btVector3(start.x, start.y, start.z),
        btVector3(sector1.x, sector1.y, sector1.z),
        btVector3(sector2.x, sector2.y, sector2.z),
        15.0f
    };
}