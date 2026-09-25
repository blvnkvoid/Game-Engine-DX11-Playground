    #pragma once

    #include <string>
    #include <vector>
    #include "../SharedTypes.h"

struct TrackRenderSettings
{
    float nearPlane = 1.0f;
    float farPlane = 1000.0f;

    float shadowForwardOffset = 350.0f;
    float shadowAreaSize = 800.0f;
    float lightDistance = 1500.0f;
    float shadowCameraNearClip = 50.0f;
    float shadowCameraFarClip = 2000.0f;

};


    struct TrackEntry
    {
        TrackSelection selection;
        std::string path;

        TrackRenderSettings renderSettings; 
    };

    extern std::vector<TrackEntry> g_TrackTable;