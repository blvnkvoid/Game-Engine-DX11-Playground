#include "AudiR8.h"

VehicleDefinition R8::CreateDefinition()
{
    VehicleDefinition car;

    // ---------------------------------------------------------
    // 1. Mass / chassis
    // ---------------------------------------------------------
    car.mass = 1630.0f;
    car.frontWeightDistribution = 0.45f;
    car.yawInertiaScale = 1.30f;
    car.pitchInertiaScale = 1.10f;
    car.rollInertiaScale = 1.10f;
    car.yawDamping = 6000.0f;

    car.wheelOffsets[0] = btVector3(-0.95f, -0.15f, 1.458f);
    car.wheelOffsets[1] = btVector3(0.95f, -0.15f, 1.458f);
    car.wheelOffsets[2] = btVector3(-0.95f, -0.15f, -1.193f);
    car.wheelOffsets[3] = btVector3(0.95f, -0.15f, -1.193f);

    // ---------------------------------------------------------
    // 2. Aero
    // ---------------------------------------------------------
    car.dragCoefficient = 0.34f;
    car.frontDownforceCoeff = 0.060f;
    car.rearDownforceCoeff = 0.075f;

    // ---------------------------------------------------------
    // 3. Engine
    // ---------------------------------------------------------
    car.engine.idleRPM = 900.0f;
    car.engine.redlineRPM = 8500.0f;
    car.engine.limiterRPM = 8600.0f;
    car.engine.maxTorque = 560.0f;
    car.engineBrakeTorque = 150.0f;

    car.engine.torqueCurveCount = 6;
    car.engine.torqueCurve[0] = { 1000.0f, 0.35f };
    car.engine.torqueCurve[1] = { 2500.0f, 0.55f };
    car.engine.torqueCurve[2] = { 4500.0f, 0.78f };
    car.engine.torqueCurve[3] = { 6500.0f, 1.00f };
    car.engine.torqueCurve[4] = { 8000.0f, 0.95f };
    car.engine.torqueCurve[5] = { 8500.0f, 0.85f };

    // ---------------------------------------------------------
    // 4. Gearbox / drivetrain
    // ---------------------------------------------------------
    car.frontTorqueDistribution = 0.3f;
    car.rearTorqueDistribution = 0.7f;

    car.gearbox.finalDrive = 3.60f;
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
    car.lockStrength = 90.0f;
    car.preload = 250.0f;
    car.diffMaxLockTorque = 650.0f;
    car.diffLoadBiasStrength = 600.0f;
    car.diffMaxLoadBiasTorque = 250.0f;

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
    car.brakeBiasFront = 0.62f;
    car.brakeBiasRear = 0.38f;
    car.brakeTorqueStrength = 3500.0f;
    car.brakeForceStrength = 5000.0f;

    // ---------------------------------------------------------
    // 8. Suspension
    // ---------------------------------------------------------
    car.suspensionRestLength = 0.6f;

    car.frontARBStiffness = 6500.0f;
    car.rearARBStiffness = 5000.0f;

    car.suspension.frontSpringRate = 42000.0f;
    car.suspension.rearSpringRate = 48000.0f;

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
    car.frontTyres.frictionCoeff = 1.95f;
    car.frontTyres.loadSmoothing = 8.0f;
    car.frontTyres.relaxationRate = 38.0f;
    car.frontTyres.stiffness = 27000.0f;
    car.frontTyres.staticCamber = -1.0f;
    car.frontTyres.camberGripStrength = 0.08f;
    car.frontTyres.CamberGain = -12.0f;
    car.frontTyres.wheelRadius = 0.34f;
    car.frontTyres.frontWheelInertia = 1.85f;

    car.rearTyres.frictionCoeff = 2.00f;
    car.rearTyres.loadSmoothing = 8.0f;
    car.rearTyres.relaxationRate = 35.0f;
    car.rearTyres.stiffness = 28500.0f;
    car.rearTyres.staticCamber = -2.2f;
    car.rearTyres.camberGripStrength = 0.08f;
    car.rearTyres.CamberGain = -16.0f;
    car.rearTyres.wheelRadius = 0.345f;
    car.rearTyres.rearWheelInertia = 1.95f;

    car.audio.basePath = "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine\\Project1\\Cars\\Audi_R8_LMS\\sounds\\";

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


    return camera;
}