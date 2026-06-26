#include "AudiR10.h"

VehicleDefinition R10::CreateDefinition()
{
    VehicleDefinition car;

    // ---------------------------------------------------------
    // 1. Mass / chassis
    // ---------------------------------------------------------
    car.mass = 925.0f;
    car.frontWeightDistribution = 0.46f;
    car.yawInertiaScale = 1.0f;
    car.pitchInertiaScale = 1.0f;
    car.rollInertiaScale = 1.0f;
    car.yawDamping = 6000.0f;

    car.wheelOffsets[0] = btVector3(-0.96f, -0.14f, 1.52f);
    car.wheelOffsets[1] = btVector3(0.96f, -0.14f, 1.52f);
    car.wheelOffsets[2] = btVector3(-0.96f, -0.14f, -1.48f);
    car.wheelOffsets[3] = btVector3(0.96f, -0.14f, -1.48f);

    // ---------------------------------------------------------
    // 2. Aero
    // ---------------------------------------------------------
    car.dragCoefficient = 0.32f;
    car.frontDownforceCoeff = 0.170f;
    car.rearDownforceCoeff = 0.270f;

    // ---------------------------------------------------------
    // 3. Engine
    // ---------------------------------------------------------
    car.engine.idleRPM = 900.0f;
    car.engine.redlineRPM = 5200.0f;
    car.engine.limiterRPM = 5400.0f;
    car.engine.maxTorque = 1150.0f;
    car.engineBrakeTorque = 260.0f;

    car.engine.torqueCurveCount = 6;
    car.engine.torqueCurve[0] = { 1000.0f, 0.48f };
    car.engine.torqueCurve[1] = { 1800.0f, 0.82f };
    car.engine.torqueCurve[2] = { 2500.0f, 1.00f };
    car.engine.torqueCurve[3] = { 3500.0f, 0.96f };
    car.engine.torqueCurve[4] = { 4500.0f, 0.82f };
    car.engine.torqueCurve[5] = { 5200.0f, 0.62f };

    // ---------------------------------------------------------
    // 4. Gearbox / drivetrain
    // ---------------------------------------------------------
    car.frontTorqueDistribution = 0.0f;
    car.rearTorqueDistribution = 1.0f;

    car.gearbox.finalDrive = 2.40f;
    car.gearbox.gearCount = 8;

    car.gearbox.gearRatios[0] = -3.10f;
    car.gearbox.gearRatios[1] = 0.00f;
    car.gearbox.gearRatios[2] = 2.85f;
    car.gearbox.gearRatios[3] = 1.92f;
    car.gearbox.gearRatios[4] = 1.42f;
    car.gearbox.gearRatios[5] = 1.12f;
    car.gearbox.gearRatios[6] = 0.92f;
    car.gearbox.gearRatios[7] = 0.76f;
    car.gearbox.gearRatios[8] = 1.0f;

    // ---------------------------------------------------------
    // 5. Differential
    // ---------------------------------------------------------
    car.lockStrength = 140.0f;
    car.preload = 260.0f;
    car.diffMaxLockTorque = 1050.0f;
    car.diffLoadBiasStrength = 850.0f;
    car.diffMaxLoadBiasTorque = 420.0f;

    // ---------------------------------------------------------
    // 6. Steering / response
    // ---------------------------------------------------------
    car.lowSpeedSteerAngle = 0.40f;
    car.highSpeedSteerAngle = 0.055f;

    car.loadResponseRate = 48.0f;
    car.slipResponseRate = 58.0f;
    car.loadTransferStrength = 0.55f;

    // ---------------------------------------------------------
    // 7. Brakes
    // ---------------------------------------------------------
    car.brakeBiasFront = 0.59f;
    car.brakeBiasRear = 0.41f;
    car.brakeTorqueStrength = 5200.0f;
    car.brakeForceStrength = 7600.0f;

    // ---------------------------------------------------------
    // 8. Suspension
    // ---------------------------------------------------------
    car.suspensionRestLength = 0.50f;

    car.frontARBStiffness = 11500.0f;
    car.rearARBStiffness = 9800.0f;

    car.suspension.frontSpringRate = 56000.0f;
    car.suspension.rearSpringRate = 64000.0f;

    car.suspension.frontSlowBump = 2300.0f;
    car.suspension.frontFastBump = 1500.0f;
    car.suspension.frontFastBumpThreshold = 0.102f;
    car.suspension.frontRebound = 4500.0f;
    car.suspension.frontFastRebound = 2800.0f;
    car.suspension.frontFastReboundThreshold = 0.122f;

    car.suspension.rearSlowBump = 2800.0f;
    car.suspension.rearFastBump = 1800.0f;
    car.suspension.rearFastBumpThreshold = 0.102f;
    car.suspension.rearRebound = 5400.0f;
    car.suspension.rearFastRebound = 3400.0f;
    car.suspension.rearFastReboundThreshold = 0.122f;

    car.suspension.bumpStopRate = 90000.0f;
    car.suspension.bumpStopStart = 0.86f;
    car.suspension.dampingScale = 0.68f;

    // ---------------------------------------------------------
    // 9. Tyres
    // ---------------------------------------------------------
    car.frontTyres.frictionCoeff = 1.72f;
    car.frontTyres.loadSmoothing = 10.0f;
    car.frontTyres.relaxationRate = 38.0f;
    car.frontTyres.stiffness = 23500.0f;
    car.frontTyres.staticCamber = -2.8f;
    car.frontTyres.camberGripStrength = 0.10f;
    car.frontTyres.CamberGain = -18.0f;
    car.frontTyres.wheelRadius = 0.34f;
    car.frontTyres.frontWheelInertia = 1.35f;

    car.rearTyres.frictionCoeff = 1.78f;
    car.rearTyres.loadSmoothing = 10.0f;
    car.rearTyres.relaxationRate = 32.0f;
    car.rearTyres.stiffness = 25000.0f;
    car.rearTyres.staticCamber = -3.1f;
    car.rearTyres.camberGripStrength = 0.10f;
    car.rearTyres.CamberGain = -20.0f;
    car.rearTyres.wheelRadius = 0.345f;
    car.rearTyres.rearWheelInertia = 1.55f;


    car.audio.basePath = "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine\\Project1\\Cars\\Audi_R10\\sounds\\";

    car.audio.idle =
        "1 EngA_01076.wav";

    car.audio.lowOn =
        "3 EngA_01671.wav";

    car.audio.midOn =
        "7 EngA_03373.wav";

    car.audio.highOn =
        "11 EngA_05416.wav";

    car.audio.topFull =
        "15 EngA_08150.wav";

    car.audio.limiter =
        "V12NM_Aventador_HighBurb_08.wav";

    car.audio.transmission =
        "transmission.wav";

    car.audio.gearUp =
        "shifting loud.wav";

    car.audio.gearDown =
        "shifting loud4.wav";

    car.audio.tyreRolling =
        "roadnoisedamplow1.wav";

    car.audio.tyreSkid =
        "skid_asphalt_lat_2_1.wav";

    car.audio.wind =
        "wind.wav";

    car.audio.idleRPM = 1076.0f;
    car.audio.lowRPM = 1671.0f;
    car.audio.midRPM = 3373.0f;
    car.audio.highRPM = 5416.0f;
    car.audio.topRPM = 8150.0f;

    car.audio.idleWidth = 900.0f;
    car.audio.lowWidth = 1100.0f;
    car.audio.midWidth = 1700.0f;
    car.audio.highWidth = 1900.0f;
    car.audio.topWidth = 1600.0f;
    return car;
}

CameraDefinition R10::CreateCameraDefinition()
{
    CameraDefinition camera;

    camera.chaseHeight = 3.0f;
    camera.chaseDistance = -32.0f;
    camera.chasePitchDeg = 15.0f;

    camera.roofHeight = 1.0f;
    camera.roofDistance = -3.0f;
    camera.roofPitchDeg = 0.0f;

    camera.bumperHeight = 0.7f;
    camera.bumperDistance = 2.2f;
    camera.bumperPitchDeg = 5.0f;


    return camera;
}