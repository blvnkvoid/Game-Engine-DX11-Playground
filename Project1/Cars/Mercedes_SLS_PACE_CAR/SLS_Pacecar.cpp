#include "SLS_Pacecar.h"

VehicleDefinition SLS_Pacecar::CreateDefinition()
{
    VehicleDefinition car;

    // ---------------------------------------------------------
    // 1. Mass / chassis
    // ---------------------------------------------------------
    car.mass = 1620.0f;
    car.frontWeightDistribution = 0.47f;

    car.yawInertiaScale = 1.08f;
    car.pitchInertiaScale = 1.03f;
    car.rollInertiaScale = 1.02f;

    // Current physics scale - old 0.92 value was from the old system.
    car.yawDamping = 5200.0f;

    car.wheelOffsets[0] = btVector3(-0.86f, -0.34f, 1.34f);
    car.wheelOffsets[1] = btVector3(0.86f, -0.34f, 1.34f);
    car.wheelOffsets[2] = btVector3(-0.86f, -0.34f, -1.32f);
    car.wheelOffsets[3] = btVector3(0.86f, -0.34f, -1.32f);

    // ---------------------------------------------------------
    // 2. Aero
    // ---------------------------------------------------------
    car.dragCoefficient = 0.37f;

    // Road car / pace car:
    // some stability from aero, nowhere near race-car territory.
    car.frontDownforceCoeff = 0.020f;
    car.rearDownforceCoeff = 0.040f;

    // ---------------------------------------------------------
    // 3. Engine
    // ---------------------------------------------------------
    car.engine.idleRPM = 900.0f;
    car.engine.redlineRPM = 7200.0f;
    car.engine.limiterRPM = 7400.0f;

    car.engine.maxTorque = 650.0f;
    car.engineBrakeTorque = 135.0f;

    car.engine.torqueCurveCount = 8;

    car.engine.torqueCurve[0] = { 1000.0f, 0.55f };
    car.engine.torqueCurve[1] = { 2000.0f, 0.76f };
    car.engine.torqueCurve[2] = { 3000.0f, 0.90f };
    car.engine.torqueCurve[3] = { 4000.0f, 0.98f };
    car.engine.torqueCurve[4] = { 4750.0f, 1.00f };
    car.engine.torqueCurve[5] = { 5500.0f, 0.98f };
    car.engine.torqueCurve[6] = { 6500.0f, 0.93f };
    car.engine.torqueCurve[7] = { 7200.0f, 0.82f };

    // ---------------------------------------------------------
    // 4. Gearbox / drivetrain
    // ---------------------------------------------------------
    car.frontTorqueDistribution = 0.0f;
    car.rearTorqueDistribution = 1.0f;

    car.gearbox.finalDrive = 3.67f;
    car.gearbox.gearCount = 7;

    // Same indexing convention as the rest of the current cars:
    // [0] reverse
    // [1] neutral
    // [2...] forward gears
    car.gearbox.gearRatios[0] = -3.42f;
    car.gearbox.gearRatios[1] = 0.00f;

    car.gearbox.gearRatios[2] = 4.38f;
    car.gearbox.gearRatios[3] = 2.86f;
    car.gearbox.gearRatios[4] = 1.92f;
    car.gearbox.gearRatios[5] = 1.37f;
    car.gearbox.gearRatios[6] = 1.00f;
    car.gearbox.gearRatios[7] = 0.82f;
    car.gearbox.gearRatios[8] = 0.73f;

    // ---------------------------------------------------------
    // 5. Differential
    // ---------------------------------------------------------
    // RWD road car with a useful amount of locking,
    // but not prototype/race-car levels.
    car.lockStrength = 75.0f;
    car.preload = 90.0f;

    car.diffMaxLockTorque = 950.0f;

    // Updated to the current physics scale.
    car.diffLoadBiasStrength = 600.0f;
    car.diffMaxLoadBiasTorque = 400.0f;

    // ---------------------------------------------------------
    // 6. Steering / response
    // ---------------------------------------------------------
    car.lowSpeedSteerAngle = 0.55f;
    car.highSpeedSteerAngle = 0.10f;

    // Slower than the R10, but still responsive.
    car.loadResponseRate = 34.0f;
    car.slipResponseRate = 38.0f;

    car.loadTransferStrength = 0.72f;

    // ---------------------------------------------------------
    // 7. Brakes
    // ---------------------------------------------------------
    car.brakeBiasFront = 0.64f;
    car.brakeBiasRear = 0.36f;

    car.brakeTorqueStrength = 4200.0f;
    car.brakeForceStrength = 6200.0f;

    // ---------------------------------------------------------
    // 8. Suspension
    // ---------------------------------------------------------
    car.suspensionRestLength = 0.50f;

    // Fairly stiff performance road-car chassis.
    // Rear remains softer in roll to keep the RWD car manageable.
    car.frontARBStiffness = 9800.0f;
    car.rearARBStiffness = 7600.0f;

    car.suspension.frontSpringRate = 54000.0f;
    car.suspension.rearSpringRate = 50000.0f;

    // Front damping
    car.suspension.frontSlowBump = 2800.0f;
    car.suspension.frontFastBump = 1800.0f;
    car.suspension.frontFastBumpThreshold = 0.11f;

    car.suspension.frontRebound = 5000.0f;
    car.suspension.frontFastRebound = 3100.0f;
    car.suspension.frontFastReboundThreshold = 0.13f;

    // Rear damping
    car.suspension.rearSlowBump = 2600.0f;
    car.suspension.rearFastBump = 1700.0f;
    car.suspension.rearFastBumpThreshold = 0.11f;

    car.suspension.rearRebound = 4700.0f;
    car.suspension.rearFastRebound = 2950.0f;
    car.suspension.rearFastReboundThreshold = 0.13f;

    car.suspension.bumpStopRate = 100000.0f;
    car.suspension.bumpStopStart = 0.84f;

    car.suspension.dampingScale = 0.72f;

    // ---------------------------------------------------------
    // 9. Tyres
    // ---------------------------------------------------------
    // Performance road tyres.
    //
    // IMPORTANT:
    // The old definition used stiffness values of 1.00 / 1.03.
    // Those belonged to the old physics scale and effectively
    // produced almost no useful lateral tyre force in the current
    // tyre model.

    // Front tyres
    car.frontTyres.frictionCoeff = 1.32f;
    car.frontTyres.loadSmoothing = 10.0f;
    car.frontTyres.relaxationRate = 31.0f;

    car.frontTyres.stiffness = 21000.0f;

    car.frontTyres.staticCamber = -1.4f;
    car.frontTyres.camberGripStrength = 0.09f;
    car.frontTyres.CamberGain = -13.0f;

    car.frontTyres.wheelRadius = 0.340f;
    car.frontTyres.frontWheelInertia = 1.55f;

    // Rear tyres
    car.rearTyres.frictionCoeff = 1.36f;
    car.rearTyres.loadSmoothing = 10.0f;
    car.rearTyres.relaxationRate = 29.0f;

    car.rearTyres.stiffness = 22000.0f;

    car.rearTyres.staticCamber = -1.8f;
    car.rearTyres.camberGripStrength = 0.09f;
    car.rearTyres.CamberGain = -14.0f;

    car.rearTyres.wheelRadius = 0.340f;
    car.rearTyres.rearWheelInertia = 1.70f;

    // ---------------------------------------------------------
    // 10. Audio
    // ---------------------------------------------------------
    car.audio.basePath =
        "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine-DX11-Playground\\Project1\\Cars\\Mercedes_SLS_PACE_CAR\\sounds\\";

    car.audio.idle =
        "bruit_63.wav";

    car.audio.lowOn =
        "4-8_4_SLS_pot_Insert 3.wav";

    car.audio.midOn =
        "4-8_4_SLS_pot_Insert 6.wav";

    car.audio.highOn =
        "4-8_4_SLS_pot_Insert 10.wav";

    car.audio.topFull =
        "4-8_4_SLS_pot_Insert 13.wav";

    car.audio.limiter =
        "4-8_4_SLS_eff_Insert 13.wav";

    car.audio.transmission =
        "transmission.wav";

    car.audio.gearUp =
        "ferrari_458_shift1.wav";

    car.audio.gearDown =
        "ferrari_458_shift1.wav";

    car.audio.tyreRolling =
        "bruit.wav";

    car.audio.tyreSkid =
        "skid_ext_mono.wav";

    car.audio.wind =
        "wind.wav";

    car.audio.idleRPM = 900.0f;
    car.audio.lowRPM = 2000.0f;
    car.audio.midRPM = 4000.0f;
    car.audio.highRPM = 6000.0f;
    car.audio.topRPM = 7600.0f;

    car.audio.idleWidth = 900.0f;
    car.audio.lowWidth = 1600.0f;
    car.audio.midWidth = 1800.0f;
    car.audio.highWidth = 1800.0f;
    car.audio.topWidth = 1200.0f;

    return car;
}


CameraDefinition SLS_Pacecar::CreateCameraDefinition()
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

    camera.cockpitHeight = 0.35f;
    camera.cockpitDistance = -2.25f;
    camera.cockpitPitchDeg = 5.0f;

    camera.cockpitOffsetX = -0.425f;
    camera.cockpitOffsetY = 0.0f;
    camera.cockpitOffsetZ = 0.0f;

    return camera;
}