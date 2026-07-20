    #pragma once

    #include <string>
    #include <vector>
    #include "../SharedTypes.h"

struct TrackRenderSettings
{
    float nearPlane = 1.0f;
    float farPlane = 1000.0f;
};


    struct TrackEntry
    {
        TrackSelection selection;
        std::string path;

        TrackRenderSettings renderSettings; 
    };

    extern const std::vector<TrackEntry> g_TrackTable;