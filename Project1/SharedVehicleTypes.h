#pragma once

#pragma warning(push)
#pragma warning(disable : 26495) // Uninitialized member variable
#include <bullet/LinearMath/btVector3.h>
#pragma warning(pop)
#include <string>

struct WheelRayHit {
    bool isGrounded;
    float hitDist;
    btVector3 hitPointWorld;
    btVector3 wheelVelocity;
};

struct WheelForceResult {
    btVector3 force;
    btVector3 offset; //Relative offset from body center/COM for applyForce
    bool active;
};

struct PhysicsAdvice {
    WheelForceResult suspension[4];
    WheelForceResult friction[4];
};

struct SuspensionPacket {
    float force[3];    // Direction and magnitude (carUp * suspensionForce)
    float offset[3];   // Where the wheel is relative to car center
    bool active;        // To handle the "isGrounded" check
};

struct SuspensionResults {
    SuspensionPacket wheels[4];
};

struct TorqueOutput {
    float wheelTorque[4];   // Nm per wheel
    float engineRPM;        // For your UI/Sound
    float driveTorque;      // Torque before the differential
    int currentGear;
};

struct TelemetryData {
    float carY;
    float groundY;
    float diff[4];
    float compression[4];
    float speed;
    int   gear;
    float rpm;
    float torque;
    float wheelLoad[4];
    float slip[4];
    float frictionForce[4];
    float drivingForce;
    float tyreSideStiffness[4];
    float maxFriction[4];
    float steeringAngle[4];
    float lateralForce[4];
    float tyreTheta[4];
    float surfaceMu[4];
    float wheelVx[4];
    float wheelVy[4];
    bool rayHit;
    float yNormal;
    float wheelTorque[4];
    float slipRatio[4];
    float slipGrip[4];
    float throttle;
    float brake;
    float longitudinalForce[4];
    float wheelLoadSmoothed[4];
    float dampingForce[4];
    float localForwardVel[4];
    float localSideVel[4];
    float bodySlipAngle[4];
    float bodyForwardVel[4];
    float bodySideVel[4];
    float diffLockTorque[4];
    float driveTorque[4];
    float diffSpeed;
    float diffTargetTorque;
    float dragMagnitude;
    float downforceMagnitude;
    float camberDeg[4];
    float camberCompression[4];
    float torqueScale;
    float frontDownforce;
    float rearDownforce;
    float diffLoadBiasTorque;
    float diffTotalTorque;
    float combinedSlip[4];
    float combinedGripScale[4];
    float combinedMaxGrip[4];
    float combinedTotalForce[4];
    float mass; 
    float lateralG;
    float longitudinalG;
    float avgSlipAngle;
    float avgSlipRatio;
    float yawRate;
};

extern TelemetryData g_DebugTelemetry;

struct SharedVehicleInputs {
    float throttle;     // 0.0 (idle) to 1.0 (NUKE)
    float brake;        // 0.0 to 1.0
    float steering;     // -1.0 (Full Left) to 1.0 (Full Right)
    bool handbrake;     // Spacebar or Circle button
    bool shiftUp;       // R2
    bool shiftDown;     // L2
};

struct WheelInfo {
    // Geometry / contact
    float radius = 0.34f;
    btVector3 localOffset = btVector3(0, 0, 0);
    btVector3 worldPos = btVector3(0, 0, 0);
    btVector3 localVelocity = btVector3(0, 0, 0);

    bool isGrounded = false;

    // Suspension
    float compression = 0.0f;
    float lastCompression = 0.0f;
    float suspensionRelativeVelocity = 0.0f;
    float suspensionForce = 0.0f;

    // Wheel rotation / drivetrain
    float angularVelocity = 0.0f;      // rad/s
    float angularAcceleration = 0.0f;  // rad/s^2
    float wheelInertia = 1.0f;

    float driveTorque = 0.0f;          // Nm
    float brakeTorque = 0.0f;          // Nm
    float tireReactionTorque = 0.0f;   // Nm

    // Tire state
    float slipAngle = 0.0f;
    float slipRatio = 0.0f;
    float filteredSlipRatio = 0.0f;

    // Tire outputs
    float lateralForce = 0.0f;
    float longitudinalForce = 0.0f;
    float normalLoad = 0.0f;
    float combinedScale = 1.0f;
    float smoothedNormalLoad = 0.0f;
    float staticReferenceLoad = 0.0f;


};


struct GearboxDefinition
{
    float finalDrive = 0.0f;
    float gearRatios[9] = {};
    int gearCount = 0;
};

struct TorqueCurvePoint
{
    float rpm = 0.0f;
    float torqueMultiplier = 0.0f;
};



struct EngineDefinition
{
    float idleRPM = 0.0f;
    float redlineRPM = 0.0f;
    float limiterRPM = 0.0f;
    float maxTorque = 0.0f;


    TorqueCurvePoint torqueCurve[8];
    int torqueCurveCount = 0;
};

struct TyreDefinition
{
    float frictionCoeff = 0.0f;
    float stiffness = 0.0f;
    float loadSmoothing = 0.0f;
    float relaxationRate = 0.0f;
    float staticCamber = 0.0f;
    float camberGripStrength = 0.0f;
    float CamberGain = 0.0f;
    float wheelRadius = 0.0f;
    float frontWheelInertia = 0.0f;
    float rearWheelInertia = 0.0f;
};

struct SuspensionDefinition
{
    float frontSpringRate = 0.0f;
    float rearSpringRate = 0.0f;

    float frontSlowBump = 0.0f;
    float frontFastBump = 0.0f;
    float frontFastBumpThreshold = 0.0f;
    float frontRebound = 0.0f;

    float rearSlowBump = 0.0f;
    float rearFastBump = 0.0f;
    float rearFastBumpThreshold = 0.0f;
    float rearRebound = 0.0f;

    float bumpStopRate = 0.0f;
    float bumpStopStart = 0.0f;
    float dampingScale = 0.0f;

    float frontFastReboundThreshold = 0.0f;
    float rearFastReboundThreshold = 0.0f;

    float frontFastRebound = 0.0f;
    float rearFastRebound = 0.0f;


};



struct AudioDefinition
{
    std::string basePath;

    std::string idle;
    std::string lowOn;
    std::string midOn;
    std::string highOn;
    std::string topFull;
    std::string limiter;

    std::string transmission;
    std::string gearUp;
    std::string gearDown;

    std::string tyreRolling;
    std::string tyreSkid;
    std::string wind;

    float idleRPM = 0.0f;
    float lowRPM = 0.0f;
    float midRPM = 0.0f;
    float highRPM = 0.0f;
    float topRPM = 0.0f;

    float idleWidth = 0.0f;
    float lowWidth = 0.0f;
    float midWidth = 0.0f;
    float highWidth = 0.0f;
    float topWidth = 0.0f;
};


struct VehicleDefinition
{
    float mass = 0.0f;

    float suspensionRestLength = 0.0f;
    float springStiffness = 0.0f;
    float springDamping = 0.0f;
    float frontARBStiffness = 0.0f;
    float rearARBStiffness = 0.0f;

    float brakeBiasFront = 0.0f;
    float brakeBiasRear = 0.0f;

    float brakeTorqueStrength = 0.0f;
    float brakeForceStrength = 0.0f;



    float engineBrakeTorque = 0.0f;

    float loadResponseRate = 0.0f;
    float slipResponseRate = 0.0f;

    float lowSpeedSteerAngle = 0.0f;
    float highSpeedSteerAngle = 0.0f;

    float lockStrength = 0.0f;
    float preload = 0.0f;
    float diffMaxLockTorque = 0.0f; 
    float diffLoadBiasStrength = 0.0f;
    float diffMaxLoadBiasTorque = 0.0f;
    float frontTorqueDistribution = 0.0f;
    float rearTorqueDistribution = 0.0f;

    float dragCoefficient = 0.0f;
    float downforceCoefficient = 0.0f;
    float frontDownforceCoeff = 0.0f;
    float rearDownforceCoeff = 0.0f;



    float frontWeightDistribution = 0.0f;
    float loadTransferStrength = 0.0f;
    float yawInertiaScale = 1.0f;
    float yawDamping = 0.0f;
    float rollInertiaScale = 1.0f;
    float pitchInertiaScale = 1.0f;

    btVector3 wheelOffsets[4];

    EngineDefinition engine;
    GearboxDefinition gearbox;
    TyreDefinition frontTyres;
    TyreDefinition rearTyres;
    SuspensionDefinition suspension;
    AudioDefinition audio;


};


