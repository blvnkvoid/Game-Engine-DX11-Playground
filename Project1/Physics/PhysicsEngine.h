#pragma once
#include "Handling.h"
#include "../Cars/Audi_R8_LMS/AudiR8.h"
#include "../Cars/Porsche_911_Carrera_S/Porsche911.h"
#include "../Cars/Honda_Civic_EG6/Civic.h"
#include <bullet/BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>

#pragma warning(push)
#pragma warning(disable : 26451) // Arithmetic overflow (the one you just found)
#pragma warning(disable : 26495) // Uninitialized member variable
#pragma warning(disable : 26812) // Prefer 'enum class' over 'enum'
#pragma warning(disable : 4099)  // Type name first seen using 'class' now seen using 'struct'
#pragma warning(disable : 4244)  // Conversion from 'double' to 'btScalar', possible loss of data
#pragma warning(disable : 26439) // This kind of function may not throw. Declare it 'noexcept'

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;

#pragma warning(pop)


class PhysicsEngine {
public:
    PhysicsEngine();
    ~PhysicsEngine() { Cleanup(); }
    void Initialize();     
    void CreatePhysicsWorld();
    void Cleanup();
    void ResetCar();
    void GetCarTransform(btTransform& outTrans);
    void AddTriangleMeshCollider(const std::vector<SharedVertex>& vertices, const std::vector<UINT>& indices);
    void Update(float deltaTime, SharedVehicleInputs inputs);
    void RequestReset() { m_needsReset = true; }
    bool m_needsReset = false;
    btRigidBody* GetCarBody() { return m_carBody; }
    btDiscreteDynamicsWorld* GetDynamicsWorld() { return m_dynamicsWorld; }
    DirectX::XMMATRIX btTransformToXMMATRIX(const btTransform& worldTransform);
    static bool CustomInternalEdgeCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
    {
        btAdjustInternalEdgeContacts(cp, colObj1Wrap, colObj0Wrap, partId1, index1);
        cp.m_combinedFriction = 0.01f;
        cp.m_combinedRestitution = 0.00f; 

        if (cp.m_distance1 < -0.01f)
        {
            cp.m_distance1 = -0.01f;
        }
        return true;
    }

    bool CheckAndResetPassedStartMeta() { bool val = m_passedStartMeta; m_passedStartMeta = false; return val; }
    bool CheckAndResetPassedSector1() { bool val = m_passedSector1;   m_passedSector1 = false;   return val; }
    bool CheckAndResetPassedSector2() { bool val = m_passedSector2;   m_passedSector2 = false;   return val; }

    void SetVehicleDefinition(const VehicleDefinition& definition);
    void SetHandling(Handling* handling);
    private:
    btDefaultCollisionConfiguration* m_collisionConfig;
    btCollisionDispatcher* m_dispatcher;
    btBroadphaseInterface* m_broadphase;
    btSequentialImpulseConstraintSolver* m_solver;
    btDiscreteDynamicsWorld* m_dynamicsWorld;
    btRigidBody* m_carBody;
    btRigidBody* m_groundBody;
    Handling* m_handling;
    std::vector<btCollisionShape*> m_collisionShapes;
    std::vector<btDefaultMotionState*> m_motionStates;    
    std::vector<btRigidBody*> m_trackBodies;
    std::vector<btStridingMeshInterface*> m_meshInterfaces;
    bool m_passedStartMeta = false;
    bool m_passedSector1 = false;
    bool m_passedSector2 = false;
    VehicleDefinition m_vehicle;

};