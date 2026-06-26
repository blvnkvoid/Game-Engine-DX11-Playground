#include "Mazda_Furai.h"

VehicleDefinition Furai::CreateDefinition()
{
    VehicleDefinition car;

    // ---------------------------------------------------------
    // 1. Mass / chassis
    // ---------------------------------------------------------
    car.mass = 675.0f;
    car.frontWeightDistribution = 0.45f;
    car.yawInertiaScale = 1.10f;
    car.pitchInertiaScale = 1.10f;
    car.rollInertiaScale = 1.10f;
    car.yawDamping = 500.0f;

    car.wheelOffsets[0] = btVector3(-0.88f, -0.14f, 1.48f);
    car.wheelOffsets[1] = btVector3(0.88f, -0.14f, 1.48f);
    car.wheelOffsets[2] = btVector3(-0.88f, -0.14f, -1.48f);
    car.wheelOffsets[3] = btVector3(0.88f, -0.14f, -1.48f);

    // ---------------------------------------------------------
    // 2. Aero
    // ---------------------------------------------------------
    car.dragCoefficient = 0.40f;
    car.frontDownforceCoeff = 0.145f;
    car.rearDownforceCoeff = 0.230f;

    // ---------------------------------------------------------
    // 3. Engine
    // ---------------------------------------------------------
    car.engine.idleRPM = 1200.0f;
    car.engine.redlineRPM = 9000.0f;
    car.engine.limiterRPM = 9200.0f;
    car.engine.maxTorque = 420.0f;
    car.engineBrakeTorque = 90.0f;

    car.engine.torqueCurveCount = 6;
    car.engine.torqueCurve[0] = { 1500.0f, 0.32f };
    car.engine.torqueCurve[1] = { 3500.0f, 0.58f };
    car.engine.torqueCurve[2] = { 5500.0f, 0.82f };
    car.engine.torqueCurve[3] = { 7500.0f, 1.00f };
    car.engine.torqueCurve[4] = { 8500.0f, 0.96f };
    car.engine.torqueCurve[5] = { 9000.0f, 0.88f };

    // ---------------------------------------------------------
    // 4. Gearbox / drivetrain
    // ---------------------------------------------------------
    car.frontTorqueDistribution = 0.0f;
    car.rearTorqueDistribution = 1.0f;

    car.gearbox.finalDrive = 4.30f;
    car.gearbox.gearCount = 8;

    car.gearbox.gearRatios[0] = -3.30f;
    car.gearbox.gearRatios[1] = 0.00f;
    car.gearbox.gearRatios[2] = 3.00f;
    car.gearbox.gearRatios[3] = 2.10f;
    car.gearbox.gearRatios[4] = 1.55f;
    car.gearbox.gearRatios[5] = 1.22f;
    car.gearbox.gearRatios[6] = 1.00f;
    car.gearbox.gearRatios[7] = 0.82f;
    car.gearbox.gearRatios[8] = 1.0f;

    // ---------------------------------------------------------
    // 5. Differential
    // ---------------------------------------------------------
    car.lockStrength = 75.0f;
    car.preload = 90.0f;
    car.diffMaxLockTorque = 500.0f;
    car.diffLoadBiasStrength = 420.0f;
    car.diffMaxLoadBiasTorque = 180.0f;

    // ---------------------------------------------------------
    // 6. Steering / response
    // ---------------------------------------------------------
    car.lowSpeedSteerAngle = 0.44f;
    car.highSpeedSteerAngle = 0.07f;

    car.loadResponseRate = 50.0f;
    car.slipResponseRate = 60.0f;
    car.loadTransferStrength = 0.48f;

    // ---------------------------------------------------------
    // 7. Brakes
    // ---------------------------------------------------------
    car.brakeBiasFront = 0.58f;
    car.brakeBiasRear = 0.42f;
    car.brakeTorqueStrength = 4300.0f;
    car.brakeForceStrength = 6200.0f;

    // ---------------------------------------------------------
    // 8. Suspension
    // ---------------------------------------------------------
    car.suspensionRestLength = 0.52f;

    car.frontARBStiffness = 9000.0f;
    car.rearARBStiffness = 7000.0f;

    car.suspension.frontSpringRate = 46000.0f;
    car.suspension.rearSpringRate = 52000.0f;

    car.suspension.frontSlowBump = 1900.0f;
    car.suspension.frontFastBump = 1200.0f;
    car.suspension.frontFastBumpThreshold = 1.0f;
    car.suspension.frontRebound = 3600.0f;
    car.suspension.frontFastRebound = 2300.0f;
    car.suspension.frontFastReboundThreshold = 1.0f;

    car.suspension.rearSlowBump = 2300.0f;
    car.suspension.rearFastBump = 1500.0f;
    car.suspension.rearFastBumpThreshold = 1.0f;
    car.suspension.rearRebound = 4300.0f;
    car.suspension.rearFastRebound = 2700.0f;
    car.suspension.rearFastReboundThreshold = 1.0f;

    car.suspension.bumpStopRate = 70000.0f;
    car.suspension.bumpStopStart = 0.85f;
    car.suspension.dampingScale = 0.60f;

    // ---------------------------------------------------------
    // 9. Tyres
    // ---------------------------------------------------------
    car.frontTyres.frictionCoeff = 1.65f;
    car.frontTyres.loadSmoothing = 10.0f;
    car.frontTyres.relaxationRate = 34.0f;
    car.frontTyres.stiffness = 22500.0f;
    car.frontTyres.staticCamber = -2.4f;
    car.frontTyres.camberGripStrength = 0.10f;
    car.frontTyres.CamberGain = -17.0f;
    car.frontTyres.wheelRadius = 0.33f;
    car.frontTyres.frontWheelInertia = 1.20f;

    car.rearTyres.frictionCoeff = 1.70f;
    car.rearTyres.loadSmoothing = 10.0f;
    car.rearTyres.relaxationRate = 31.0f;
    car.rearTyres.stiffness = 24000.0f;
    car.rearTyres.staticCamber = -2.8f;
    car.rearTyres.camberGripStrength = 0.10f;
    car.rearTyres.CamberGain = -19.0f;
    car.rearTyres.wheelRadius = 0.335f;
    car.rearTyres.rearWheelInertia = 1.30f;


    car.audio.basePath = "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine\\Project1\\Cars\\Mazda_Furai\\sounds\\";

    car.audio.idle =
        "exh_idl_vn2.wav";

    car.audio.lowOn =
        "exholow.wav";

    car.audio.midOn =
        "exhomid.wav";

    car.audio.highOn =
        "exhomidhigh.wav";

    car.audio.topFull =
        "exhohigh.wav";

    car.audio.limiter =
        "int_limit.wav";

    car.audio.transmission =
        "shcrk1.wav";

    car.audio.gearUp =
        "up1.wav";

    car.audio.gearDown =
        "dn1.wav";

    car.audio.tyreRolling =
        "tyre_rolling.wav";

    car.audio.tyreSkid =
        "skidhigh.wav";

    car.audio.wind =
        "hi_nw.wav";

    car.audio.idleRPM = 1500.0f;
    car.audio.lowRPM = 3500.0f;
    car.audio.midRPM = 5500.0f;
    car.audio.highRPM = 7500.0f;
    car.audio.topRPM = 9000.0f;

    car.audio.idleWidth = 1400.0f;
    car.audio.lowWidth = 1900.0f;
    car.audio.midWidth = 2100.0f;
    car.audio.highWidth = 1900.0f;
    car.audio.topWidth = 1700.0f;
    return car;
}


CameraDefinition Furai::CreateCameraDefinition()
{
    CameraDefinition camera;

    camera.chaseHeight = 3.0f;
    camera.chaseDistance = -32.0f;
    camera.chasePitchDeg = 15.0f;

    camera.roofHeight = 1.3f;
    camera.roofDistance = -2.5f;
    camera.roofPitchDeg = 0.0f;

    camera.bumperHeight = 0.7f;
    camera.bumperDistance = 2.0f;
    camera.bumperPitchDeg = 5.0f;


    return camera;
}