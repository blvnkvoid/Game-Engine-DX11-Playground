#include "EventDefinition.h"


EventDefinition CreateSundayCup_Tsukuba()
{
    EventDefinition event;
    event.track = TrackSelection::Tsukuba;
    event.environment.startTime = 60.0f;
    event.environment.dynamicTime = true;
    event.totalLaps = 100;
    event.cars =
    {
        { VehicleSelection::FURAI, true,0 },
        { VehicleSelection::MX5,            false, 1 },
        { VehicleSelection::COPEN,          false, 2 },
        { VehicleSelection::XSARA,          false, 3 },
        { VehicleSelection::CIVIC,          false, 4 },
        { VehicleSelection::PORSCHE_911,    false, 5 },
        { VehicleSelection::AUDI_R8,        false, 6 },
        { VehicleSelection::JGTCSUPRA2000,  false, 7 },
        { VehicleSelection::JGTCNSX2000,    false, 8 },
        { VehicleSelection::GT500,          false, 9 },
        { VehicleSelection::MINOLTA,        false, 10 },
        { VehicleSelection::AUDI_R10,       false, 11 },
        { VehicleSelection::FURAI, false, 12 },
        { VehicleSelection::SLS_PACECAR,    false, 13 }
    };

    return event;

}


EventDefinition CreateSundayCup_Spa()
{
    EventDefinition event;
    event.track = TrackSelection::Spa;
    event.environment.startTime = 180.0f;
    event.environment.dynamicTime = true;
    event.totalLaps = 1;
    event.cars =
    {
        { VehicleSelection::CIVIC, true,0 },
        /*{ VehicleSelection::MX5, false,1 },
        { VehicleSelection::COPEN, false,2 },
        { VehicleSelection::XSARA, false,3 }*/
    };

    return event;
}

EventDefinition CreateSundayCup_TrialMountain()
{
    EventDefinition event;
    event.track = TrackSelection::TrialMountain;
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