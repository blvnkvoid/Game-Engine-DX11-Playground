#include "Xsara_KitCar.h"

VehicleDefinition Xsara::CreateDefinition()
{
    VehicleDefinition car;

    // ---------------------------------------------------------
    // 1. Mass / chassis
    // ---------------------------------------------------------
    car.mass = 960.0f;
    car.frontWeightDistribution = 0.51f;
    car.yawInertiaScale = 1.25f;
    car.pitchInertiaScale = 1.10f;
    car.rollInertiaScale = 1.10f;
    car.yawDamping = 2000.0f;

    car.wheelOffsets[0] = btVector3(-0.823f, -0.14f, 1.252f);
    car.wheelOffsets[1] = btVector3(0.823f, -0.14f, 1.252f);
    car.wheelOffsets[2] = btVector3(-0.810f, -0.14f, -1.303f);
    car.wheelOffsets[3] = btVector3(0.810f, -0.14f, -1.303f);

    // ---------------------------------------------------------
    // 2. Aero
    // ---------------------------------------------------------
    car.dragCoefficient = 0.38f;
    car.frontDownforceCoeff = 0.030f;
    car.rearDownforceCoeff = 0.045f;

    // ---------------------------------------------------------
    // 3. Engine
    // ---------------------------------------------------------
    car.engine.idleRPM = 1100.0f;
    car.engine.redlineRPM = 8500.0f;
    car.engine.limiterRPM = 8700.0f;
    car.engine.maxTorque = 260.0f;
    car.engineBrakeTorque = 120.0f;

    car.engine.torqueCurveCount = 6;
    car.engine.torqueCurve[0] = { 1500.0f, 0.42f };
    car.engine.torqueCurve[1] = { 3000.0f, 0.68f };
    car.engine.torqueCurve[2] = { 5000.0f, 0.90f };
    car.engine.torqueCurve[3] = { 6500.0f, 1.00f };
    car.engine.torqueCurve[4] = { 7600.0f, 0.94f };
    car.engine.torqueCurve[5] = { 8200.0f, 0.82f };

    // ---------------------------------------------------------
    // 4. Gearbox / drivetrain
    // ---------------------------------------------------------
    car.frontTorqueDistribution = 1.0f;
    car.rearTorqueDistribution = 0.0f;

    car.gearbox.finalDrive = 4.60f;
    car.gearbox.gearCount = 8;

    car.gearbox.gearRatios[0] = -3.30f;
    car.gearbox.gearRatios[1] = 0.00f;
    car.gearbox.gearRatios[2] = 2.90f;
    car.gearbox.gearRatios[3] = 2.10f;
    car.gearbox.gearRatios[4] = 1.62f;
    car.gearbox.gearRatios[5] = 1.30f;
    car.gearbox.gearRatios[6] = 1.08f;
    car.gearbox.gearRatios[7] = 0.92f;
    car.gearbox.gearRatios[8] = 1.0f;

    // ---------------------------------------------------------
    // 5. Differential
    // ---------------------------------------------------------
    car.lockStrength = 75.0f;
    car.preload = 140.0f;
    car.diffMaxLockTorque = 520.0f;
    car.diffLoadBiasStrength = 420.0f;
    car.diffMaxLoadBiasTorque = 180.0f;

    // ---------------------------------------------------------
    // 6. Steering / response
    // ---------------------------------------------------------
    car.lowSpeedSteerAngle = 0.50f;
    car.highSpeedSteerAngle = 0.085f;

    car.loadResponseRate = 38.0f;
    car.slipResponseRate = 46.0f;
    car.loadTransferStrength = 0.75f;

    // ---------------------------------------------------------
    // 7. Brakes
    // ---------------------------------------------------------
    car.brakeBiasFront = 0.66f;
    car.brakeBiasRear = 0.34f;
    car.brakeTorqueStrength = 3400.0f;
    car.brakeForceStrength = 4800.0f;

    // ---------------------------------------------------------
    // 8. Suspension
    // ---------------------------------------------------------
    car.suspensionRestLength = 0.6f;

    car.frontARBStiffness = 7200.0f;
    car.rearARBStiffness = 5000.0f;

    car.suspension.frontSpringRate = 36000.0f;
    car.suspension.rearSpringRate = 34000.0f;

    car.suspension.frontSlowBump = 1600.0f;
    car.suspension.frontFastBump = 1050.0f;
    car.suspension.frontFastBumpThreshold = 0.102f;
    car.suspension.frontRebound = 3100.0f;
    car.suspension.frontFastRebound = 2000.0f;
    car.suspension.frontFastReboundThreshold = 0.122f;

    car.suspension.rearSlowBump = 1450.0f;
    car.suspension.rearFastBump = 950.0f;
    car.suspension.rearFastBumpThreshold = 0.102f;
    car.suspension.rearRebound = 2800.0f;
    car.suspension.rearFastRebound = 1800.0f;
    car.suspension.rearFastReboundThreshold = 0.122f;

    car.suspension.bumpStopRate = 60000.0f;
    car.suspension.bumpStopStart = 0.82f;
    car.suspension.dampingScale = 0.60f;

    // ---------------------------------------------------------
    // 9. Tyres
    // ---------------------------------------------------------
    car.frontTyres.frictionCoeff = 1.45f;
    car.frontTyres.loadSmoothing = 10.0f;
    car.frontTyres.relaxationRate = 30.0f;
    car.frontTyres.stiffness = 18500.0f;
    car.frontTyres.staticCamber = -2.2f;
    car.frontTyres.camberGripStrength = 0.08f;
    car.frontTyres.CamberGain = -12.0f;
    car.frontTyres.wheelRadius = 0.315f;
    car.frontTyres.frontWheelInertia = 1.25f;

    car.rearTyres.frictionCoeff = 1.50f;
    car.rearTyres.loadSmoothing = 10.0f;
    car.rearTyres.relaxationRate = 27.0f;
    car.rearTyres.stiffness = 20000.0f;
    car.rearTyres.staticCamber = -1.8f;
    car.rearTyres.camberGripStrength = 0.08f;
    car.rearTyres.CamberGain = -10.0f;
    car.rearTyres.wheelRadius = 0.315f;
    car.rearTyres.rearWheelInertia = 1.15f;


    // ---------------------------------------------------------
    // 10. Audio
    // ---------------------------------------------------------
    car.audio.basePath = "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine\\Project1\\Cars\\Citroen_Xsara_KitCar\\sounds\\";

    car.audio.idle = "idle_2705.wav";

    car.audio.lowOn = "3714a.wav";
    car.audio.midOn = "4885a.wav";
    car.audio.highOn = "7267a.wav";
    car.audio.topFull = "8680b.wav";

    car.audio.limiter = "limiter.wav";

    car.audio.transmission = "transmission.wav";
    car.audio.gearUp = "gearup.wav";
    car.audio.gearDown = "geardn.wav";

    car.audio.tyreRolling = "tyre_rolling.wav";
    car.audio.tyreSkid = "Skid.wav";
    car.audio.wind = "wind.wav";

    car.audio.idleRPM = 1372.0f;
    car.audio.lowRPM = 2761.0f;
    car.audio.midRPM = 4974.0f;
    car.audio.highRPM = 6686.0f;
    car.audio.topRPM = 8500.0f;

    car.audio.idleWidth = 1300.0f;
    car.audio.lowWidth = 1700.0f;
    car.audio.midWidth = 1900.0f;
    car.audio.highWidth = 1600.0f;
    car.audio.topWidth = 1600.0f;
    return car;
}

CameraDefinition Xsara::CreateCameraDefinition()
{
    CameraDefinition camera;

    camera.chaseHeight = 3.5f;
    camera.chaseDistance = -35.0f;
    camera.chasePitchDeg = 15.0f;

    camera.roofHeight = 1.0f;
    camera.roofDistance = 1.0f;
    camera.roofPitchDeg = 0.0f;

    camera.bumperHeight = 0.7f;
    camera.bumperDistance = 5.0f;
    camera.bumperPitchDeg = 5.0f;

    return camera;
}