#include "Toyota_Supra_JGTC_2000.h"

VehicleDefinition SUPRA_JGTC2000::CreateDefinition()
{
    VehicleDefinition car;
    // ---------------------------------------------------------
    // 1. Mass / chassis
    // ---------------------------------------------------------
    car.mass = 1100.0f;
    car.frontWeightDistribution = 0.51f;
    car.yawInertiaScale = 1.20f;
    car.pitchInertiaScale = 1.10f;
    car.rollInertiaScale = 1.10f;
    car.yawDamping = 5000.0f;

    car.wheelOffsets[0] = btVector3(-0.800f, -0.15f, 1.250f);
    car.wheelOffsets[1] = btVector3(0.800f, -0.15f, 1.250f);
    car.wheelOffsets[2] = btVector3(-0.795f, -0.15f, -1.301f);
    car.wheelOffsets[3] = btVector3(0.795f, -0.15f, -1.301f);

    // ---------------------------------------------------------
    // 2. Aero
    // ---------------------------------------------------------
    car.dragCoefficient = 0.42f;
    car.frontDownforceCoeff = 0.120f;
    car.rearDownforceCoeff = 0.180f;

    // ---------------------------------------------------------
    // 3. Engine
    // ---------------------------------------------------------
    car.engine.idleRPM = 900.0f;
    car.engine.redlineRPM = 8500.0f;
    car.engine.limiterRPM = 8600.0f;
    car.engine.maxTorque = 650.0f;
    car.engineBrakeTorque = 140.0f;

    car.engine.torqueCurveCount = 6;
    car.engine.torqueCurve[0] = { 1000.0f, 0.30f };
    car.engine.torqueCurve[1] = { 2500.0f, 0.48f };
    car.engine.torqueCurve[2] = { 4500.0f, 0.78f };
    car.engine.torqueCurve[3] = { 6500.0f, 1.00f };
    car.engine.torqueCurve[4] = { 7800.0f, 0.96f };
    car.engine.torqueCurve[5] = { 8500.0f, 0.88f };

    // ---------------------------------------------------------
    // 4. Gearbox / drivetrain
    // ---------------------------------------------------------
    car.frontTorqueDistribution = 0.0f;
    car.rearTorqueDistribution = 1.0f;

    car.gearbox.finalDrive = 4.10f;
    car.gearbox.gearCount = 9;

    car.gearbox.gearRatios[0] = -3.91f;
    car.gearbox.gearRatios[1] = 0.00f;
    car.gearbox.gearRatios[2] = 3.10f;
    car.gearbox.gearRatios[3] = 2.25f;
    car.gearbox.gearRatios[4] = 1.72f;
    car.gearbox.gearRatios[5] = 1.37f;
    car.gearbox.gearRatios[6] = 1.12f;
    car.gearbox.gearRatios[7] = 0.93f;
    car.gearbox.gearRatios[8] = 0.78f;

    // ---------------------------------------------------------
    // 5. Differential
    // ---------------------------------------------------------
    car.lockStrength = 120.0f;
    car.preload = 180.0f;
    car.diffMaxLockTorque = 850.0f;
    car.diffLoadBiasStrength = 800.0f;
    car.diffMaxLoadBiasTorque = 350.0f;

    // ---------------------------------------------------------
    // 6. Steering / response
    // ---------------------------------------------------------
    car.lowSpeedSteerAngle = 0.42f;
    car.highSpeedSteerAngle = 0.06f;

    car.loadResponseRate = 45.0f;
    car.slipResponseRate = 55.0f;
    car.loadTransferStrength = 0.65f;

    // ---------------------------------------------------------
    // 7. Brakes
    // ---------------------------------------------------------
    car.brakeBiasFront = 0.60f;
    car.brakeBiasRear = 0.40f;
    car.brakeTorqueStrength = 5000.0f;
    car.brakeForceStrength = 7000.0f;

    // ---------------------------------------------------------
    // 8. Suspension
    // ---------------------------------------------------------
    car.suspensionRestLength = 0.6f;

    car.frontARBStiffness = 12000.0f;
    car.rearARBStiffness = 10500.0f;

    car.suspension.frontSpringRate = 52000.0f;
    car.suspension.rearSpringRate = 58000.0f;

    car.suspension.frontSlowBump = 2200.0f;
    car.suspension.frontFastBump = 1400.0f;
    car.suspension.frontFastBumpThreshold = 1.0f;
    car.suspension.frontRebound = 4200.0f;
    car.suspension.frontFastRebound = 2600.0f;
    car.suspension.frontFastReboundThreshold = 1.0f;

    car.suspension.rearSlowBump = 2600.0f;
    car.suspension.rearFastBump = 1700.0f;
    car.suspension.rearFastBumpThreshold = 1.0f;
    car.suspension.rearRebound = 5000.0f;
    car.suspension.rearFastRebound = 3100.0f;
    car.suspension.rearFastReboundThreshold = 1.0f;

    car.suspension.bumpStopRate = 80000.0f;
    car.suspension.bumpStopStart = 0.85f;
    car.suspension.dampingScale = 0.65f;

    // ---------------------------------------------------------
    // 9. Tyres
    // ---------------------------------------------------------
    car.frontTyres.frictionCoeff = 1.65f;
    car.frontTyres.loadSmoothing = 10.0f;
    car.frontTyres.relaxationRate = 34.0f;
    car.frontTyres.stiffness = 22500.0f;
    car.frontTyres.staticCamber = -2.7f;
    car.frontTyres.camberGripStrength = 0.10f;
    car.frontTyres.CamberGain = -18.0f;
    car.frontTyres.wheelRadius = 0.34f;
    car.frontTyres.frontWheelInertia = 1.45f;

    car.rearTyres.frictionCoeff = 1.70f;
    car.rearTyres.loadSmoothing = 10.0f;
    car.rearTyres.relaxationRate = 31.0f;
    car.rearTyres.stiffness = 24000.0f;
    car.rearTyres.staticCamber = -3.0f;
    car.rearTyres.camberGripStrength = 0.10f;
    car.rearTyres.CamberGain = -20.0f;
    car.rearTyres.wheelRadius = 0.345f;
    car.rearTyres.rearWheelInertia = 1.55f;

    // ---------------------------------------------------------
    // 10. Audio
    // ---------------------------------------------------------
    car.audio.basePath = "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine\\Project1\\Cars\\Toyota_Supra_JGTC_2000\\sounds\\";

    car.audio.idle =
        "962c_turbo_low2.wav";

    car.audio.lowOn =
        "3310c.wav";

    car.audio.midOn =
        "5571c.wav";

    car.audio.highOn =
        "7186b.wav";

    car.audio.topFull = "7748c_front.wav";


    car.audio.transmission =
        "transmission.wav";

    car.audio.gearUp =
        "gear_up.wav";

    car.audio.gearDown =
        "gear_dn.wav";

    car.audio.tyreRolling =
        "tyre_rolling.wav";

    car.audio.tyreSkid =
        "Skid.wav";

    car.audio.wind =
        "wind.wav";

    car.audio.idleRPM = 962.0f;
    car.audio.lowRPM = 3310.0f;
    car.audio.midRPM = 5571.0f;
    car.audio.highRPM = 7186.0f;
    car.audio.topRPM = 7748.0f;

    car.audio.idleWidth = 1800.0f;
    car.audio.lowWidth = 2200.0f;
    car.audio.midWidth = 2400.0f;
    car.audio.highWidth = 2200.0f;
    car.audio.topWidth = 2200.0f;
    return car;
}

CameraDefinition SUPRA_JGTC2000::CreateCameraDefinition()
{
    CameraDefinition camera;

    camera.chaseHeight = 3.5f;
    camera.chaseDistance = -35.0f;
    camera.chasePitchDeg = 15.0f;

    camera.roofHeight = 1.0f;
    camera.roofDistance = -1.5f;
    camera.roofPitchDeg = 0.0f;

    camera.bumperHeight = 0.7f;
    camera.bumperDistance = 3.5f;
    camera.bumperPitchDeg = 5.0f;   


    return camera;
}