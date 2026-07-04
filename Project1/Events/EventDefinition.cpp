#include "EventDefinition.h"


EventDefinition CreateSundayCup_Tsukuba()
{
    EventDefinition event;
    event.track = TrackSelection::Tsukuba;
    event.environment.startTime = 90.0f;
    event.environment.dynamicTime = false;

    event.cars =
    {
        { VehicleSelection::CIVIC, true },
        { VehicleSelection::MX5, false },
        { VehicleSelection::COPEN, false },
        { VehicleSelection::XSARA, false }
    };

    return event;
}