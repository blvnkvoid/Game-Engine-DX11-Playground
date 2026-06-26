#include "Minolta.h"

VehicleDefinition Minolta::CreateDefinition()
{
    VehicleDefinition car;
// ---------------------------------------------------------
// 1. Mass / chassis
// ---------------------------------------------------------
car.mass = 850.0f;
car.frontWeightDistribution = 0.49f;
car.yawInertiaScale = 1.20f;
car.pitchInertiaScale = 1.20f;
car.rollInertiaScale = 1.20f;
car.yawDamping = 5500.0f;

car.wheelOffsets[0] = btVector3(-0.775f, -0.12f,  1.351f);
car.wheelOffsets[1] = btVector3( 0.775f, -0.12f,  1.351f);
car.wheelOffsets[2] = btVector3(-0.760f, -0.12f, -1.299f);
car.wheelOffsets[3] = btVector3( 0.760f, -0.12f, -1.299f);

// ---------------------------------------------------------
// 2. Aero
// ---------------------------------------------------------
car.dragCoefficient = 0.32f;
car.frontDownforceCoeff = 0.150f;
car.rearDownforceCoeff = 0.225f;

// ---------------------------------------------------------
// 3. Engine
// ---------------------------------------------------------
car.engine.idleRPM = 1200.0f;
car.engine.redlineRPM = 8000.0f;
car.engine.limiterRPM = 8300.0f;
car.engine.maxTorque = 915.0f;
car.engineBrakeTorque = 180.0f;

car.engine.torqueCurveCount = 8;
car.engine.torqueCurve[0] = { 1200.0f, 0.25f };
car.engine.torqueCurve[1] = { 3000.0f, 0.55f };
car.engine.torqueCurve[2] = { 4500.0f, 0.82f };
car.engine.torqueCurve[3] = { 5800.0f, 0.98f };
car.engine.torqueCurve[4] = { 6500.0f, 1.00f };
car.engine.torqueCurve[5] = { 7200.0f, 1.00f };
car.engine.torqueCurve[6] = { 8000.0f, 0.96f };
car.engine.torqueCurve[7] = { 8300.0f, 0.86f };

// ---------------------------------------------------------
// 4. Gearbox / drivetrain
// ---------------------------------------------------------
car.frontTorqueDistribution = 0.0f;
car.rearTorqueDistribution = 1.0f;

car.gearbox.finalDrive = 3.80f;
car.gearbox.gearCount = 7;

car.gearbox.gearRatios[0] = -2.80f;
car.gearbox.gearRatios[1] = 0.00f;
car.gearbox.gearRatios[2] = 2.85f;
car.gearbox.gearRatios[3] = 1.95f;
car.gearbox.gearRatios[4] = 1.45f;
car.gearbox.gearRatios[5] = 1.12f;
car.gearbox.gearRatios[6] = 0.88f;
car.gearbox.gearRatios[7] = 0.68f;
car.gearbox.gearRatios[8] = 0.00f;

// ---------------------------------------------------------
// 5. Differential
// ---------------------------------------------------------
car.lockStrength = 70.0f;          // from 85
car.preload = 300.0f;              // from 400
car.diffMaxLockTorque = 750.0f;    // from 950
car.diffLoadBiasStrength = 700.0f; // from 1000
car.diffMaxLoadBiasTorque = 450.0f;// from 600

// ---------------------------------------------------------
// 6. Steering / response
// ---------------------------------------------------------
//car.lowSpeedSteerAngle = 0.38f;
car.lowSpeedSteerAngle = 0.45f;
car.highSpeedSteerAngle = 0.075f;

car.loadResponseRate = 45.0f;
car.slipResponseRate = 45.0f;
car.loadTransferStrength = 0.85f;

// ---------------------------------------------------------
// 7. Brakes
// ---------------------------------------------------------
car.brakeBiasFront = 0.51f;
car.brakeBiasRear = 0.49f;
car.brakeTorqueStrength = 6200.0f;
car.brakeForceStrength = 9000.0f;

// ---------------------------------------------------------
// 8. Suspension
// ---------------------------------------------------------
car.suspensionRestLength = 0.60f;

car.frontARBStiffness = 19000.0f;
car.rearARBStiffness = 19000.0f;

car.suspension.frontSpringRate = 145000.0f;
car.suspension.rearSpringRate = 155000.0f;

car.suspension.frontSlowBump = 6500.0f;
car.suspension.frontFastBump = 4200.0f;
car.suspension.frontFastBumpThreshold = 0.10f;
car.suspension.frontRebound = 9500.0f;
car.suspension.frontFastRebound = 6000.0f;
car.suspension.frontFastReboundThreshold = 0.12f;

car.suspension.rearSlowBump = 7000.0f;
car.suspension.rearFastBump = 4600.0f;
car.suspension.rearFastBumpThreshold = 0.10f;
car.suspension.rearRebound = 10500.0f;
car.suspension.rearFastRebound = 6500.0f;
car.suspension.rearFastReboundThreshold = 0.12f;

car.suspension.bumpStopRate = 300000.0f;
car.suspension.bumpStopStart = 0.70f;
car.suspension.dampingScale = 0.85f;

// ---------------------------------------------------------
// 9. Tyres
// ---------------------------------------------------------
car.frontTyres.frictionCoeff = 1.95f;
car.frontTyres.loadSmoothing = 8.0f;
car.frontTyres.relaxationRate = 38.0f;
car.frontTyres.stiffness = 27000.0f;
car.frontTyres.staticCamber = -2.8f;
car.frontTyres.camberGripStrength = 0.06f;
car.frontTyres.CamberGain = -14.0f;
car.frontTyres.wheelRadius = 0.335f;
car.frontTyres.frontWheelInertia = 1.85f;

car.rearTyres.frictionCoeff = 2.00f;
car.rearTyres.loadSmoothing = 8.0f;
car.rearTyres.relaxationRate = 35.0f;
car.rearTyres.stiffness = 28500.0f;
car.rearTyres.staticCamber = -2.2f;
car.rearTyres.camberGripStrength = 0.06f;
car.rearTyres.CamberGain = -10.0f;
car.rearTyres.wheelRadius = 0.345f;
car.rearTyres.rearWheelInertia = 2.05f; 

    car.audio.basePath =
        "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine\\Project1\\Cars\\Toyota_Minolta\\sounds\\";

    car.audio.idle = "962c_3_in_idle.wav";

    car.audio.lowOn = "962c_ex_on_verylow3.wav";
    car.audio.midOn = "962c_4_in_on_low.wav";
    car.audio.highOn = "962c_4_in_on_mid2.wav";
    car.audio.topFull = "962c_3_in_on_high.wav";

    car.audio.limiter = "limiter_nissangtrgt3.wav";

    car.audio.transmission = "962_app.wav";
    car.audio.gearUp = "gearupEXT.wav";
    car.audio.gearDown = "geardnEXT.wav";

    car.audio.tyreRolling = "tyre_rolling.wav";
    car.audio.tyreSkid = "skid_ext_mono.wav";
    car.audio.wind = "wind.wav";

    car.audio.idleRPM = 1100.0f;
    car.audio.lowRPM = 2500.0f;
    car.audio.midRPM = 4500.0f;
    car.audio.highRPM = 6500.0f;
    car.audio.topRPM = 8000.0f;

    car.audio.idleWidth = 1400.0f;
    car.audio.lowWidth = 2200.0f;
    car.audio.midWidth = 2400.0f;
    car.audio.highWidth = 2200.0f;
    car.audio.topWidth = 1800.0f;

    return car;
}

CameraDefinition Minolta::CreateCameraDefinition()
{
    CameraDefinition camera;

    camera.chaseHeight = 3.0f;
    camera.chaseDistance = -32.0f;
    camera.chasePitchDeg = 15.0f;

    camera.roofHeight = 1.2f;
    camera.roofDistance = -2.5f;
    camera.roofPitchDeg = 0.0f;

    camera.bumperHeight = 0.7f;
    camera.bumperDistance = 2.2f;
    camera.bumperPitchDeg = 5.0f;


    return camera;
}