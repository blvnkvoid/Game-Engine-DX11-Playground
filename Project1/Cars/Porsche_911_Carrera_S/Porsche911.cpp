#include "Porsche911.h"

VehicleDefinition Porsche911::CreateDefinition()
{
    VehicleDefinition car;

    // ---------------------------------------------------------
    // 1. Mass / chassis
    // ---------------------------------------------------------
    car.mass = 1500.0f;
    car.frontWeightDistribution = 0.38f;
    car.yawInertiaScale = 1.35f;
    car.pitchInertiaScale = 1.10f;
    car.rollInertiaScale = 1.10f;
    car.yawDamping = 5000.0f;

    car.wheelOffsets[0] = btVector3(-0.95f, -0.15f, 1.407f);
    car.wheelOffsets[1] = btVector3(0.95f, -0.15f, 1.407f);
    car.wheelOffsets[2] = btVector3(-0.95f, -0.15f, -0.863f);
    car.wheelOffsets[3] = btVector3(0.95f, -0.15f, -0.863f);

    // ---------------------------------------------------------
    // 2. Aero
    // ---------------------------------------------------------
    car.dragCoefficient = 0.32f;
    car.frontDownforceCoeff = 0.055f;
    car.rearDownforceCoeff = 0.085f;

    // ---------------------------------------------------------
    // 3. Engine
    // ---------------------------------------------------------
    car.engine.idleRPM = 900.0f;
    car.engine.redlineRPM = 7500.0f;
    car.engine.limiterRPM = 7600.0f;
    car.engine.maxTorque = 500.0f;
    car.engineBrakeTorque = 200.0f;

    car.engine.torqueCurveCount = 6;
    car.engine.torqueCurve[0] = { 900.0f,  0.45f };
    car.engine.torqueCurve[1] = { 1800.0f, 0.75f };
    car.engine.torqueCurve[2] = { 2500.0f, 1.00f };
    car.engine.torqueCurve[3] = { 5200.0f, 1.00f };
    car.engine.torqueCurve[4] = { 6500.0f, 0.92f };
    car.engine.torqueCurve[5] = { 7500.0f, 0.78f };

    // ---------------------------------------------------------
    // 4. Gearbox / drivetrain
    // ---------------------------------------------------------
    car.frontTorqueDistribution = 0.0f;
    car.rearTorqueDistribution = 1.0f;

    car.gearbox.finalDrive = 3.80f;
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
    car.lockStrength = 120.0f;
    car.preload = 250.0f;
    car.diffMaxLockTorque = 800.0f;
    car.diffLoadBiasStrength = 800.0f;
    car.diffMaxLoadBiasTorque = 300.0f;

    // ---------------------------------------------------------
    // 6. Steering / response
    // ---------------------------------------------------------
    car.lowSpeedSteerAngle = 0.45f;
    car.highSpeedSteerAngle = 0.08f;

    car.loadResponseRate = 35.0f;
    car.slipResponseRate = 40.0f;
    car.loadTransferStrength = 0.75f;

    // ---------------------------------------------------------
    // 7. Brakes
    // ---------------------------------------------------------
    car.brakeBiasFront = 0.57f;
    car.brakeBiasRear = 0.43f;
    car.brakeTorqueStrength = 3500.0f;
    car.brakeForceStrength = 5000.0f;

    // ---------------------------------------------------------
    // 8. Suspension
    // ---------------------------------------------------------
    car.suspensionRestLength = 0.6f;

    car.frontARBStiffness = 6000.0f;
    car.rearARBStiffness = 3500.0f;

    car.suspension.frontSpringRate = 33000.0f;
    car.suspension.rearSpringRate = 52000.0f;

    car.suspension.frontSlowBump = 2725.0f;
    car.suspension.frontFastBump = 1805.0f;
    car.suspension.frontFastBumpThreshold = 0.102f;
    car.suspension.frontRebound = 5415.0f;
    car.suspension.frontFastRebound = 3380.0f;
    car.suspension.frontFastReboundThreshold = 0.122f;

    car.suspension.rearSlowBump = 4365.0f;
    car.suspension.rearFastBump = 2870.0f;
    car.suspension.rearFastBumpThreshold = 0.102f;
    car.suspension.rearRebound = 9470.0f;
    car.suspension.rearFastRebound = 5220.0f;
    car.suspension.rearFastReboundThreshold = 0.122f;

    car.suspension.bumpStopRate = 70000.0f;
    car.suspension.bumpStopStart = 0.75f;
    car.suspension.dampingScale = 0.75f;

    // ---------------------------------------------------------
    // 9. Tyres
    // ---------------------------------------------------------
    car.frontTyres.frictionCoeff = 1.35f;
    car.frontTyres.loadSmoothing = 13.0f;
    car.frontTyres.relaxationRate = 28.0f;
    car.frontTyres.stiffness = 17000.0f;
    car.frontTyres.staticCamber = -1.0f;
    car.frontTyres.camberGripStrength = 0.08f;
    car.frontTyres.CamberGain = -12.0f;
    car.frontTyres.wheelRadius = 0.34f;
    car.frontTyres.frontWheelInertia = 1.76f;

    car.rearTyres.frictionCoeff = 1.40f;
    car.rearTyres.loadSmoothing = 13.0f;
    car.rearTyres.relaxationRate = 25.0f;
    car.rearTyres.stiffness = 18500.0f;
    car.rearTyres.staticCamber = -2.2f;
    car.rearTyres.camberGripStrength = 0.08f;
    car.rearTyres.CamberGain = -16.0f;
    car.rearTyres.wheelRadius = 0.346f;
    car.rearTyres.rearWheelInertia = 1.98f;

    car.audio.basePath = "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine\\Project1\\Cars\\Porsche_911_Carrera_S\\sounds\\";

    car.audio.idle = "911_Carrera_S_idle_in.wav";
    car.audio.lowOn = "911_Carrera_S_2300_on_in.wav";
    car.audio.midOn = "911_Carrera_S_4100_on_in.wav";
    car.audio.highOn = "911_Carrera_S_5800_on_in.wav";
    car.audio.topFull = "911_Carrera_S_7300_full_in.wav";
    car.audio.limiter = "911_Carrera_S_limiter_full_in.wav";

    car.audio.transmission = "transmission.wav";
    car.audio.gearUp = "gearupEXT.wav";
    car.audio.gearDown = "geardnEXT.wav";

    car.audio.tyreRolling = "tyre_rolling.wav";
    car.audio.tyreSkid = "Skid_street.wav";
    car.audio.wind = "MX-5_Wind.wav";

    car.audio.idleRPM = 900.0f;
    car.audio.lowRPM = 2300.0f;
    car.audio.midRPM = 4100.0f;
    car.audio.highRPM = 5800.0f;
    car.audio.topRPM = 7300.0f;

    car.audio.idleWidth = 1200.0f;
    car.audio.lowWidth = 1400.0f;
    car.audio.midWidth = 1600.0f;
    car.audio.highWidth = 1800.0f;
    car.audio.topWidth = 2200.0f;

    return car;
}


CameraDefinition Porsche911::CreateCameraDefinition()
{
    CameraDefinition camera;

    camera.chaseHeight = 3.0f;
    camera.chaseDistance = -32.0f;
    camera.chasePitchDeg = 15.0f;

    camera.roofHeight = 1.5f;
    camera.roofDistance = -2.2f;
    camera.roofPitchDeg = 0.0f;

    camera.bumperHeight = 0.7f;
    camera.bumperDistance = 4.0f;
    camera.bumperPitchDeg = 5.0f;


    return camera;
}