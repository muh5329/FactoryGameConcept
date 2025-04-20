#ifndef WORLD_H
#define WORLD_H

#include "btBulletDynamicsCommon.h"

class World { 

public:
    btDiscreteDynamicsWorld* dynamicsWorld;
    
public:
    World();
    ~World();

    void InitializePhysics();
    void ShutdownPhysics();
    void CreateGround();

private:
    btCollisionConfiguration* collisionConfiguration;
    btDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btConstraintSolver* solver;
};

#endif // WORLD_H
