#include "TelemetryRecorder.h"
#include <iomanip>

bool TelemetryRecorder::Start(const std::string& filename)
{
    if (m_recording)
        return false;

    m_file.open(filename, std::ios::out | std::ios::trunc);
    if (!m_file.is_open())
        return false;

    m_elapsedTime = 0.0f;
    m_sampleCount = 0;
    m_file << std::fixed << std::setprecision(6);

    m_file
        << "time,dt,speed,throttle,"
        << "brake,steering,yawRate,lateralG,"
        << "longitudinalG,rpm,gear,torque,"
        << "FL_load,FR_load,RL_load,RR_load,"
        << "FL_compression,FR_compression,RL_compression,RR_compression,"
        << "FL_slipRatio,FR_slipRatio,RL_slipRatio,RR_slipRatio,"
        << "FL_lateralForce,FR_lateralForce,RL_lateralForce,RR_lateralForce,"
        << "FL_longitudinalForce,FR_longitudinalForce,RL_longitudinalForce,RR_longitudinalForce,"
        << "FL_wheelOmega,FR_wheelOmega,RL_wheelOmega,RR_wheelOmega,"
        << "FL_roadOmega,FR_roadOmega,RL_roadOmega,RR_roadOmega,"
        << "FL_driveTorque,FR_driveTorque,RL_driveTorque,RR_driveTorque,"
        << "FL_brakeTorque,FR_brakeTorque,RL_brakeTorque,RR_brakeTorque,"
        << "FL_reactionTorque,FR_reactionTorque,RL_reactionTorque,RR_reactionTorque,"
        << "FL_netTorque,FR_netTorque,RL_netTorque,RR_netTorque,"
        << "FL_drivingForce,FR_drivingForce,RL_drivingForce,RR_drivingForce,"
        << "FL_rawLongitudinalForce,FR_rawLongitudinalForce,RL_rawLongitudinalForce,RR_rawLongitudinalForce,"
        << "FL_rawLateralForce,FR_rawLateralForce,RL_rawLateralForce,RR_rawLateralForce,"
        << "FL_finalLongitudinalForce,FR_finalLongitudinalForce,RL_finalLongitudinalForce,RR_finalLongitudinalForce,"
        << "FL_finalLateralForce,FR_finalLateralForce,RL_finalLateralForce,RR_finalLateralForce,"
        << "FL_longitudinalGripUtilization,FR_longitudinalGripUtilization,RL_longitudinalGripUtilization,RR_longitudinalGripUtilization,"
        << "FL_lateralGripUtilization,FR_lateralGripUtilization,RL_lateralGripUtilization,RR_lateralGripUtilization,"
        << "FL_combinedGripUtilization,FR_combinedGripUtilization,RL_combinedGripUtilization,RR_combinedGripUtilization,"
        << "FL_combinedMaxGrip,FR_combinedMaxGrip,RL_combinedMaxGrip,RR_combinedMaxGrip,"
        << "FL_combinedTotalForce,FR_combinedTotalForce,RL_combinedTotalForce,RR_combinedTotalForce,"
        << "FL_longitudinalPeakForce,FR_longitudinalPeakForce,RL_longitudinalPeakForce,RR_longitudinalPeakForce,"
        << "FL_wheelAngularAcceleration,FR_wheelAngularAcceleration,RL_wheelAngularAcceleration,RR_wheelAngularAcceleration,"
        << "FL_reactionTorqueForceMismatch,FR_reactionTorqueForceMismatch,RL_reactionTorqueForceMismatch,RR_reactionTorqueForceMismatch,"
        << "FL_rawLoad,FR_rawLoad,RL_rawLoad,RR_rawLoad,"
        << "FL_suspensionDiff,FR_suspensionDiff,RL_suspensionDiff,RR_suspensionDiff,"
        << "FL_steeringAngle,FR_steeringAngle,RL_steeringAngle,RR_steeringAngle,"
        << "FL_wheelTorque,FR_wheelTorque,RL_wheelTorque,RR_wheelTorque,"
        << "FL_dampingForce,FR_dampingForce,RL_dampingForce,RR_dampingForce,"
        << "FL_raycastHitType,FR_raycastHitType,RL_raycastHitType,RR_raycastHitType,"
        << "FL_tyreForceHeightError,FR_tyreForceHeightError,RL_tyreForceHeightError,RR_tyreForceHeightError,"
        << "FL_slipAngle,FR_slipAngle,RL_slipAngle,RR_slipAngle,"
        << "FL_tyreTheta,FR_tyreTheta,RL_tyreTheta,RR_tyreTheta,"
        << "FL_surfaceMu,FR_surfaceMu,RL_surfaceMu,RR_surfaceMu,"
        << "FL_tyreSideStiffness,FR_tyreSideStiffness,RL_tyreSideStiffness,RR_tyreSideStiffness,"
        << "FL_wheelVx,FR_wheelVx,RL_wheelVx,RR_wheelVx,"
        << "FL_wheelVy,FR_wheelVy,RL_wheelVy,RR_wheelVy,"
        << "FL_localForwardVel,FR_localForwardVel,RL_localForwardVel,RR_localForwardVel,"
        << "FL_localSideVel,FR_localSideVel,RL_localSideVel,RR_localSideVel,"
        << "FL_bodySideVel,FR_bodySideVel,RL_bodySideVel,RR_bodySideVel,"
        << "FL_bodyForwardVel,FR_bodyForwardVel,RL_bodyForwardVel,RR_bodyForwardVel,"
        << "FL_bodySlipAngle,FR_bodySlipAngle,RL_bodySlipAngle,RR_bodySlipAngle,"
        << "FL_camberDeg,FR_camberDeg,RL_camberDeg,RR_camberDeg,"
        << "FL_camberCompression,FR_camberCompression,RL_camberCompression,RR_camberCompression,"
        << "FL_diffLockTorque,FR_diffLockTorque,RL_diffLockTorque,RR_diffLockTorque,"
        << "diffSpeed,diffTargetTorque,diffLoadBiasTorque,diffTotalTorque,"
        << "mass,dragMagnitude,downforceMagnitude,frontDownforce,"
        << "rearDownforce\n";

    m_recording = true;
    return true;
}

void TelemetryRecorder::Stop()
{
    if (!m_recording)
        return;

    m_file.flush();
    m_file.close();
    m_recording = false;
}

void TelemetryRecorder::Toggle(const std::string& filename)
{
    if (m_recording)
        Stop();
    else
        Start(filename);
}

void TelemetryRecorder::Record(float deltaTime, const SharedVehicleInputs& inputs)
{
    if (!m_recording || !m_file.is_open())
        return;

    m_file
        << m_elapsedTime
        << "," << deltaTime
        << "," << g_DebugTelemetry.speed
        << "," << inputs.throttle
        << "," << inputs.brake
        << "," << inputs.steering
        << "," << g_DebugTelemetry.yawRate
        << "," << g_DebugTelemetry.lateralG
        << "," << g_DebugTelemetry.longitudinalG
        << "," << g_DebugTelemetry.rpm
        << "," << g_DebugTelemetry.gear
        << "," << g_DebugTelemetry.torque;

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.wheelLoadSmoothed[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.compression[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.slipRatio[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.lateralForce[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.longitudinalForce[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.wheelAngularVelocity[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.roadAngularVelocity[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.driveTorque[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.brakeTorque[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.tireReactionTorque[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.wheelNetTorque[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.drivingForce[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.rawLongitudinalForce[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.rawLateralForce[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.finalLongitudinalForce[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.finalLateralForce[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.longitudinalGripUtilization[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.lateralGripUtilization[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.combinedGripUtilization[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.combinedMaxGrip[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.combinedTotalForce[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.longitudinalPeakForce[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.wheelAngularAcceleration[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.reactionTorqueForceMismatch[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.wheelLoad[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.diff[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.steeringAngle[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.wheelTorque[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.dampingForce[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.raycastHitType[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.tyreForceHeightError[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.slip[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.tyreTheta[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.surfaceMu[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.tyreSideStiffness[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.wheelVx[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.wheelVy[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.localForwardVel[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.localSideVel[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.bodySideVel[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.bodyForwardVel[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.bodySlipAngle[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.camberDeg[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.camberCompression[i];

    for (int i = 0; i < 4; ++i)
        m_file << ',' << g_DebugTelemetry.diffLockTorque[i];

    m_file << ',' << g_DebugTelemetry.diffSpeed;
    m_file << ',' << g_DebugTelemetry.diffTargetTorque;
    m_file << ',' << g_DebugTelemetry.diffLoadBiasTorque;
    m_file << ',' << g_DebugTelemetry.diffTotalTorque;
    m_file << ',' << g_DebugTelemetry.mass;
    m_file << ',' << g_DebugTelemetry.dragMagnitude;
    m_file << ',' << g_DebugTelemetry.downforceMagnitude;
    m_file << ',' << g_DebugTelemetry.frontDownforce;
    m_file << ',' << g_DebugTelemetry.rearDownforce;

    m_file << '\n';

    m_elapsedTime += deltaTime;
    ++m_sampleCount;
}
