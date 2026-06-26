#include "Mazda_MX5.h"

VehicleDefinition Mx5::CreateDefinition()
{
    VehicleDefinition car;

    // ---------------------------------------------------------
    // 1. Mass / chassis
    // ---------------------------------------------------------
    car.mass = 1050.0f;
    car.frontWeightDistribution = 0.51f;
    car.yawInertiaScale = 1.25f;
    car.pitchInertiaScale = 1.10f;
    car.rollInertiaScale = 1.10f;
    car.yawDamping = 2000.0f;

    car.wheelOffsets[0] = btVector3(-0.72f, -0.13f, 1.109f);
    car.wheelOffsets[1] = btVector3(0.72f, -0.13f, 1.109f);
    car.wheelOffsets[2] = btVector3(-0.72f, -0.13f, -1.13f);
    car.wheelOffsets[3] = btVector3(0.72f, -0.13f, -1.13f);

    // ---------------------------------------------------------
    // 2. Aero
    // ---------------------------------------------------------
    car.dragCoefficient = 0.38f;
    car.frontDownforceCoeff = 0.005f;
    car.rearDownforceCoeff = 0.008f;

    // ---------------------------------------------------------
    // 3. Engine
    // ---------------------------------------------------------
    car.engine.idleRPM = 850.0f;
    car.engine.redlineRPM = 7000.0f;
    car.engine.limiterRPM = 7200.0f;
    car.engine.maxTorque = 170.0f;
    car.engineBrakeTorque = 70.0f;

    car.engine.torqueCurveCount = 6;
    car.engine.torqueCurve[0] = { 1000.0f, 0.45f };
    car.engine.torqueCurve[1] = { 2500.0f, 0.70f };
    car.engine.torqueCurve[2] = { 4000.0f, 0.92f };
    car.engine.torqueCurve[3] = { 5500.0f, 1.00f };
    car.engine.torqueCurve[4] = { 6500.0f, 0.93f };
    car.engine.torqueCurve[5] = { 7000.0f, 0.82f };

    // ---------------------------------------------------------
    // 4. Gearbox / drivetrain
    // ---------------------------------------------------------
    car.frontTorqueDistribution = 0.0f;
    car.rearTorqueDistribution = 1.0f;

    car.gearbox.finalDrive = 4.10f;
    car.gearbox.gearCount = 7;

    car.gearbox.gearRatios[0] = -3.50f;
    car.gearbox.gearRatios[1] = 0.00f;
    car.gearbox.gearRatios[2] = 3.14f;
    car.gearbox.gearRatios[3] = 1.89f;
    car.gearbox.gearRatios[4] = 1.33f;
    car.gearbox.gearRatios[5] = 1.00f;
    car.gearbox.gearRatios[6] = 0.81f;
    car.gearbox.gearRatios[7] = 1.0f;
    car.gearbox.gearRatios[8] = 1.0f;

    // ---------------------------------------------------------
    // 5. Differential
    // ---------------------------------------------------------
    car.lockStrength = 35.0f;
    car.preload = 45.0f;
    car.diffMaxLockTorque = 220.0f;
    car.diffLoadBiasStrength = 180.0f;
    car.diffMaxLoadBiasTorque = 90.0f;

    // ---------------------------------------------------------
    // 6. Steering / response
    // ---------------------------------------------------------
    car.lowSpeedSteerAngle = 0.50f;
    car.highSpeedSteerAngle = 0.10f;

    car.loadResponseRate = 28.0f;
    car.slipResponseRate = 32.0f;
    car.loadTransferStrength = 0.85f;

    // ---------------------------------------------------------
    // 7. Brakes
    // ---------------------------------------------------------
    car.brakeBiasFront = 0.62f;
    car.brakeBiasRear = 0.38f;
    car.brakeTorqueStrength = 2600.0f;
    car.brakeForceStrength = 3600.0f;

    // ---------------------------------------------------------
    // 8. Suspension
    // ---------------------------------------------------------
    car.suspensionRestLength = 0.50f;

    car.frontARBStiffness = 4200.0f;
    car.rearARBStiffness = 3200.0f;

    car.suspension.frontSpringRate = 24000.0f;
    car.suspension.rearSpringRate = 22000.0f;

    car.suspension.frontSlowBump = 1100.0f;
    car.suspension.frontFastBump = 700.0f;
    car.suspension.frontFastBumpThreshold = 1.0f;
    car.suspension.frontRebound = 2100.0f;
    car.suspension.frontFastRebound = 1300.0f;
    car.suspension.frontFastReboundThreshold = 1.0f;

    car.suspension.rearSlowBump = 1000.0f;
    car.suspension.rearFastBump = 650.0f;
    car.suspension.rearFastBumpThreshold = 1.0f;
    car.suspension.rearRebound = 1900.0f;
    car.suspension.rearFastRebound = 1200.0f;
    car.suspension.rearFastReboundThreshold = 1.0f;

    car.suspension.bumpStopRate = 45000.0f;
    car.suspension.bumpStopStart = 0.80f;
    car.suspension.dampingScale = 0.55f;

    // ---------------------------------------------------------
    // 9. Tyres
    // ---------------------------------------------------------
    car.frontTyres.frictionCoeff = 1.25f;
    car.frontTyres.loadSmoothing = 10.0f;
    car.frontTyres.relaxationRate = 26.0f;
    car.frontTyres.stiffness = 15500.0f;
    car.frontTyres.staticCamber = -1.1f;
    car.frontTyres.camberGripStrength = 0.06f;
    car.frontTyres.CamberGain = -8.0f;
    car.frontTyres.wheelRadius = 0.30f;
    car.frontTyres.frontWheelInertia = 1.10f;

    car.rearTyres.frictionCoeff = 1.30f;
    car.rearTyres.loadSmoothing = 10.0f;
    car.rearTyres.relaxationRate = 23.0f;
    car.rearTyres.stiffness = 17000.0f;
    car.rearTyres.staticCamber = -1.4f;
    car.rearTyres.camberGripStrength = 0.06f;
    car.rearTyres.CamberGain = -10.0f;
    car.rearTyres.wheelRadius = 0.30f;
    car.rearTyres.rearWheelInertia = 1.15f;

    car.audio.basePath = "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine\\Project1\\Cars\\Mazda_MX5\\sounds\\";

    car.audio.idle =
        "kunos miata idle 850.wav";

    car.audio.lowOn =
        "miata straightpipe acc 2893.wav";

    car.audio.midOn =
        "miata straightpipe acc2 4341.wav";

    car.audio.highOn =
        "turbo miata on 5212.wav";

    car.audio.topFull =
        "turbo miata on 5907.wav";

    car.audio.limiter =
        "Miata Straightpipe limiter long loop.wav";

    car.audio.transmission =
        "transmission.wav";

    car.audio.gearUp =
        "gearupEXT.wav";

    car.audio.gearDown =
        "geardnEXT.wav";

    car.audio.tyreRolling =
        "tyre_rolling.wav";

    car.audio.tyreSkid =
        "skid_ext_mono.wav";

    car.audio.wind =
        "wind.wav";

    car.audio.idleRPM = 900.0f;
    car.audio.lowRPM = 2900.0f;
    car.audio.midRPM = 4340.0f;
    car.audio.highRPM = 5210.0f;
    car.audio.topRPM = 6900.0f;

    car.audio.idleWidth = 1200.0f;
    car.audio.lowWidth = 1800.0f;
    car.audio.midWidth = 1600.0f;
    car.audio.highWidth = 1200.0f;
    car.audio.topWidth = 1000.0f;

    return car;
}


CameraDefinition Mx5::CreateCameraDefinition()
{
    CameraDefinition camera;

    camera.chaseHeight = 3.0f;
    camera.chaseDistance = -32.0f;
    camera.chasePitchDeg = 15.0f;

    camera.roofHeight = 1.0f;
    camera.roofDistance = -2.0f;
    camera.roofPitchDeg = 0.0f;

    camera.bumperHeight = 0.7f;
    camera.bumperDistance = 1.7f;
    camera.bumperPitchDeg = 5.0f;



    return camera;
}