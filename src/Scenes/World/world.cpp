#include <Scenes/World/world.h>
#include <Common/constants.hpp>

World::World()
    : dynamicsWorld(nullptr),
      collisionConfiguration(nullptr),
      dispatcher(nullptr),
      overlappingPairCache(nullptr),
      solver(nullptr) {}

World::~World() {
    ShutdownPhysics();
}

void World::InitializePhysics() {
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, Constants::GRAVITY, 0));
   
    
}

void World::ShutdownPhysics() {
    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;

    dynamicsWorld = nullptr;
    solver = nullptr;
    overlappingPairCache = nullptr;
    dispatcher = nullptr;
    collisionConfiguration = nullptr;
}

void World::CreateGround() {
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, Constants::GROUND_Y, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundRigidBody);
}
