#include "BugattiChiron.h"

VehicleDefinition Chiron::CreateDefinition()
{
    VehicleDefinition car;

    // ---------------------------------------------------------
    // 1. Mass / chassis
    // ---------------------------------------------------------
    car.mass = 1995.0f;
    car.frontWeightDistribution = 0.45f;

    car.yawInertiaScale = 1.10f;
    car.pitchInertiaScale = 1.05f;
    car.rollInertiaScale = 1.08f;

    car.yawDamping = 5200.0f;

    car.wheelOffsets[0] = btVector3(-0.95f, -0.15f, 1.65f);
    car.wheelOffsets[1] = btVector3(0.95f, -0.15f, 1.65f);
    car.wheelOffsets[2] = btVector3(-0.95f, -0.15f, -1.65f);
    car.wheelOffsets[3] = btVector3(0.95f, -0.15f, -1.65f);

    // ---------------------------------------------------------
    // 2. Aero
    // ---------------------------------------------------------
    // Chiron is slippery for what it is, but still has substantial
    // high-speed aero. Keep this far below prototype-race-car levels.
    car.dragCoefficient = 0.35f;

    car.frontDownforceCoeff = 0.085f;
    car.rearDownforceCoeff = 0.145f;

    // ---------------------------------------------------------
    // 3. Engine
    // ---------------------------------------------------------
    // 8.0L quad-turbo W16
    car.engine.idleRPM = 800.0f;
    car.engine.redlineRPM = 6700.0f;
    car.engine.limiterRPM = 6900.0f;

    car.engine.maxTorque = 1600.0f;

    car.engineBrakeTorque = 320.0f;

    // Broad turbo torque plateau.
    // Values are multipliers of maxTorque.
    car.engine.torqueCurveCount = 8;

    car.engine.torqueCurve[0] = { 1000.0f, 0.45f };
    car.engine.torqueCurve[1] = { 1500.0f, 0.72f };
    car.engine.torqueCurve[2] = { 2000.0f, 1.00f };
    car.engine.torqueCurve[3] = { 3000.0f, 1.00f };
    car.engine.torqueCurve[4] = { 4000.0f, 1.00f };
    car.engine.torqueCurve[5] = { 5000.0f, 0.98f };
    car.engine.torqueCurve[6] = { 6000.0f, 0.90f };
    car.engine.torqueCurve[7] = { 6700.0f, 0.78f };

    // ---------------------------------------------------------
    // 4. Gearbox / drivetrain
    // ---------------------------------------------------------
    // AWD. Rear biased as a baseline so it doesn't feel like a
    // gigantic FWD-ish lump under power.
    car.frontTorqueDistribution = 0.35f;
    car.rearTorqueDistribution = 0.65f;

    // 7-speed dual-clutch gearbox.
    //
    // Array layout:
    // [0] reverse
    // [1] neutral
    // [2..8] gears 1-7
    car.gearbox.finalDrive = 2.73f;
    car.gearbox.gearCount = 7;

    car.gearbox.gearRatios[0] = -3.20f;
    car.gearbox.gearRatios[1] = 0.00f;

    car.gearbox.gearRatios[2] = 3.18f;   // 1st
    car.gearbox.gearRatios[3] = 2.26f;   // 2nd
    car.gearbox.gearRatios[4] = 1.68f;   // 3rd
    car.gearbox.gearRatios[5] = 1.29f;   // 4th
    car.gearbox.gearRatios[6] = 1.03f;   // 5th
    car.gearbox.gearRatios[7] = 0.84f;   // 6th
    car.gearbox.gearRatios[8] = 0.68f;   // 7th

    // ---------------------------------------------------------
    // 5. Differential
    // ---------------------------------------------------------
    // Stronger/stabler than the R10 baseline, but don't weld the
    // rear axle together. This thing has enormous torque.
    car.lockStrength = 115.0f;
    car.preload = 210.0f;

    car.diffMaxLockTorque = 1350.0f;

    car.diffLoadBiasStrength = 700.0f;
    car.diffMaxLoadBiasTorque = 500.0f;

    // ---------------------------------------------------------
    // 6. Steering / response
    // ---------------------------------------------------------
    // Keep your original Chiron steering range as the baseline.
    car.lowSpeedSteerAngle = 0.65f;
    car.highSpeedSteerAngle = 0.08f;

    // Heavy road car: deliberately slower response than R10.
    car.loadResponseRate = 35.0f;
    car.slipResponseRate = 40.0f;

    car.loadTransferStrength = 0.62f;

    // ---------------------------------------------------------
    // 7. Brakes
    // ---------------------------------------------------------
    car.brakeBiasFront = 0.57f;
    car.brakeBiasRear = 0.43f;

    // Considerably stronger than the unfinished old values.
    // Nearly two tonnes + hypercar speed needs serious brakes.
    car.brakeTorqueStrength = 6500.0f;
    car.brakeForceStrength = 9000.0f;

    // ---------------------------------------------------------
    // 8. Suspension
    // ---------------------------------------------------------
    car.suspensionRestLength = 0.60f;

    // Road hypercar rather than prototype.
    // Front remains substantially stiffer in roll than rear.
    car.frontARBStiffness = 12000.0f;
    car.rearARBStiffness = 7000.0f;

    car.suspension.frontSpringRate = 52000.0f;
    car.suspension.rearSpringRate = 58000.0f;

    // Front damping
    car.suspension.frontSlowBump = 2600.0f;
    car.suspension.frontFastBump = 1650.0f;
    car.suspension.frontFastBumpThreshold = 0.105f;

    car.suspension.frontRebound = 4800.0f;
    car.suspension.frontFastRebound = 3000.0f;
    car.suspension.frontFastReboundThreshold = 0.125f;

    // Rear damping
    car.suspension.rearSlowBump = 2850.0f;
    car.suspension.rearFastBump = 1800.0f;
    car.suspension.rearFastBumpThreshold = 0.105f;

    car.suspension.rearRebound = 5200.0f;
    car.suspension.rearFastRebound = 3250.0f;
    car.suspension.rearFastReboundThreshold = 0.125f;

    car.suspension.bumpStopRate = 105000.0f;
    car.suspension.bumpStopStart = 0.84f;

    car.suspension.dampingScale = 0.72f;

    // ---------------------------------------------------------
    // 9. Tyres
    // ---------------------------------------------------------
    // High-performance road tyres.
    //
    // Lower outright friction/stiffness than the R10 race tyre,
    // with a little slower response due to the road-car setup.

    // Front
    car.frontTyres.frictionCoeff = 1.42f;
    car.frontTyres.loadSmoothing = 10.0f;
    car.frontTyres.relaxationRate = 30.0f;
    car.frontTyres.stiffness = 20500.0f;

    car.frontTyres.staticCamber = -1.4f;
    car.frontTyres.camberGripStrength = 0.085f;
    car.frontTyres.CamberGain = -12.0f;

    car.frontTyres.wheelRadius = 0.355f;
    car.frontTyres.frontWheelInertia = 1.85f;

    // Rear
    car.rearTyres.frictionCoeff = 1.46f;
    car.rearTyres.loadSmoothing = 10.0f;
    car.rearTyres.relaxationRate = 28.0f;
    car.rearTyres.stiffness = 22000.0f;

    car.rearTyres.staticCamber = -1.6f;
    car.rearTyres.camberGripStrength = 0.085f;
    car.rearTyres.CamberGain = -13.0f;

    car.rearTyres.wheelRadius = 0.365f;
    car.rearTyres.rearWheelInertia = 2.10f;

    car.audio.basePath = "C:\\Users\\Void\\Documents\\GitHub\\Game-Engine-DX11-Playground\\Project1\\Cars\\Bugatti_Chiron\\sounds\\";

    car.audio.idle =
        "ext_sls_idle.wav";

    car.audio.lowOn =
        "ext_sls_on_2400.wav";

    car.audio.midOn =
        "ext_sls_on_3000.wav";

    car.audio.highOn =
        "ext_sls_on_4400.wav";

    car.audio.topFull =
        "ext_sls_on_6000.wav";

    car.audio.limiter =
        "limiter.wav";

    car.audio.transmission =
        "transmission.wav";

    car.audio.gearUp =
        "gearup1.wav";

    car.audio.gearDown =
        "geardn1.wav";

    car.audio.tyreRolling =
        "tyre_rolling.wav";

    car.audio.tyreSkid =
        "skid_ext_mono.wav";

    car.audio.wind =
        "wind.wav";

    car.audio.idleRPM = 800.0f;
    car.audio.lowRPM = 2400.0f;
    car.audio.midRPM = 3000.0f;
    car.audio.highRPM = 4400.0f;
    car.audio.topRPM = 6000.0f;

    car.audio.idleWidth = 900.0f;
    car.audio.lowWidth = 1100.0f;
    car.audio.midWidth = 1400.0f;
    car.audio.highWidth = 1700.0f;
    car.audio.topWidth = 1600.0f;

    return car;
}


CameraDefinition Chiron::CreateCameraDefinition()
{
    CameraDefinition camera;

    camera.chaseHeight = 3.0f;
    camera.chaseDistance = -32.0f;
    camera.chasePitchDeg = 15.0f;

    camera.roofHeight = 2.0f;
    camera.roofDistance = 1.0f;
    camera.roofPitchDeg = 0.0f;

    camera.bumperHeight = 0.3f;
    camera.bumperDistance = -1.5f;
    camera.bumperPitchDeg = 5.0f;

    return camera;
}