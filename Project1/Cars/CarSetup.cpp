#include "CarSetup.h"


void ApplySetup(VehicleDefinition& car, const CarSetupState& setup)
{
    car.gearbox.finalDrive *= setup.finalDriveMultiplier;

    if (setup.brakeBiasFront >= 0.0f)
    {
        car.brakeBiasFront = setup.brakeBiasFront;
        car.brakeBiasRear = 1.0f - setup.brakeBiasFront;
    }

    car.frontARBStiffness *= setup.frontARBMultiplier;
    car.rearARBStiffness *= setup.rearARBMultiplier;

    car.suspension.frontSpringRate *= setup.frontSpringMultiplier;
    car.suspension.rearSpringRate *= setup.rearSpringMultiplier;

    car.frontDownforceCoeff *= setup.frontDownforceMultiplier;
    car.rearDownforceCoeff *= setup.rearDownforceMultiplier;

}