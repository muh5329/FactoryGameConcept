#ifndef CAPSULE_H
#define CAPSULE_H

#include <raylib.h>
#include <btBulletDynamicsCommon.h>
#include <Scenes/World/world.h>

// External constants and physics world assumed to be defined elsewhere
extern float MOVE_SPEED;
extern float JUMP_FORCE;
extern float GRAVITY;
extern float GROUND_Y;
extern btDiscreteDynamicsWorld* dynamicsWorld;

struct CapsuleObj {
    Vector3 position;
    float radius;
    float height;
    Vector3 velocity;
    bool grounded;
    btRigidBody* body = nullptr;
};

class Capsule {
public:
    Capsule();  // Constructor

    void Update(float deltaTime);  // Update logic for capsule movement
    void Draw() const;            // Render capsule
    void UpdateCapsule(CapsuleObj* capsule, float delta);  // Update capsule physics
    void CreateCapsule(CapsuleObj* capsule, World* world);  // Create a rigid body for the capsule

private:
    Vector3 position;
    float radius;
    float height;
    Vector3 velocity;
    bool grounded;
    btRigidBody* body;
};

#endif // CAPSULE_H
