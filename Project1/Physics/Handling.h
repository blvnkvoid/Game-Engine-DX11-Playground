#pragma once
#include "../SharedVehicleTypes.h"
#include "TorqueManager.h"
#include "Tyres.h"

class Handling {
public:
	Handling();
	~Handling() { Cleanup(); }
	void Cleanup();
	void SetPhysicsPointers(btRigidBody* body) {m_carBody = body;}		
	void ProcessSuspensionData(int wheelIdx, WheelRayHit hitData);
	bool CheckResetRequested();
	SuspensionResults CalculateSuspension();
	btVector3 GetWheelLocalOffset(int i) const { return m_wheels[i].localOffset; }
	PhysicsAdvice CalculateFriction(float steeringInput, const btTransform& tr, float currentSpeed, float carMass, float throttle, float brake);
	PhysicsAdvice CalculateDrivingForces(float throttle, float brake, const btTransform& tr, float currentSpeed);
	PhysicsAdvice CombinedForces(float steeringInput,
		const btTransform& tr,
		float currentSpeed,
		float carMass,
		float throttle,
		float brake	);
	float CalculateSlipRatioGrip(float slipRatio);
	float CalculateLongitudinalForce(float slipRatio, float normalLoad, float mu);
	float CalculateLoadSensitiveMu(float normalLoad, float baseMu);
	void SetVehicleDefinition(const VehicleDefinition& definition);
	void ApplyRearLimitedSlipDifferential(
		float& rearLeftTorque,
		float& rearRightTorque,
		float internalDelta);	
	
	void ApplyFrontLimitedSlipDifferential(
		float& frontLeftTorque,
		float& frontRightTorque,
		float internalDelta);

	void ApplyYawDamping();
private:
	float smoothedSteering = 0.0f;
	float m_currentFrontLockTorque = 0.0f;
	float m_currentRearLockTorque = 0.0f;
	bool rWasPressed = false;
	btVector3 m_lastVelocity = btVector3(0, 0, 0);
	float wheelDriveTorque[4];
	WheelInfo m_wheels[4];
	btRigidBody* m_carBody;
	TorqueManager m_torqueManager;
	Tire m_tyres[4];
	VehicleDefinition m_vehicle;
};