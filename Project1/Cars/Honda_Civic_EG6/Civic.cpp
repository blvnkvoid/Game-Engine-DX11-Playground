#include "Civic.h"

VehicleDefinition Civic::CreateDefinition()
{
    VehicleDefinition car;

    // ---------------------------------------------------------
// 1. Mass / chassis
// ---------------------------------------------------------
    car.mass = 1090.0f;
    car.frontWeightDistribution = 0.60f;
    car.yawInertiaScale = 1.1f;
    car.pitchInertiaScale = 1.10f;
    car.rollInertiaScale = 1.10f;
    car.yawDamping = 3000.0f;

    car.wheelOffsets[0] = btVector3(-0.7375f, -0.14f, 1.028f);
    car.wheelOffsets[1] = btVector3(0.7375f, -0.14f, 1.028f);
    car.wheelOffsets[2] = btVector3(-0.7325f, -0.15f, -1.542f);
    car.wheelOffsets[3] = btVector3(0.7325f, -0.15f, -1.542f);

    // ---------------------------------------------------------
    // 2. Aero
    // ---------------------------------------------------------
    car.dragCoefficient = 0.34f;
    car.frontDownforceCoeff = 0.015f;
    car.rearDownforceCoeff = 0.010f;

    // ---------------------------------------------------------
    // 3. Engine
    // ---------------------------------------------------------
    car.engine.idleRPM = 900.0f;
    car.engine.redlineRPM = 8000.0f;
    car.engine.limiterRPM = 8200.0f;
    car.engine.maxTorque = 200.0f;
    car.engineBrakeTorque = 90.0f;

    car.engine.torqueCurveCount = 7;
    car.engine.torqueCurve[0] = { 900.0f,  0.38f };
    car.engine.torqueCurve[1] = { 2500.0f, 0.55f };
    car.engine.torqueCurve[2] = { 4000.0f, 0.72f };
    car.engine.torqueCurve[3] = { 5500.0f, 0.86f };
    car.engine.torqueCurve[4] = { 6500.0f, 1.00f };
    car.engine.torqueCurve[5] = { 7600.0f, 0.96f };
    car.engine.torqueCurve[6] = { 8200.0f, 0.82f };

    // ---------------------------------------------------------
    // 4. Gearbox / drivetrain
    // ---------------------------------------------------------
    car.frontTorqueDistribution = 1.0f;
    car.rearTorqueDistribution = 0.0f;

    car.gearbox.finalDrive = 4.40f;
    car.gearbox.gearCount = 7;

    car.gearbox.gearRatios[0] = -3.23f;
    car.gearbox.gearRatios[1] = 0.00f;
    car.gearbox.gearRatios[2] = 3.23f;
    car.gearbox.gearRatios[3] = 2.10f;
    car.gearbox.gearRatios[4] = 1.46f;
    car.gearbox.gearRatios[5] = 1.11f;
    car.gearbox.gearRatios[6] = 0.85f;
    car.gearbox.gearRatios[7] = 0.00f;
    car.gearbox.gearRatios[8] = 0.00f;

    // ---------------------------------------------------------
    // 5. Differential
    // ---------------------------------------------------------
    car.lockStrength = 65.0f;
    car.preload = 120.0f;
    car.diffMaxLockTorque = 350.0f;
    car.diffLoadBiasStrength = 350.0f;
    car.diffMaxLoadBiasTorque = 160.0f;

    // ---------------------------------------------------------
    // 6. Steering / response
    // ---------------------------------------------------------
    car.lowSpeedSteerAngle = 0.50f;
    car.highSpeedSteerAngle = 0.10f;

    car.loadResponseRate = 35.0f;
    car.slipResponseRate = 40.0f;
    car.loadTransferStrength = 0.75f;

    // ---------------------------------------------------------
    // 7. Brakes
    // ---------------------------------------------------------
    car.brakeBiasFront = 0.68f;
    car.brakeBiasRear = 0.32f;
    car.brakeTorqueStrength = 2600.0f;
    car.brakeForceStrength = 3900.0f;

    // ---------------------------------------------------------
    // 8. Suspension
    // ---------------------------------------------------------
    car.suspensionRestLength = 0.55f;

    car.frontARBStiffness = 9000.0f;
    car.rearARBStiffness = 6500.0f;

    car.suspension.frontSpringRate = 32000.0f;
    car.suspension.rearSpringRate = 24000.0f;

    car.suspension.frontSlowBump = 3700.0f;
    car.suspension.frontFastBump = 2700.0f;
    car.suspension.frontFastBumpThreshold = 0.10f;
    car.suspension.frontRebound = 5400.0f;
    car.suspension.frontFastRebound = 4000.0f;
    car.suspension.frontFastReboundThreshold = 0.10f;

    car.suspension.rearSlowBump = 4600.0f;
    car.suspension.rearFastBump = 3200.0f;
    car.suspension.rearFastBumpThreshold = 0.10f;
    car.suspension.rearRebound = 5500.0f;
    car.suspension.rearFastRebound = 3600.0f;
    car.suspension.rearFastReboundThreshold = 0.10f;

    car.suspension.bumpStopRate = 40000.0f;
    car.suspension.bumpStopStart = 0.78f;
    car.suspension.dampingScale = 0.75f;

    // ---------------------------------------------------------
    // 9. Tyres
    // ---------------------------------------------------------
    car.frontTyres.frictionCoeff = 1.24f;
    car.frontTyres.loadSmoothing = 10.0f;
    car.frontTyres.relaxationRate = 26.0f;
    car.frontTyres.stiffness = 12000.0f;
    car.frontTyres.staticCamber = -1.6f;
    car.frontTyres.camberGripStrength = 0.07f;
    car.frontTyres.CamberGain = -10.0f;
    car.frontTyres.wheelRadius = 0.308f;
    car.frontTyres.frontWheelInertia = 1.59f;

    car.rearTyres.frictionCoeff = 1.24f;
    car.rearTyres.loadSmoothing = 10.0f;
    car.rearTyres.relaxationRate = 24.0f;
    car.rearTyres.stiffness = 11500.0f;
    car.rearTyres.staticCamber = -1.2f;
    car.rearTyres.camberGripStrength = 0.07f;
    car.rearTyres.CamberGain = -8.0f;
    car.rearTyres.wheelRadius = 0.308f;
    car.rearTyres.rearWheelInertia = 1.59f;

    car.audio.basePath =
        "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine\\Project1\\Cars\\Honda_Civic_EG6\\sounds\\";

    car.audio.idle = "1 EngA_00867.wav";
    car.audio.lowOn = "3 EngA_02100.wav";
    car.audio.midOn = "8 EngA_04888.wav";
    car.audio.highOn = "11 EngA_06859.wav";
    car.audio.topFull = "14 EngA_08829.wav";

    car.audio.limiter = "16 EngB_08261.wav";

    car.audio.transmission = "transmission.wav";
    car.audio.gearUp = "gearupEXT.wav";
    car.audio.gearDown = "geardnEXT.wav";

    car.audio.tyreRolling = "tyre_rolling.wav";
    car.audio.tyreSkid = "skid_ext_mono.wav";
    car.audio.wind = "wind.wav";

    car.audio.idleRPM = 867.0f;
    car.audio.lowRPM = 2100.0f;
    car.audio.midRPM = 4888.0f;
    car.audio.highRPM = 6859.0f;
    car.audio.topRPM = 8829.0f;

    car.audio.idleWidth = 1200.0f;
    car.audio.lowWidth = 1800.0f;
    car.audio.midWidth = 2100.0f;
    car.audio.highWidth = 1700.0f;
    car.audio.topWidth = 1600.0f;

    return car;
}

CameraDefinition Civic::CreateCameraDefinition()
{
    CameraDefinition camera;

    camera.chaseHeight = 3.0f;
    camera.chaseDistance = -32.0f;
    camera.chasePitchDeg = 15.0f;

    camera.roofHeight = 1.0f;
    camera.roofDistance = -0.5f;
    camera.roofPitchDeg = 0.0f;

    camera.bumperHeight = 0.7f;
    camera.bumperDistance = 2.7f;
    camera.bumperPitchDeg = 5.0f;


    return camera;
}