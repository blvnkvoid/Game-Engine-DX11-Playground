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
        { VehicleSelection::CIVIC, true },
        { VehicleSelection::MX5, false },
        { VehicleSelection::COPEN, false },
        { VehicleSelection::XSARA, false }
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
        { VehicleSelection::PORSCHE_911, true },
        { VehicleSelection::MX5, false },
        { VehicleSelection::COPEN, false },
        { VehicleSelection::XSARA, false }
    };

    return event;
}