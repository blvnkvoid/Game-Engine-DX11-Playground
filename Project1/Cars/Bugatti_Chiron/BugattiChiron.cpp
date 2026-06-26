#include "BugattiChiron.h"

VehicleDefinition Chiron::CreateDefinition()
{
    VehicleDefinition car;

    car.mass = 1500.0f;

    car.suspensionRestLength = 0.6f;
    car.springStiffness = 50000.0f;

    car.frontARBStiffness = 12000.0f;
    car.rearARBStiffness = 4000.0f;

    car.brakeBiasFront = 0.57f;
    car.brakeBiasRear = 0.43f;

    car.brakeTorqueStrength = 3500.0f;
    car.brakeForceStrength = 5000.0f;


    car.engineBrakeTorque = 300.0f;

    car.loadResponseRate = 35.0f;
    car.slipResponseRate = 40.0f;

    car.lowSpeedSteerAngle = 0.65f;
    car.highSpeedSteerAngle = 0.08f;

    car.wheelOffsets[0] = btVector3(-0.95f, -0.15f, 1.65f);
    car.wheelOffsets[1] = btVector3(0.95f, -0.15f, 1.65f);
    car.wheelOffsets[2] = btVector3(-0.95f, -0.15f, -1.65f);
    car.wheelOffsets[3] = btVector3(0.95f, -0.15f, -1.65f);

    return car;
}

CameraDefinition Chiron::CreateCameraDefinition()
{
    CameraDefinition camera;

    camera.chaseHeight = 3.0f;
    camera.chaseDistance = -32.0f;
    camera.chasePitchDeg = 15.0f;

    camera.roofHeight = 2.0f;
    camera.roofDistance = 1.0f;
    camera.roofPitchDeg = 0.0f;

    camera.bumperHeight = 0.7f;
    camera.bumperDistance = 8.2f;
    camera.bumperPitchDeg = 5.0f;


    return camera;
}