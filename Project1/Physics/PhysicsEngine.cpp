#pragma warning(push)
#pragma warning(disable : 26451) // Arithmetic overflow (the one you just found)
#pragma warning(disable : 26495) // Uninitialized member variable
#pragma warning(disable : 26812) // Prefer 'enum class' over 'enum'
#pragma warning(disable : 4099)  // Type name first seen using 'class' now seen using 'struct'
#pragma warning(disable : 4244)  // Conversion from 'double' to 'btScalar', possible loss of data
#pragma warning(disable : 26439) // This kind of function may not throw. Declare it 'noexcept'
#include <bullet/btBulletDynamicsCommon.h>
#pragma warning(pop)

#include "PhysicsEngine.h"
#include "../Input/Input.h"
#include "../SharedVehicleTypes.h"
#include "Spawner.h"



PhysicsEngine::PhysicsEngine()
{

    m_passedStartMeta = false;
    m_passedSector1 = false;
    m_passedSector2 = false;

}

void PhysicsEngine::Initialize() {
    m_collisionConfig = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfig);
    gContactAddedCallback = PhysicsEngine::CustomInternalEdgeCallback;  
    m_broadphase = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver();
    m_dynamicsWorld = new btDiscreteDynamicsWorld(
        m_dispatcher, m_broadphase, m_solver, m_collisionConfig
    );
    m_dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
    m_dynamicsWorld->getSolverInfo().m_globalCfm = 0.00000f;
    m_dynamicsWorld->getSolverInfo().m_erp2 = 0.02f;
}
void PhysicsEngine::SetVehicleDefinition(
    const VehicleDefinition& definition)
{
    m_vehicle = definition;
}

void PhysicsEngine::SetHandling(Handling* handling)
{
    m_handling = handling;
}


void PhysicsEngine::CreatePhysicsWorld() {
    

    Spawner spawn;
    m_groundBody = nullptr;

    btBoxShape* box = new btBoxShape(btVector3(0.925f, 0.64f, 2.275f));
    m_collisionShapes.push_back(box);

    btCompoundShape* compound = new btCompoundShape();
    m_collisionShapes.push_back(compound);

    btTransform localTrans;
    localTrans.setIdentity();
    localTrans.setOrigin(btVector3(0.0f, 0.64f, 0.0f));
    compound->addChildShape(localTrans, box);


    btVector3 localInertia(0, 0, 0);
    compound->calculateLocalInertia(m_vehicle.mass, localInertia);
    localInertia.setY(localInertia.y()  * m_vehicle.yawInertiaScale);
    localInertia.setX(localInertia.x() * m_vehicle.pitchInertiaScale);
    localInertia.setZ(localInertia.z() * m_vehicle.rollInertiaScale);


    btTransform carTransform = spawn.PutCar();

    btDefaultMotionState* carMotionState = new btDefaultMotionState(carTransform);
    m_motionStates.push_back(carMotionState);

    btRigidBody::btRigidBodyConstructionInfo carRBInfo(m_vehicle.mass, carMotionState, compound, localInertia);
    m_carBody = new btRigidBody(carRBInfo);
    carRBInfo.m_localInertia = localInertia;

    m_handling->SetPhysicsPointers(m_carBody);

    m_carBody->setMassProps(m_vehicle.mass, localInertia);
    m_carBody->updateInertiaTensor();

    m_carBody->setActivationState(ACTIVE_TAG);
    m_carBody->setDeactivationTime(1.0f);
    m_carBody->setSleepingThresholds(0.1f, 0.1f);
    m_carBody->setRestitution(0.0f);
    m_carBody->setFriction(0.02f);

    m_dynamicsWorld->addRigidBody(m_carBody, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::AllFilter);

    m_carBody->setCcdMotionThreshold(1.0f / 300.0f);
    m_carBody->setCcdSweptSphereRadius(0.2f);
}

DirectX::XMMATRIX PhysicsEngine::btTransformToXMMATRIX(const btTransform& worldTransform) {
    btScalar m[16];
    worldTransform.getOpenGLMatrix(m);
    DirectX::XMMATRIX mat = DirectX::XMMATRIX(
        (float)m[0], (float)m[1], (float)m[2], (float)m[3],
        (float)m[4], (float)m[5], (float)m[6], (float)m[7],
        (float)m[8], (float)m[9], (float)m[10], (float)m[11],
        (float)m[12], (float)m[13], (float)m[14], (float)m[15]
    );
    return mat;
}

void PhysicsEngine::GetCarTransform(btTransform& outTrans) {
    if (m_carBody) m_carBody->getMotionState()->getWorldTransform(outTrans);
    else outTrans.setIdentity();
}

void PhysicsEngine::AddTriangleMeshCollider(const std::vector<SharedVertex>& vertices, const std::vector<UINT>& indices) {
    if (vertices.empty() || indices.empty()) return;

    btIndexedMesh part;
    part.m_numTriangles = indices.size() / 3;
    part.m_triangleIndexBase = (const unsigned char*)indices.data();
    part.m_triangleIndexStride = 3 * sizeof(UINT);
    part.m_numVertices = vertices.size();
    part.m_vertexBase = (const unsigned char*)&vertices[0].pos;
    part.m_vertexStride = sizeof(SharedVertex);
    part.m_indexType = PHY_INTEGER;
    part.m_vertexType = PHY_FLOAT;

    btTriangleIndexVertexArray* meshInterface = new btTriangleIndexVertexArray();
    meshInterface->addIndexedMesh(part, PHY_INTEGER);

    btBvhTriangleMeshShape* trimeshShape = new btBvhTriangleMeshShape(meshInterface, false);
    trimeshShape->setMargin(0.0f);
    m_collisionShapes.push_back(trimeshShape);

    btTriangleInfoMap* infoMap = new btTriangleInfoMap();
    btGenerateInternalEdgeInfo(trimeshShape, infoMap);
    trimeshShape->setTriangleInfoMap(infoMap);

    btTransform trans;
    trans.setIdentity();

    btDefaultMotionState* motionState = new btDefaultMotionState(trans);
    m_motionStates.push_back(motionState);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, trimeshShape, btVector3(0, 0, 0));
    btRigidBody* body = new btRigidBody(rbInfo);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    body->setFriction(0.05f);
    body->setRestitution(0.0f);

    m_dynamicsWorld->addRigidBody(body, btBroadphaseProxy::StaticFilter, btBroadphaseProxy::AllFilter);
    m_trackBodies.push_back(body);
    m_meshInterfaces.push_back(meshInterface);
}

void PhysicsEngine::ResetCar() {
    if (!m_carBody) return;

    m_carBody->setLinearVelocity(btVector3(0, 0, 0));
    m_carBody->setAngularVelocity(btVector3(0, 0, 0));
    m_carBody->clearForces();


    Spawner spawn;

    btTransform resetTr = spawn.ResetPos();
    m_carBody->setWorldTransform(resetTr);

    if (m_carBody->getMotionState()) {
        m_carBody->getMotionState()->setWorldTransform(resetTr);
    }

    m_carBody->activate(true);
    m_carBody->setActivationState(DISABLE_DEACTIVATION);
    m_carBody->applyCentralImpulse(btVector3(0, -0.01f, 0));
    m_carBody->setActivationState(ACTIVE_TAG);
}

void PhysicsEngine::Update(float deltaTime, SharedVehicleInputs inputs) {
    float internalDelta = 1.0f / 300.0f;

    if (m_handling->CheckResetRequested()) m_needsReset = true;
    if (m_needsReset) {
        this->ResetCar();
        m_needsReset = false;
        return;
    }

    btTransform tr;
    m_carBody->getMotionState()->getWorldTransform(tr);
    btVector3 currentVel = m_carBody->getLinearVelocity();
    float speedKmh = currentVel.length() * 3.6f;

    g_DebugTelemetry.speed = speedKmh;
    g_DebugTelemetry.carY = tr.getOrigin().y();

    m_carBody->activate(true);
    btVector3 carUp = tr.getBasis().getColumn(1);

    for (int i = 0; i < 4; i++) {
        btVector3 localOff = m_handling->GetWheelLocalOffset(i);
        float raycastOffsetUp = 1.0f;
        float restLength = 0.7f;
        float droopExtension = 0.1f;

        btVector3 rayStart = tr * (localOff + btVector3(0.0f, raycastOffsetUp, 0.0f));
        btVector3 rayDir = carUp * -1.0f;
        btVector3 rayEnd = rayStart + rayDir * (restLength + raycastOffsetUp + droopExtension);

        btCollisionWorld::ClosestRayResultCallback rayCallback(rayStart, rayEnd);
        m_dynamicsWorld->rayTest(rayStart, rayEnd, rayCallback);

        WheelRayHit hit;
        hit.isGrounded = rayCallback.hasHit();

        if (hit.isGrounded) {
            btVector3 normal = rayCallback.m_hitNormalWorld;
            if (normal.y() < 0.70f) {
                hit.isGrounded = false;
                hit.hitDist = restLength + droopExtension;
                hit.wheelVelocity = btVector3(0, 0, 0);
                hit.hitPointWorld = btVector3(0, 0, 0);
            }
            if (normal.y() > 0.95f) {
                normal = btVector3(0, 1, 0);
            }
            g_DebugTelemetry.yNormal = normal.y();
            g_DebugTelemetry.groundY = rayCallback.m_hitPointWorld.y();

            btVector3 wheelPointWorldOffset = tr.getBasis() * localOff;
            btVector3 velocityWorld = m_carBody->getVelocityInLocalPoint(wheelPointWorldOffset);

            hit.wheelVelocity.setX(velocityWorld.dot(tr.getBasis().getColumn(0)));
            hit.wheelVelocity.setY(velocityWorld.dot(tr.getBasis().getColumn(1)));
            hit.wheelVelocity.setZ(velocityWorld.dot(tr.getBasis().getColumn(2)));

            float rawHitDist = (rayCallback.m_hitPointWorld - rayStart).length() - raycastOffsetUp;
            hit.hitDist = rawHitDist;
            hit.hitPointWorld = rayCallback.m_hitPointWorld;
        }
        else {
            hit.hitDist = restLength + droopExtension;
            hit.wheelVelocity = btVector3(0, 0, 0);
            hit.hitPointWorld = btVector3(0, 0, 0);
        }

        m_handling->ProcessSuspensionData(i, hit);
    }


    btVector3 carCenter = tr.getOrigin();
    const float MAX_PHYSICS_FORCE = 40000.0f;

    SuspensionResults suspension = m_handling->CalculateSuspension();


    for (int i = 0; i < 4; i++) {
        if (suspension.wheels[i].active) {
            btVector3 f(suspension.wheels[i].force[0], suspension.wheels[i].force[1], suspension.wheels[i].force[2]);
            btVector3 p(suspension.wheels[i].offset[0], suspension.wheels[i].offset[1], suspension.wheels[i].offset[2]);
            if (f.length() > MAX_PHYSICS_FORCE) f = f.normalized() * MAX_PHYSICS_FORCE;
            m_carBody->applyForce(f, p);
        }
    }

    PhysicsAdvice combinedAdvice = m_handling->CombinedForces(
        inputs.steering,
        tr,
        speedKmh,
        m_carBody->getMass(),
        inputs.throttle,
        inputs.brake
    );


    for (int i = 0; i < 4; i++) {
        if (combinedAdvice.friction[i].active) {
            btVector3 f = combinedAdvice.friction[i].force;
            btVector3 p = combinedAdvice.friction[i].offset;

            if (p.length() > 500.0f)
                p = p + carCenter;

            if (f.length() > MAX_PHYSICS_FORCE)
                f = f.normalized() * MAX_PHYSICS_FORCE;

            m_carBody->applyForce(f, p);
        }
    }



    if (speedKmh < 1.0f && inputs.throttle < 0.1f && inputs.brake < 0.1f) {
        m_carBody->setLinearVelocity(btVector3(0, 0, 0));
    }
    else {
        m_carBody->setDamping(0.0f, 0.1f);
    }

    btVector3 velocity = m_carBody->getLinearVelocity();
    float speed = velocity.length();

    if (speed > 0.1f)
    {
        float dragMagnitude = speed * speed * m_vehicle.dragCoefficient;
        btVector3 dragForce = -velocity.normalized() * dragMagnitude;
        m_carBody->applyCentralForce(dragForce);


        g_DebugTelemetry.dragMagnitude = dragMagnitude;

        float frontDownforce = speed * speed * m_vehicle.frontDownforceCoeff;
        float rearDownforce = speed * speed * m_vehicle.rearDownforceCoeff;

        btVector3 downDir = tr.getBasis().getColumn(1) * -1.0f;


        btVector3 frontPos = tr.getBasis() * btVector3(0, 0, 2.35f);
        btVector3 rearPos = tr.getBasis() * btVector3(0, 0, -1.60f);


        m_carBody->applyForce(downDir* frontDownforce, frontPos);
        m_carBody->applyForce(downDir* rearDownforce, rearPos); 

        g_DebugTelemetry.frontDownforce = frontDownforce;
        g_DebugTelemetry.rearDownforce = rearDownforce;
        g_DebugTelemetry.downforceMagnitude = frontDownforce + rearDownforce;
    }


    if (m_dynamicsWorld) {
        m_dynamicsWorld->stepSimulation(deltaTime, 10, 1.0f / 300.0f);
    }
    static float triggerCooldown = 0.0f;
    if (triggerCooldown > 0.0f) {
        triggerCooldown -= deltaTime;
    }
    btVector3 carPos = tr.getOrigin();

    btVector3 metaPos(-153.0f, 15.0f, 170.0f); // Tsukuba
    //btVector3 metaPos(-573.0f, 150.0F, -2305.0f); // Nords
    //btVector3 metaPos(-314.0f, 24.0f, 730.0f); // SPA
    btVector3 s1Pos(120.0f, 60.0f, -200.0f);
    btVector3 s2Pos(800.0f, 40.0f, 500.0f);

    float triggerRadius = 15.0f;

    if (triggerCooldown <= 0.0f) {
        if (carPos.distance(metaPos) < triggerRadius) {
            m_passedStartMeta = true;
            triggerCooldown = 5.0f;
        }
        else if (carPos.distance(s1Pos) < triggerRadius) {
            m_passedSector1 = true;
            triggerCooldown = 5.0f;
        }
        else if (carPos.distance(s2Pos) < triggerRadius) {
            m_passedSector2 = true;
            triggerCooldown = 5.0f;
        }
    }
}

void PhysicsEngine::Cleanup() {
    for (auto body : m_trackBodies) {
        if (m_dynamicsWorld) m_dynamicsWorld->removeRigidBody(body);
        delete body;
    }
    m_trackBodies.clear();

    if (m_dynamicsWorld) {
        if (m_carBody) {
            m_dynamicsWorld->removeRigidBody(m_carBody);
            delete m_carBody;
            m_carBody = nullptr;
        }
        if (m_groundBody) {
            m_dynamicsWorld->removeRigidBody(m_groundBody);
            delete m_groundBody;
            m_groundBody = nullptr;
        }
    }

    for (auto ms : m_motionStates) {
        delete ms;
    }
    m_motionStates.clear();

    delete m_dynamicsWorld;     m_dynamicsWorld = nullptr;
    delete m_solver;            m_solver = nullptr;
    delete m_broadphase;        m_broadphase = nullptr;
    delete m_dispatcher;        m_dispatcher = nullptr;
    delete m_collisionConfig;   m_collisionConfig = nullptr;
    delete m_handling;          m_handling = nullptr;

    for (auto shape : m_collisionShapes) {
        if (shape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE) {
            auto meshShape = static_cast<btBvhTriangleMeshShape*>(shape);
            delete meshShape->getTriangleInfoMap();
        }
        delete shape;
    }
    m_collisionShapes.clear();
}