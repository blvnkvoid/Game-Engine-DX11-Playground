    #include "Copen.h"

VehicleDefinition Copen::CreateDefinition()
{
    VehicleDefinition car;

    // ---------------------------------------------------------
    // 1. Mass / chassis
    // ---------------------------------------------------------
    car.mass = 850.0f;
    car.frontWeightDistribution = 0.63f;
    car.yawInertiaScale = 1.1f;
    car.pitchInertiaScale = 1.10f;
    car.rollInertiaScale = 1.10f;
    car.yawDamping = 1500.0f;

    car.wheelOffsets[0] = btVector3(-0.65f, -0.12f, 0.825f);
    car.wheelOffsets[1] = btVector3(0.65f, -0.12f, 0.825f);
    car.wheelOffsets[2] = btVector3(-0.645f, -0.12f, -1.404f);
    car.wheelOffsets[3] = btVector3(0.645f, -0.12f, -1.404f);

    // ---------------------------------------------------------
    // 2. Aero
    // ---------------------------------------------------------
    car.dragCoefficient = 0.38f;
    car.frontDownforceCoeff = 0.050f;
    car.rearDownforceCoeff = 0.050f;

    // ---------------------------------------------------------
    // 3. Engine
    // ---------------------------------------------------------
    car.engine.idleRPM = 1100.0f;
    car.engine.redlineRPM = 9500.0f;
    car.engine.limiterRPM = 9500.0f;
    car.engine.maxTorque = 105.0f;
    car.engineBrakeTorque = 55.0f;

    car.engine.torqueCurveCount = 7;
    car.engine.torqueCurve[0] = { 1100.0f, 0.32f };
    car.engine.torqueCurve[1] = { 2500.0f, 0.62f };
    car.engine.torqueCurve[2] = { 4000.0f, 0.88f };
    car.engine.torqueCurve[3] = { 5500.0f, 1.00f };
    car.engine.torqueCurve[4] = { 7000.0f, 0.94f };
    car.engine.torqueCurve[5] = { 8500.0f, 0.78f };
    car.engine.torqueCurve[6] = { 9300.0f, 0.58f };

    // ---------------------------------------------------------
    // 4. Gearbox / drivetrain
    // ---------------------------------------------------------
    car.frontTorqueDistribution = 1.0f;
    car.rearTorqueDistribution = 0.0f;

    car.gearbox.finalDrive = 4.90f;
    car.gearbox.gearCount = 7;

    car.gearbox.gearRatios[0] = -3.25f;
    car.gearbox.gearRatios[1] = 0.00f;
    car.gearbox.gearRatios[2] = 3.18f;
    car.gearbox.gearRatios[3] = 1.84f;
    car.gearbox.gearRatios[4] = 1.25f;
    car.gearbox.gearRatios[5] = 0.92f;
    car.gearbox.gearRatios[6] = 0.72f;
    car.gearbox.gearRatios[7] = 1.0f;
    car.gearbox.gearRatios[8] = 1.0f;

    // ---------------------------------------------------------
    // 5. Differential
    // ---------------------------------------------------------
    car.lockStrength = 35.0f;
    car.preload = 45.0f;
    car.diffMaxLockTorque = 180.0f;
    car.diffLoadBiasStrength = 160.0f;
    car.diffMaxLoadBiasTorque = 80.0f;

    // ---------------------------------------------------------
    // 6. Steering / response
    // ---------------------------------------------------------
    car.lowSpeedSteerAngle = 0.46f;
    car.highSpeedSteerAngle = 0.075f;

    car.loadResponseRate = 28.0f;
    car.slipResponseRate = 32.0f;
    car.loadTransferStrength = 0.68f;

    // ---------------------------------------------------------
    // 7. Brakes
    // ---------------------------------------------------------
    car.brakeBiasFront = 0.68f;
    car.brakeBiasRear = 0.32f;
    car.brakeTorqueStrength = 2100.0f;
    car.brakeForceStrength = 3200.0f;

    // ---------------------------------------------------------
    // 8. Suspension
    // ---------------------------------------------------------
    car.suspensionRestLength = 0.6f;

    car.frontARBStiffness = 4200.0f;
    car.rearARBStiffness = 2800.0f;

    car.suspension.frontSpringRate = 24000.0f;
    car.suspension.rearSpringRate = 22000.0f;

    car.suspension.frontSlowBump = 1450.0f;
    car.suspension.frontFastBump = 900.0f;
    car.suspension.frontFastBumpThreshold = 0.10f;
    car.suspension.frontRebound = 2600.0f;
    car.suspension.frontFastRebound = 1650.0f;
    car.suspension.frontFastReboundThreshold = 0.12f;

    car.suspension.rearSlowBump = 1250.0f;
    car.suspension.rearFastBump = 800.0f;
    car.suspension.rearFastBumpThreshold = 0.10f;
    car.suspension.rearRebound = 2300.0f;
    car.suspension.rearFastRebound = 1450.0f;
    car.suspension.rearFastReboundThreshold = 0.12f;

    car.suspension.bumpStopRate = 45000.0f;
    car.suspension.bumpStopStart = 0.78f;
    car.suspension.dampingScale = 0.58f;

    // ---------------------------------------------------------
    // 9. Tyres
    // ---------------------------------------------------------
    car.frontTyres.frictionCoeff = 1.08f;
    car.frontTyres.loadSmoothing = 9.0f;
    car.frontTyres.relaxationRate = 24.0f;
    car.frontTyres.stiffness = 10500.0f;
    car.frontTyres.staticCamber = -0.8f;
    car.frontTyres.camberGripStrength = 0.05f;
    car.frontTyres.CamberGain = -7.0f;
    car.frontTyres.wheelRadius = 0.285f;
    car.frontTyres.frontWheelInertia = 0.85f;

    car.rearTyres.frictionCoeff = 1.05f;
    car.rearTyres.loadSmoothing = 9.0f;
    car.rearTyres.relaxationRate = 22.0f;
    car.rearTyres.stiffness = 9500.0f;
    car.rearTyres.staticCamber = -0.6f;
    car.rearTyres.camberGripStrength = 0.05f;
    car.rearTyres.CamberGain = -5.0f;
    car.rearTyres.wheelRadius = 0.285f;
    car.rearTyres.rearWheelInertia = 0.75f;

    car.audio.basePath =
        "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine\\Project1\\Cars\\Daihatsu_Copen\\sounds\\";

    car.audio.idle = "1 EngB_01290.wav";

    car.audio.lowOn = "8 EngA_03951.wav";
    car.audio.midOn = "9 EngA_05031.wav";
    car.audio.highOn = "11 EngA_07200.wav";
    car.audio.topFull = "13 EngA_09420.wav";

    car.audio.gearUp = "gearupEXT.wav";
    car.audio.gearDown = "geardnEXT.wav";

    car.audio.tyreRolling = "tyre_rolling.wav";
    car.audio.tyreSkid = "skid_asphalt_lat_2_1.wav";
    car.audio.wind = "wind.wav";

    car.audio.idleRPM = 1290.0f;
    car.audio.lowRPM = 3951.0f;
    car.audio.midRPM = 5031.0f;
    car.audio.highRPM = 7200.0f;
    car.audio.topRPM = 9000.0f;

    car.audio.idleWidth = 2200.0f;
    car.audio.lowWidth = 1800.0f;
    car.audio.midWidth = 2400.0f;
    car.audio.highWidth = 2800.0f;
    car.audio.topWidth = 1400.0f;

    return car;
}

CameraDefinition Copen::CreateCameraDefinition()
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