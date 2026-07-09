#include "EventDefinition.h"


EventDefinition CreateSundayCup_Tsukuba()
{
    EventDefinition event;
    event.track = TrackSelection::Tsukuba;
    event.environment.startTime = 60.0f;
    event.environment.dynamicTime = true;
    event.totalLaps = 1;
    event.cars =
    {
        { VehicleSelection::FURAI, true,0 },
        { VehicleSelection::MX5, false,1 },
        { VehicleSelection::COPEN, false,2 },
        { VehicleSelection::XSARA, false,3 }
    };

    return event;

}

EventDefinition CreateSundayCup_Spa()
{
    EventDefinition event;
    event.track = TrackSelection::Spa;
    event.environment.startTime = 180.0f;
    event.environment.dynamicTime = true;

    event.cars =
    {
        { VehicleSelection::CIVIC, true,0 },
        { VehicleSelection::MX5, false,1 },
        { VehicleSelection::COPEN, false,2 },
        { VehicleSelection::XSARA, false,3 }
    };

    return event;
}