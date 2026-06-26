 #pragma warning(push)
#pragma warning(disable : 26451) // Arithmetic overflow (the one you just found)
#pragma warning(disable : 26495) // Uninitialized member variable
#pragma warning(disable : 26812) // Prefer 'enum class' over 'enum'
#pragma warning(disable : 4099)  // Type name first seen using 'class' now seen using 'struct'
#pragma warning(disable : 4244)  // Conversion from 'double' to 'btScalar', possible loss of data
#pragma warning(disable : 26439) // This kind of function may not throw. Declare it 'noexcept'
#include <bullet/btBulletDynamicsCommon.h>
#pragma warning(pop)
#include "Handling.h"
#include "../Cars/Porsche_911_Carrera_S/Porsche911.h"
#include "../Cars/Audi_R8_LMS/AudiR8.h"
#include "../Cars/Honda_Civic_EG6/Civic.h"
#define LERP(a, b, t) (a + t * (b - a))

#define clamp(val, low, high) ((val) < (low) ? (low) : ((val) > (high) ? (high) : (val)))



Handling::Handling()
{
        
}


bool Handling::CheckResetRequested() {
    bool rIsDown = GetAsyncKeyState('R') & 0x8000;
    bool request = (rIsDown && !rWasPressed);
    rWasPressed = rIsDown;
    if (request) {
        for (int i = 0; i < 4; i++) {
            m_wheels[i].compression = 0.0f;
            m_wheels[i].suspensionForce = 0.0f;
            m_wheels[i].isGrounded = false;
        }
    }
    return request;
}

void Handling::SetVehicleDefinition(
    const VehicleDefinition& definition)    
{
    m_vehicle = definition;
    m_torqueManager.SetVehicleDefinition(definition);


    for (int i = 0; i < 4; i++)
    {
        m_wheels[i].localOffset = m_vehicle.wheelOffsets[i];    
       
        if (i < 2)
        {
            m_tyres[i].SetTyreDefinition(definition.frontTyres);
            m_wheels[i].wheelInertia = m_vehicle.frontTyres.frontWheelInertia;
        }
        else
        {
            m_tyres[i].SetTyreDefinition(definition.rearTyres);
            m_wheels[i].wheelInertia = m_vehicle.rearTyres.rearWheelInertia;
        }
    }
}


void Handling::ProcessSuspensionData(int wheelIdx, WheelRayHit hitData) {
    float droopExtension = 0.1f;
    if (hitData.isGrounded && hitData.hitDist <= (m_vehicle.suspensionRestLength + droopExtension)) {

        float diff = m_vehicle.suspensionRestLength - hitData.hitDist;
        m_wheels[wheelIdx].worldPos = hitData.hitPointWorld;
        m_wheels[wheelIdx].localVelocity = hitData.wheelVelocity;
        m_wheels[wheelIdx].isGrounded = true;


        g_DebugTelemetry.diff[wheelIdx] = diff;
        float rawCompression = diff / m_vehicle.suspensionRestLength;
        m_wheels[wheelIdx].compression = clamp(rawCompression, 0.0f, 1.0f);
    }
    else {
        m_wheels[wheelIdx].isGrounded = false;
        m_wheels[wheelIdx].compression = 0.0f;
        g_DebugTelemetry.diff[wheelIdx] = 0.0f;
    }
}

float Handling::CalculateSlipRatioGrip(float slipRatio)
{
    float s = abs(slipRatio);

    if (s < 0.08f)
        return 1.0f;

    if (s < 0.35f)
    {
        float t = (s - 0.08f) / 0.27f;
        t = t * t * (3.0f - 2.0f * t);
        return 1.0f - 0.15f * t; // 1.00 -> 0.85
    }

    if (s < 1.2f)
    {
        float t = (s - 0.35f) / 0.85f;
        t = t * t * (3.0f - 2.0f * t);
        return 0.85f - 0.25f * t; // 0.85 -> 0.60
    }

    return 0.60f;
}
    
SuspensionResults Handling::CalculateSuspension() {
    float internalDelta = 1.0f / 300.0f;
    SuspensionResults results;
    btTransform tr;
    m_carBody->getMotionState()->getWorldTransform(tr);
    btVector3 carUp = tr.getBasis().getColumn(1);
    float frontRollForce =
        ((m_wheels[0].compression - m_wheels[1].compression)
            * m_vehicle.suspensionRestLength)
        * m_vehicle.frontARBStiffness;
    float rearRollForce = ((m_wheels[2].compression - m_wheels[3].compression) * m_vehicle.suspensionRestLength) * m_vehicle.rearARBStiffness;
    
        

    for (int i = 0; i < 4; i++) {

        results.wheels[i].active = false;
        if (!m_wheels[i].isGrounded) continue;

        btVector3 wheelWorldPos = tr * m_wheels[i].localOffset;
        btVector3 relPos = wheelWorldPos - tr.getOrigin();


        float springVel =
            ((m_wheels[i].compression - m_wheels[i].lastCompression)
                * m_vehicle.suspensionRestLength)
            / internalDelta;
        m_wheels[i].suspensionRelativeVelocity = springVel;

        float vel = m_wheels[i].suspensionRelativeVelocity; 

        bool front = i < 2;

        float springRate = front
            ? m_vehicle.suspension.frontSpringRate
            : m_vehicle.suspension.rearSpringRate;

            float springForce =
                m_wheels[i].compression * m_vehicle.suspensionRestLength * springRate;

        float absVel = abs(vel);

        float currentDamping = 0.0f;
  
        if (vel > 0.0f)
        {
            float threshold = front
                ? m_vehicle.suspension.frontFastBumpThreshold
                : m_vehicle.suspension.rearFastBumpThreshold;

            currentDamping = front
                ? (absVel > threshold ? m_vehicle.suspension.frontFastBump : m_vehicle.suspension.frontSlowBump)
                : (absVel > threshold ? m_vehicle.suspension.rearFastBump : m_vehicle.suspension.rearSlowBump);
        }
        else
        {
            float threshold = front
                ? m_vehicle.suspension.frontFastReboundThreshold
                : m_vehicle.suspension.rearFastReboundThreshold;

            currentDamping = front
                ? (absVel > threshold ? m_vehicle.suspension.frontFastRebound : m_vehicle.suspension.frontRebound)
                : (absVel > threshold ? m_vehicle.suspension.rearFastRebound : m_vehicle.suspension.rearRebound);
        }


        float dampingForce = vel * currentDamping * m_vehicle.suspension.dampingScale;

        float arbForce = 0.0f;
        if (i == 0) arbForce = -frontRollForce;
        if (i == 1) arbForce = frontRollForce;
        if (i == 2) arbForce = -rearRollForce;
        if (i == 3) arbForce = rearRollForce;

        float bumpStopForce = 0.0f;
        if (m_wheels[i].compression > m_vehicle.suspension.bumpStopStart)
        {
            float extra =
                (
                    m_wheels[i].compression -
                    m_vehicle.suspension.bumpStopStart
                    )
                * m_vehicle.suspensionRestLength;

            bumpStopForce =
                extra * m_vehicle.suspension.bumpStopRate;
        }
        float totalForceScalar = springForce + dampingForce + arbForce + bumpStopForce;

        if (totalForceScalar < 0.0f)
            totalForceScalar = 0.0f;

        m_wheels[i].suspensionForce = totalForceScalar;

        if (totalForceScalar > 0) {
            btVector3 finalForce = carUp * totalForceScalar;
            btVector3 worldOffset = tr.getBasis() * m_wheels[i].localOffset;

            results.wheels[i].force[0] = finalForce.x();
            results.wheels[i].force[1] = finalForce.y();
            results.wheels[i].force[2] = finalForce.z();

            results.wheels[i].offset[0] = relPos.x();
            results.wheels[i].offset[1] = relPos.y();
            results.wheels[i].offset[2] = relPos.z();
            results.wheels[i].active = true;
        }
        m_wheels[i].lastCompression = m_wheels[i].compression;

        g_DebugTelemetry.dampingForce[i] = dampingForce;
    }

    return results;
}


PhysicsAdvice Handling::CalculateFriction(float steeringInput, const btTransform& tr, float currentSpeed, float carMass, float throttle, float brake) {

    PhysicsAdvice advice;
    float internalDelta = 1.0f / 300.0f;

    float speedKMH = currentSpeed;
    float lowSpeedAngle = m_vehicle.lowSpeedSteerAngle;
    float highSpeedAngle = m_vehicle.highSpeedSteerAngle;
    float speedNormalization = clamp(speedKMH / 250.0f, 0.0f, 1.0f);
    float currentMaxSteer = lowSpeedAngle + (speedNormalization * (highSpeedAngle - lowSpeedAngle));
    float speedFactor = clamp(speedKMH / 180.0f, 0.0f, 1.0f);

    float steerInSpeed = 4.0f - speedFactor * 1.5f;

    float returnSpeed = 8.0f - speedFactor * 2.0f;

    float steeringSmoothSpeed = (abs(steeringInput) < 0.1f) ? returnSpeed : steerInSpeed;

    float steeringAlpha = clamp(steeringSmoothSpeed * internalDelta, 0.0f, 1.0f);

    smoothedSteering = LERP(smoothedSteering, steeringInput, steeringAlpha);
    
    float steeringValue = smoothedSteering * currentMaxSteer;

    btVector3 worldVel = m_carBody->getLinearVelocity();
    btVector3 carRight = tr.getBasis().getColumn(0);
    btVector3 carUp = tr.getBasis().getColumn(1);
    btVector3 carForward = tr.getBasis().getColumn(2);


    float totalLatForce = 0.0f;
    float totalLongForce = 0.0f;

    for (int i = 0; i < 4; i++)
    {
        totalLatForce += m_wheels[i].lateralForce;
        totalLongForce += m_wheels[i].longitudinalForce;
    }

    float lateralG =
        totalLatForce /
        (m_vehicle.mass * 9.81f);

    float longitudinalG =
        totalLongForce /
        (m_vehicle.mass * 9.81f);


    float bodySideVel =
        worldVel.dot(carRight);

    float bodyForwardVel =
        worldVel.dot(carForward);

    float bodySlipAngle = 0.0f;

    if (abs(bodyForwardVel) > 0.5f)
    {
        bodySlipAngle =
            atan2(bodySideVel, abs(bodyForwardVel));
    }


    for (int i = 0; i < 4; i++) {
        advice.friction[i].active = false;
        if (!m_wheels[i].isGrounded) {
            m_wheels[i].lastCompression = 0.0f;
            m_wheels[i].suspensionForce = 0.0f;
            continue;
        }



        float currentWheelTheta = 0.0f;
        float currentWheelMu = 0.0f;
        float telemetryVx = 0.0f;
        float telemetryVy = 0.0f;

        float speed = m_carBody->getLinearVelocity().length();

        float springLoad = max(0.0f, m_wheels[i].suspensionForce);

        float rearWeightDistribution = 1.0f - m_vehicle.frontWeightDistribution;

        float staticFrontLoad =
            m_vehicle.mass * 9.81f * m_vehicle.frontWeightDistribution * 0.5f;

        float staticRearLoad =
            m_vehicle.mass * 9.81f * rearWeightDistribution * 0.5f;

        float aeroScale = 3.25f;

        float frontDownforce = speed * speed * m_vehicle.frontDownforceCoeff * aeroScale;
        float rearDownforce = speed * speed * m_vehicle.rearDownforceCoeff * aeroScale;

        float frontAeroPerWheel = frontDownforce * 0.5f;
        float rearAeroPerWheel = rearDownforce * 0.5f;

        float baseLoad =
            (i < 2) ? staticFrontLoad : staticRearLoad;

        float aeroLoad =
            (i < 2) ? frontAeroPerWheel : rearAeroPerWheel;

        float dynamicDelta = springLoad - baseLoad;

        float rawLoad =
            baseLoad +
            aeroLoad +
            dynamicDelta * m_vehicle.loadTransferStrength;

        float loadResponseRate = m_vehicle.loadResponseRate;
        float loadAlpha = 1.0f - exp(-loadResponseRate * internalDelta);

        float effectiveLoad = clamp(rawLoad, 400.0f, 45000.0f);


        m_wheels[i].smoothedNormalLoad +=
            (effectiveLoad - m_wheels[i].smoothedNormalLoad) * loadAlpha;

        float tyreLoad = m_wheels[i].smoothedNormalLoad;
         
        float steeringAngle = 0.0f;
        if (i < 2) {
            float ack = (steeringValue > 0) ? (i == 1 ? 1.12f : 0.92f) : (i == 0 ? 1.12f : 0.92f);
            steeringAngle = steeringValue * ack;
        }
        else {
            steeringAngle = 0.0f;
        }

        float localSideVel = m_wheels[i].localVelocity.x();
        float localForwardVel = m_wheels[i].localVelocity.z();

    
        float currentVx = localForwardVel;
        float currentVy = localSideVel;

        float cosA = cos(steeringAngle);
        float sinA = sin(steeringAngle);

        float tireSlipVel = (localSideVel * cosA) - (localForwardVel * sinA);
        float tireForwardVel = (localSideVel * sinA) + (localForwardVel * cosA);
        float slipRatio = 0.0f;

        float wheelRadius = (i < 2)
            ? m_vehicle.frontTyres.wheelRadius
            : m_vehicle.rearTyres.wheelRadius;


    
        float wheelLinearSpeed = m_wheels[i].angularVelocity * wheelRadius;
        float roadSpeed = tireForwardVel;
            slipRatio = (wheelLinearSpeed - roadSpeed) / max(abs(roadSpeed), 1.0f);



        float slipAngle = 0.0f;
        if (abs(tireForwardVel) > 0.5f) {
            float slipDenom = max(abs(tireForwardVel), 8.0f);

            slipAngle = atan2(tireSlipVel, slipDenom);
        }

        float compressionTravel = m_wheels[i].compression * m_vehicle.suspensionRestLength;
        

        float camberDeg = (i < 2)
            ? m_vehicle.frontTyres.staticCamber
            : m_vehicle.rearTyres.staticCamber;

        float camberGain = (i < 2)
            ? m_vehicle.frontTyres.CamberGain
            : m_vehicle.rearTyres.CamberGain;

        camberDeg += compressionTravel * camberGain;


        float frictionForceScalar = m_tyres[i].Update(tyreLoad, slipAngle, speedKMH, currentVx, currentVy, currentWheelTheta, currentWheelMu, telemetryVx, telemetryVy, camberDeg, i, slipRatio);
      
        float slipGrip = CalculateSlipRatioGrip(slipRatio);

        frictionForceScalar *= slipGrip;    

        m_wheels[i].lateralForce = frictionForceScalar;

        constexpr float RAD_TO_DEG = 180.0f / 3.14159265359f;

        float yawRateDeg =
            m_carBody->getAngularVelocity().y() * RAD_TO_DEG;

        btVector3 wheelRightDir = carRight;
        if (abs(steeringAngle) > 0.0001f) {
            wheelRightDir = btMatrix3x3(btQuaternion(carUp, steeringAngle)) * carRight;
        }


        advice.friction[i].force = wheelRightDir * frictionForceScalar;
        advice.friction[i].offset = tr.getBasis() * m_wheels[i].localOffset;
        advice.friction[i].active = true;

        float bodySlipDeg =
            bodySlipAngle * 57.2958f;

    
        m_wheels[i].slipRatio = slipRatio;
        m_wheels[i].normalLoad = tyreLoad;

        g_DebugTelemetry.camberCompression[i] = compressionTravel;
        g_DebugTelemetry.camberDeg[i] = camberDeg;
        g_DebugTelemetry.bodySideVel[i] = bodySideVel;
        g_DebugTelemetry.bodyForwardVel[i] = bodyForwardVel;
        g_DebugTelemetry.bodySlipAngle[i] = bodySlipDeg;
        g_DebugTelemetry.localForwardVel[i] = localForwardVel;
        g_DebugTelemetry.localSideVel[i] = localSideVel;
        g_DebugTelemetry.slip[i] = slipAngle;
        g_DebugTelemetry.frictionForce[i] = frictionForceScalar;
        g_DebugTelemetry.compression[i] = m_wheels[i].compression;
        g_DebugTelemetry.wheelLoad[i] = effectiveLoad;
        g_DebugTelemetry.wheelLoadSmoothed[i] = tyreLoad;
        g_DebugTelemetry.steeringAngle[i] = steeringAngle;
        g_DebugTelemetry.lateralForce[i] = frictionForceScalar;
        g_DebugTelemetry.tyreTheta[i] = currentWheelTheta;
        g_DebugTelemetry.surfaceMu[i] = currentWheelMu;
        g_DebugTelemetry.wheelVx[i] = telemetryVx;
        g_DebugTelemetry.wheelVy[i] = telemetryVy;
        g_DebugTelemetry.slipRatio[i] = slipRatio;
        g_DebugTelemetry.slipGrip[i] = slipGrip;
        g_DebugTelemetry.mass = m_vehicle.mass;
        g_DebugTelemetry.yawRate = yawRateDeg;

        float gAlpha = 1.0f - exp(-12.0f * internalDelta);
        g_DebugTelemetry.lateralG += (lateralG - g_DebugTelemetry.lateralG) * gAlpha;
        g_DebugTelemetry.longitudinalG += (longitudinalG - g_DebugTelemetry.longitudinalG) * gAlpha;

    }
    return advice;
}

float Handling::CalculateLoadSensitiveMu(float normalLoad, float baseMu)
{
    float referenceLoad = 3500.0f; // typical static load per tyre
    float loadRatio = normalLoad / referenceLoad;

    loadRatio = clamp(loadRatio, 0.25f, 3.0f);

    float loadSensitivity = 0.12f;

    float mu = baseMu * (1.0f - loadSensitivity * (loadRatio - 1.0f));

    return clamp(mu, baseMu * 0.70f, baseMu * 1.15f);
}

float Handling::CalculateLongitudinalForce(float slipRatio, float normalLoad, float mu)
{
    float s = abs(slipRatio);   
    float effectiveMu = CalculateLoadSensitiveMu(normalLoad, mu);
    float peakSlip = 0.22f;
    float peakForce = normalLoad * effectiveMu;

    float force = 0.0f;

    if (s < peakSlip)
        force = peakForce * (s / peakSlip);

    else
    {
        float falloff = (s - peakSlip) / 1.0f;
        force = peakForce * (1.0f - 0.65f * falloff);
        force = clamp(force, peakForce * 0.40f, peakForce);
    }

    force = clamp(force, 0.0f, peakForce);

    return (slipRatio >= 0.0f) ? force : -force;
}

void Handling::ApplyFrontLimitedSlipDifferential(
    float& frontLeftTorque,
    float& frontRightTorque,
    float internalDelta)
{
    float diffSpeed =
        m_wheels[0].angularVelocity -
        m_wheels[1].angularVelocity;

    float targetLockTorque = 0.0f;

    const float diffDeadzone = 3.0f;

    if (abs(diffSpeed) > diffDeadzone)
    {
        float effectiveDiffSpeed =
            diffSpeed -
            ((diffSpeed > 0.0f)
                ? diffDeadzone
                : -diffDeadzone);

        targetLockTorque =
            effectiveDiffSpeed * m_vehicle.lockStrength;
    }



    targetLockTorque = clamp(
        targetLockTorque,
        -m_vehicle.diffMaxLockTorque,
        m_vehicle.diffMaxLockTorque);

    float lockAlpha =
        1.0f - exp(-18.0f * internalDelta);

    m_currentFrontLockTorque +=
        (targetLockTorque - m_currentFrontLockTorque) * lockAlpha;

    float loadL = m_wheels[0].normalLoad;
    float loadR = m_wheels[1].normalLoad;

    float loadSum = max(loadL + loadR, 1.0f);

    float loadBias =
        (loadR - loadL) / loadSum;

    float targetLoadBiasTorque =
        loadBias * m_vehicle.diffLoadBiasStrength;

    targetLoadBiasTorque = clamp(
        targetLoadBiasTorque,
        -m_vehicle.diffMaxLoadBiasTorque,
        m_vehicle.diffMaxLoadBiasTorque);

    float totalDiffTorque =
        m_currentFrontLockTorque + targetLoadBiasTorque;

    frontLeftTorque -= totalDiffTorque;
    frontRightTorque += totalDiffTorque;

    g_DebugTelemetry.diffSpeed = diffSpeed;
    g_DebugTelemetry.diffTargetTorque = targetLockTorque;
    g_DebugTelemetry.diffLockTorque[0] = m_currentFrontLockTorque;
    g_DebugTelemetry.diffLockTorque[1] = m_currentFrontLockTorque;
    g_DebugTelemetry.diffLoadBiasTorque = targetLoadBiasTorque;
    g_DebugTelemetry.diffTotalTorque = totalDiffTorque;
}


void Handling::ApplyRearLimitedSlipDifferential(
    float& rearLeftTorque,
    float& rearRightTorque,
    float internalDelta)
{
    float diffSpeed =
        m_wheels[2].angularVelocity -
        m_wheels[3].angularVelocity;

    float targetLockTorque = 0.0f;

    const float diffDeadzone = 3.0f;

    if (abs(diffSpeed) > diffDeadzone)
    {
        float effectiveDiffSpeed =
            diffSpeed -
            ((diffSpeed > 0.0f)
                ? diffDeadzone
                : -diffDeadzone);

        targetLockTorque =
            effectiveDiffSpeed * m_vehicle.lockStrength;
    }



    targetLockTorque = clamp(
        targetLockTorque,
        -m_vehicle.diffMaxLockTorque,
        m_vehicle.diffMaxLockTorque);

    float lockAlpha =
        1.0f - exp(-18.0f * internalDelta);
        
    m_currentRearLockTorque +=
        (targetLockTorque - m_currentRearLockTorque) * lockAlpha;

    float loadL = m_wheels[2].normalLoad;
    float loadR = m_wheels[3].normalLoad;

    float loadSum = max(loadL + loadR, 1.0f);

    float loadBias =
        (loadR - loadL) / loadSum;

    float targetLoadBiasTorque =
        loadBias * m_vehicle.diffLoadBiasStrength;

    targetLoadBiasTorque = clamp(
        targetLoadBiasTorque,
        -m_vehicle.diffMaxLoadBiasTorque,
        m_vehicle.diffMaxLoadBiasTorque);

    float totalDiffTorque =
        m_currentRearLockTorque + targetLoadBiasTorque;

    rearLeftTorque -= totalDiffTorque;
    rearRightTorque += totalDiffTorque;

    g_DebugTelemetry.diffSpeed = diffSpeed;
    g_DebugTelemetry.diffTargetTorque = targetLockTorque;
    g_DebugTelemetry.diffLockTorque[2] = m_currentRearLockTorque;
    g_DebugTelemetry.diffLockTorque[3] = m_currentRearLockTorque;
    g_DebugTelemetry.diffLoadBiasTorque = targetLoadBiasTorque; 
    g_DebugTelemetry.diffTotalTorque = totalDiffTorque;
}

PhysicsAdvice Handling::CalculateDrivingForces(float throttle, float brake, const btTransform& tr, float currentSpeed)
{
    PhysicsAdvice advice;

    const float internalDelta = 1.0f / 300.0f;
    const float speedMps = currentSpeed / 3.6f;

    for (int i = 0; i < 4; i++)
    {
        float wheelRadius = (i < 2)
            ? m_vehicle.frontTyres.wheelRadius
            : m_vehicle.rearTyres.wheelRadius;

        if (abs(m_wheels[i].angularVelocity) < 0.001f)
            m_wheels[i].angularVelocity = speedMps / wheelRadius;
    }

    float frontDrivenAngularVel =
        (abs(m_wheels[0].angularVelocity) + abs(m_wheels[1].angularVelocity)) * 0.5f;

    float rearDrivenAngularVel =
        (abs(m_wheels[2].angularVelocity) + abs(m_wheels[3].angularVelocity)) * 0.5f;


    float drivenWheelAngularVel =
        frontDrivenAngularVel * m_vehicle.frontTorqueDistribution +
        rearDrivenAngularVel * m_vehicle.rearTorqueDistribution;


    TorqueOutput torque =   
        m_torqueManager.Update(throttle, drivenWheelAngularVel, internalDelta); 

    float totalDriveTorque = torque.wheelTorque[2] + torque.wheelTorque[3];



    float frontAxleTorque = totalDriveTorque * m_vehicle.frontTorqueDistribution;
    float rearAxleTorque =  totalDriveTorque * m_vehicle.rearTorqueDistribution;

    float frontLeftTorque = frontAxleTorque * 0.5f;
    float frontRightTorque = frontAxleTorque * 0.5f;

    float rearLeftTorque = rearAxleTorque * 0.5f;
    float rearRightTorque = rearAxleTorque * 0.5f;

    if (m_vehicle.frontTorqueDistribution > 0.0f)
    {
        ApplyFrontLimitedSlipDifferential(
            frontLeftTorque,
            frontRightTorque,
            internalDelta);
    }

    if (m_vehicle.rearTorqueDistribution > 0.0f)
    {
        ApplyRearLimitedSlipDifferential(
            rearLeftTorque,
            rearRightTorque,
            internalDelta);
    }

    float wheelDriveTorque[4] = {};
    wheelDriveTorque[0] = frontLeftTorque;
    wheelDriveTorque[1] = frontRightTorque;
    wheelDriveTorque[2] = rearLeftTorque;
    wheelDriveTorque[3] = rearRightTorque;


    // 1. Wheel angular dynamics
    for (int i = 0; i < 4; i++)
    {
        float axleBrakeBias = (i < 2) ? m_vehicle.brakeBiasFront : m_vehicle.brakeBiasRear;
        float wheelDir = (m_wheels[i].angularVelocity >= 0.0f) ? 1.0f : -1.0f;


        float wheelRadius = (i < 2)
            ? m_vehicle.frontTyres.wheelRadius
            : m_vehicle.rearTyres.wheelRadius;

        m_wheels[i].driveTorque = wheelDriveTorque[i];


        m_wheels[i].brakeTorque =
            brake * m_vehicle.brakeTorqueStrength * axleBrakeBias * wheelRadius * wheelDir;


        float netTorque =
            m_wheels[i].driveTorque 
            - m_wheels[i].brakeTorque
            - m_wheels[i].tireReactionTorque;

        m_wheels[i].angularAcceleration =
            netTorque / m_wheels[i].wheelInertia;

        m_wheels[i].angularVelocity +=
            m_wheels[i].angularAcceleration * internalDelta;

        if (m_wheels[i].isGrounded)
        {
            float wheelForwardSpeed = m_wheels[i].localVelocity.z();
            float roadAngularVel =
                wheelForwardSpeed / wheelRadius;

            float coupling = 0.025f;

            if (brake > 0.01f)
                coupling = 0.04f;

            m_wheels[i].angularVelocity +=
                (roadAngularVel - m_wheels[i].angularVelocity) * coupling;
        }

        m_wheels[i].angularVelocity =
            clamp(m_wheels[i].angularVelocity, -500.0f, 500.0f);
    }

    // 2. Actual longitudinal forces
    int currentGear = m_torqueManager.GetCurrentGear();

    btVector3 carForward = tr.getBasis().getColumn(2);
    btVector3 velocity = m_carBody->getLinearVelocity();
    float forwardVel = velocity.dot(carForward);

    for (int i = 0; i < 4; i++)
    {
        advice.friction[i].active = false;

        if (!m_wheels[i].isGrounded)
            continue;

        float axleBrake = (i < 2) ? m_vehicle.brakeBiasFront : m_vehicle.brakeBiasRear;

        float drivingForce = 0.0f;
        float engineBrakeForce = 0.0f;

        float slipResponseRate = m_vehicle.slipResponseRate;
        float slipAlpha = 1.0f - exp(-slipResponseRate * internalDelta);

        m_wheels[i].filteredSlipRatio +=
            (m_wheels[i].slipRatio - m_wheels[i].filteredSlipRatio) * slipAlpha;

        float wheelRadius = (i < 2)
            ? m_vehicle.frontTyres.wheelRadius
            : m_vehicle.rearTyres.wheelRadius;
      
        float torqueForce = m_wheels[i].driveTorque / wheelRadius;

            float slipForce = CalculateLongitudinalForce(
                m_wheels[i].filteredSlipRatio,
                m_wheels[i].normalLoad,
                g_DebugTelemetry.surfaceMu[i]
            );

            float launchBlend =
                clamp(abs(forwardVel) / 5.0f, 0.0f, 1.0f);


            if (throttle < 0.05f && torque.currentGear > 0)
            {
                float engineBrakeTorque =
                    m_vehicle.engineBrakeTorque * (torque.engineRPM / m_vehicle.engine.redlineRPM);

                drivingForce = -engineBrakeTorque / wheelRadius;
            }
            else
            {
                drivingForce =
                    torqueForce * (1.0f - launchBlend) +
                    slipForce * launchBlend;
            }
       

        float brakeForceMag =
            brake * m_vehicle.brakeForceStrength * axleBrake;

        float finalBrakeForce = 0.0f;

        if (abs(forwardVel) > 0.1f)
        {
            float brakeDir =
                (forwardVel > 0.0f) ? -1.0f : 1.0f;

            float forceToStop =
                (m_vehicle.mass * 0.25f * abs(forwardVel)) / internalDelta;

            finalBrakeForce =
                min(brakeForceMag, forceToStop) * brakeDir;
        }

        float totalForce =
            drivingForce + finalBrakeForce;

        if (currentGear > 0)
        {
            if (forwardVel < 0.1f && totalForce < 0.0f)
                totalForce = max(0.0f, totalForce);
        }
        else if (currentGear < 0)
        {
            if (drivingForce < 0.0f)
            {
                totalForce = drivingForce + finalBrakeForce;
            }
            else
            {
                if (forwardVel > -0.1f && totalForce > 0.0f)
                    totalForce = 0.0f;
            }
        }

        if (drivingForce > 0.0f)
            m_wheels[i].tireReactionTorque = drivingForce * wheelRadius;
        else
            m_wheels[i].tireReactionTorque = 0.0f;

        m_wheels[i].longitudinalForce = totalForce;

        advice.friction[i].force =
            carForward * totalForce;

        advice.friction[i].offset =
            tr.getBasis() * m_wheels[i].localOffset;

        advice.friction[i].active = true;
        g_DebugTelemetry.longitudinalForce[i] = totalForce;
        g_DebugTelemetry.wheelTorque[i] = m_wheels[i].driveTorque;
        g_DebugTelemetry.throttle = throttle;
        g_DebugTelemetry.brake = brake;
    }

    return advice;
}

void Handling::ApplyYawDamping()
{
    btTransform tr;
    m_carBody->getMotionState()->getWorldTransform(tr);

    btVector3 carUp = tr.getBasis().getColumn(1);
    btVector3 angVel = m_carBody->getAngularVelocity();

    float yawRate = angVel.dot(carUp);

    float dampingTorque = -yawRate * m_vehicle.yawDamping;

    m_carBody->applyTorque(carUp * dampingTorque);
}

PhysicsAdvice Handling::CombinedForces(
    float steeringInput,
    const btTransform& tr,
    float currentSpeed,
    float carMass,
    float throttle,
    float brake)
{

    ApplyYawDamping();


    PhysicsAdvice lateralAdvice = CalculateFriction(
        steeringInput, tr, currentSpeed, carMass, throttle, brake);

    PhysicsAdvice driveAdvice = CalculateDrivingForces(
        throttle, brake, tr, currentSpeed);

    PhysicsAdvice combinedAdvice = {};

 

    for (int i = 0; i < 4; i++)
    {
        combinedAdvice.friction[i].active = false;
        combinedAdvice.friction[i].force = btVector3(0, 0, 0);
        combinedAdvice.friction[i].offset = btVector3(0, 0, 0);

        if (!lateralAdvice.friction[i].active &&
            !driveAdvice.friction[i].active)
            continue;

        btVector3 lateralForce = lateralAdvice.friction[i].force;
        btVector3 longForce = driveAdvice.friction[i].force;

        float effectiveLoad = m_wheels[i].normalLoad;
        float mu = g_DebugTelemetry.surfaceMu[i];

        float maxGrip = effectiveLoad * mu * 1.35f;

        float latMag = lateralForce.length();
        float longMag = longForce.length();

    
        float total = sqrt(latMag * latMag + longMag * longMag);

        if (total > maxGrip && total > 0.001f)
        {
            float scale = maxGrip / total;
            lateralForce *= scale;
            longForce *= scale;
        }

        g_DebugTelemetry.combinedGripScale[i] =
            (total > maxGrip && total > 0.001f) ? maxGrip / total : 1.0f;

        g_DebugTelemetry.combinedMaxGrip[i] = maxGrip;
        g_DebugTelemetry.combinedTotalForce[i] = total;
      

        btVector3 combinedForce = lateralForce + longForce;
  
        combinedAdvice.friction[i].force = combinedForce;
        combinedAdvice.friction[i].offset = lateralAdvice.friction[i].active
            ? lateralAdvice.friction[i].offset
            : driveAdvice.friction[i].offset;

        combinedAdvice.friction[i].active = true;
    }

    return combinedAdvice;
}

void Handling::Cleanup() {

}