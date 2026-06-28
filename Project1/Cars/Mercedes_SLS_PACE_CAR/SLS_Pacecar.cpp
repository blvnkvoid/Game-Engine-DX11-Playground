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
    car.yawDamping = 0.92f;

    car.wheelOffsets[0] = btVector3(-0.86f, -0.34f, 1.34f);
    car.wheelOffsets[1] = btVector3(0.86f, -0.34f, 1.34f);
    car.wheelOffsets[2] = btVector3(-0.86f, -0.34f, -1.32f);
    car.wheelOffsets[3] = btVector3(0.86f, -0.34f, -1.32f);

    // ---------------------------------------------------------
    // 2. Aero
    // ---------------------------------------------------------
    car.dragCoefficient = 0.37f;
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

    car.gearbox.gearRatios[0] = -3.42f;
    car.gearbox.gearRatios[1] = 4.38f;
    car.gearbox.gearRatios[2] = 2.86f;
    car.gearbox.gearRatios[3] = 1.92f;
    car.gearbox.gearRatios[4] = 1.37f;
    car.gearbox.gearRatios[5] = 1.00f;
    car.gearbox.gearRatios[6] = 0.82f;
    car.gearbox.gearRatios[7] = 0.73f;
    car.gearbox.gearRatios[8] = 0.00f;

    // ---------------------------------------------------------
    // 5. Differential
    // ---------------------------------------------------------
    car.lockStrength = 58.0f;
    car.preload = 65.0f;
    car.diffMaxLockTorque = 1250.0f;
    car.diffLoadBiasStrength = 0.72f;
    car.diffMaxLoadBiasTorque = 520.0f;

    // ---------------------------------------------------------
    // 6. Steering / response
    // ---------------------------------------------------------
    car.lowSpeedSteerAngle = 0.55f;
    car.highSpeedSteerAngle = 0.10f;

    car.loadResponseRate = 34.0f;
    car.slipResponseRate = 38.0f;
    car.loadTransferStrength = 0.78f;

    // ---------------------------------------------------------
    // 7. Brakes
    // ---------------------------------------------------------
    car.brakeBiasFront = 0.66f;
    car.brakeBiasRear = 0.34f;
    car.brakeTorqueStrength = 3100.0f;
    car.brakeForceStrength = 4700.0f;

    // ---------------------------------------------------------
    // 8. Suspension
    // ---------------------------------------------------------
    car.suspensionRestLength = 0.50f;

    car.frontARBStiffness = 9800.0f;
    car.rearARBStiffness = 7600.0f;

    car.suspension.frontSpringRate = 64000.0f;
    car.suspension.rearSpringRate = 60000.0f;

    car.suspension.frontSlowBump = 4200.0f;
    car.suspension.frontFastBump = 3000.0f;
    car.suspension.frontFastBumpThreshold = 0.18f;
    car.suspension.frontRebound = 6400.0f;
    car.suspension.frontFastRebound = 4600.0f;
    car.suspension.frontFastReboundThreshold = 0.22f;

    car.suspension.rearSlowBump = 3900.0f;
    car.suspension.rearFastBump = 2800.0f;
    car.suspension.rearFastBumpThreshold = 0.18f;
    car.suspension.rearRebound = 6100.0f;
    car.suspension.rearFastRebound = 4400.0f;
    car.suspension.rearFastReboundThreshold = 0.22f;

    car.suspension.bumpStopRate = 160000.0f;
    car.suspension.bumpStopStart = 0.085f;
    car.suspension.dampingScale = 1.00f;

    // ---------------------------------------------------------
    // 9. Tyres
    // ---------------------------------------------------------
    car.frontTyres.frictionCoeff = 1.18f;
    car.frontTyres.loadSmoothing = 34.0f;
    car.frontTyres.relaxationRate = 44.0f;
    car.frontTyres.stiffness = 1.00f;
    car.frontTyres.staticCamber = -1.4f;
    car.frontTyres.camberGripStrength = 0.28f;
    car.frontTyres.CamberGain = 0.82f;
    car.frontTyres.wheelRadius = 0.340f;
    car.frontTyres.frontWheelInertia = 1.18f;

    car.rearTyres.frictionCoeff = 1.22f;
    car.rearTyres.loadSmoothing = 34.0f;
    car.rearTyres.relaxationRate = 44.0f;
    car.rearTyres.stiffness = 1.03f;
    car.rearTyres.staticCamber = -1.8f;
    car.rearTyres.camberGripStrength = 0.30f;
    car.rearTyres.CamberGain = 0.82f;
    car.rearTyres.wheelRadius = 0.340f;
    car.rearTyres.rearWheelInertia = 1.30f;


    car.audio.basePath = "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine\\Project1\\Cars\\Mercedes_SLS_PACE_CAR\\sounds\\";

    car.audio.idle = "bruit_63.wav";

    car.audio.lowOn = "4-8_4_SLS_pot_Insert 3.wav";
    car.audio.midOn = "4-8_4_SLS_pot_Insert 6.wav";
    car.audio.highOn = "4-8_4_SLS_pot_Insert 10.wav";
    car.audio.topFull = "4-8_4_SLS_pot_Insert 13.wav";

    car.audio.limiter = "4-8_4_SLS_eff_Insert 13.wav";

    car.audio.transmission = "transmission.wav";
    car.audio.gearUp = "ferrari_458_shift1.wav";
    car.audio.gearDown = "missgear.wav";

    car.audio.tyreRolling = "bruit.wav";
    car.audio.tyreSkid = "skid_ext_mono.wav";
    car.audio.wind = "wind.wav";

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


    return camera;
}