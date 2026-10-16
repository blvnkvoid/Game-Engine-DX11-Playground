#include "AudiR8.h"

VehicleDefinition R8::CreateDefinition()
{
    VehicleDefinition car;

    // ---------------------------------------------------------
    // 1. Mass / chassis
    // ---------------------------------------------------------
    car.mass = 1400.0f;
    car.frontWeightDistribution = 0.48f;

    car.yawInertiaScale = 1.10f;
    car.pitchInertiaScale = 1.00f;
    car.rollInertiaScale = 1.00f;
    car.yawDamping = 500.0f;
    car.bodyLength = 4.49f;
    car.Offset_X = 0.95f;
    car.Offset_Y = 0.15f;
    car.Offset_Z_Front = 1.378f;
    car.Offset_Z_Back = 1.272f;
    car.Offset_Z = car.Offset_Z_Front + car.Offset_Z_Back;

    car.wheelOffsets[0] = btVector3(-(car.Offset_X), -(car.Offset_Y), car.Offset_Z_Front);
    car.wheelOffsets[1] = btVector3(car.Offset_X, -(car.Offset_Y), car.Offset_Z_Front);
    car.wheelOffsets[2] = btVector3(-(car.Offset_X), -(car.Offset_Y), -(car.Offset_Z_Back));
    car.wheelOffsets[3] = btVector3(car.Offset_X, -(car.Offset_Y), -(car.Offset_Z_Back));

    // ---------------------------------------------------------
    // 2. Aero
    // ---------------------------------------------------------
    car.dragCoefficient = 0.34f;
    car.frontDownforceCoeff = 0.0f;
    car.rearDownforceCoeff = 0.0f;

    // ---------------------------------------------------------
    // 3. Engine
    // ---------------------------------------------------------
    car.engine.idleRPM = 900.0f;
    car.engine.redlineRPM = 8500.0f;
    car.engine.limiterRPM = 8600.0f;

    car.engine.maxTorque = 350.0f;
    car.engineBrakeTorque = 100.0f;

    car.engine.torqueCurveCount = 6;
    car.engine.torqueCurve[0] = { 1000.0f, 0.65f };
    car.engine.torqueCurve[1] = { 2500.0f, 0.90f };
    car.engine.torqueCurve[2] = { 4500.0f, 1.00f };
    car.engine.torqueCurve[3] = { 6500.0f, 1.00f };
    car.engine.torqueCurve[4] = { 8000.0f, 0.95f };
    car.engine.torqueCurve[5] = { 8500.0f, 0.85f };
    // ---------------------------------------------------------
    // 4. Gearbox / drivetrain
    // ---------------------------------------------------------
    car.frontTorqueDistribution = 0.0f;
    car.rearTorqueDistribution = 1.0f;

    car.gearbox.finalDrive = 3.30f;
    car.gearbox.gearCount = 9;

    car.gearbox.gearRatios[0] = -3.91f;
    car.gearbox.gearRatios[1] = 0.00f;
    car.gearbox.gearRatios[2] = 3.91f;
    car.gearbox.gearRatios[3] = 2.29f;
    car.gearbox.gearRatios[4] = 1.58f;
    car.gearbox.gearRatios[5] = 1.18f;
    car.gearbox.gearRatios[6] = 0.94f;
    car.gearbox.gearRatios[7] = 0.79f;
    car.gearbox.gearRatios[8] = 0.62f;

    // ---------------------------------------------------------
    // 5. Differential
    // ---------------------------------------------------------
    car.lockStrength = 160.0f;
    car.preload = 180.0f;
    car.diffMaxLockTorque = 900.0f;

    car.diffLoadBiasStrength = 800.0f;
    car.diffMaxLoadBiasTorque = 300.0f;
    // ---------------------------------------------------------
    // 6. Steering / response
    // ---------------------------------------------------------
    car.lowSpeedSteerAngle = 0.50f;
    car.highSpeedSteerAngle = 0.25f;

    car.loadResponseRate = 50.0f;
    car.slipResponseRate = 55.0f;
    car.loadTransferStrength = 0.85f;

    // ---------------------------------------------------------
    // 7. Brakes
    // ---------------------------------------------------------
    car.brakeBiasFront = 0.70f;
    car.brakeBiasRear = 0.30f;

    car.brakeTorqueStrength = 3500.0f;
    car.brakeForceStrength = 5000.0f;

    // ---------------------------------------------------------
    // 8. Suspension
    // ---------------------------------------------------------
    car.suspensionRestLength = 0.6f;

    car.frontARBStiffness = 5500.0f;
    car.rearARBStiffness = 6000.0f;

    car.suspension.frontSpringRate = 42000.0f;
    car.suspension.rearSpringRate = 45000.0f;

    car.suspension.frontSlowBump = 2725.0f;
    car.suspension.frontFastBump = 1805.0f;
    car.suspension.frontFastBumpThreshold = 0.102f;
    car.suspension.frontRebound = 5415.0f;
    car.suspension.frontFastRebound = 3380.0f;
    car.suspension.frontFastReboundThreshold = 0.122f;

    car.suspension.rearSlowBump = 3400.0f;
    car.suspension.rearFastBump = 2300.0f;
    car.suspension.rearFastBumpThreshold = 0.102f;
    car.suspension.rearRebound = 7000.0f;
    car.suspension.rearFastRebound = 4200.0f;
    car.suspension.rearFastReboundThreshold = 0.122f;

    car.suspension.bumpStopRate = 70000.0f;
    car.suspension.bumpStopStart = 0.75f;
    car.suspension.dampingScale = 1.00f;

    // ---------------------------------------------------------
    // 9. Tyres
    // ---------------------------------------------------------


    ApplyTyreCompound(car.frontTyres, TyreCompound::N3_Road);

    car.frontTyres.staticCamber = -4.0f;
    car.frontTyres.camberGripStrength = 0.08f;
    car.frontTyres.CamberGain = -12.0f;
    car.frontTyres.wheelRadius = 0.34f;
    car.frontTyres.frontWheelInertia = 1.85f;


    ApplyTyreCompound(car.rearTyres, TyreCompound::N1_Economic);

    car.rearTyres.staticCamber = -1.0f;
    car.rearTyres.camberGripStrength = 0.08f;
    car.rearTyres.CamberGain = -12.0f;
    car.rearTyres.wheelRadius = 0.345f;
    car.rearTyres.rearWheelInertia = 1.95f;

    car.audio.basePath = "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine-DX11-Playground\\Project1\\Cars\\Audi_R8_LMS\\sounds\\";

    car.audio.idle = "idle_1372.wav";

    car.audio.lowOn = "2761e.wav";
    car.audio.midOn = "4974b.wav";
    car.audio.highOn = "6686a.wav";
    car.audio.topFull = "7881f.wav";

    car.audio.limiter = "limiter.wav";

    car.audio.transmission = "coast4.wav";
    car.audio.gearUp = "gearupEXT.wav";
    car.audio.gearDown = "geardnEXT.wav";

    car.audio.tyreRolling = "tyre_rolling.wav";
    car.audio.tyreSkid = "skid_ext_mono.wav";
    car.audio.wind = "wind.wav";

    car.audio.idleRPM = 1372.0f;
    car.audio.lowRPM = 2761.0f;
    car.audio.midRPM = 4974.0f;
    car.audio.highRPM = 6686.0f;
    car.audio.topRPM = 8050.0f;

    car.audio.idleWidth = 1300.0f;
    car.audio.lowWidth = 1700.0f;
    car.audio.midWidth = 1900.0f;
    car.audio.highWidth = 1600.0f;
    car.audio.topWidth = 1600.0f;
    return car;
}


CameraDefinition R8::CreateCameraDefinition()
{
    CameraDefinition camera;

    camera.chaseHeight = 3.0f;
    camera.chaseDistance = -32.0f;
    camera.chasePitchDeg = 15.0f;

    camera.roofHeight = 1.00f;
    camera.roofDistance = -0.5f;
    camera.roofPitchDeg = 0.0f;

    camera.bumperHeight = 0.7f;
    camera.bumperDistance = 4.5f;
    camera.bumperPitchDeg = 5.0f;

    camera.cockpitHeight = 0.3f;
    camera.cockpitDistance = -0.8f;
    camera.cockpitPitchDeg = 5.0f;

    camera.cockpitOffsetX = -0.3f;
    camera.cockpitOffsetY = 0.0f;
    camera.cockpitOffsetZ = 0.0f;

    return camera;
}