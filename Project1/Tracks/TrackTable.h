    #pragma once

    #include <string>
    #include <vector>
    #include "../SharedTypes.h"

    struct TrackEntry
    {
        TrackSelection selection;
        std::string path;
    };

    extern const std::vector<TrackEntry> g_TrackTable;