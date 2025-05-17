#include "raylib.h"
#include <cmath>
#include <vector>
#include <Common/constants.hpp>
#include <btBulletDynamicsCommon.h>
#include <Scenes/World/world.h>

// External constants and physics world assumed to be defined elsewhere
extern float MOVE_SPEED;
extern float JUMP_FORCE;
extern float GRAVITY;
extern float GROUND_Y;
extern btDiscreteDynamicsWorld* dynamicsWorld;

class Unit {
public:
    bool active = true;
    Vector3 position{};
    Vector3 target{};
    float radius = 1.0f;
    float height = 2.0f;
    btRigidBody* body = nullptr;
    Vector3 velocity{};
    bool grounded = false;
    bool selected = false;
    bool moving = false;
    bool attacking = false;
    int targetIndex = -1;

    Unit(World* world) {
        position = { static_cast<float>(GetRandomValue(0, 10)), 0, static_cast<float>(GetRandomValue(0, 10)) };
        target = position;
        CreateUnit(world);
    }

    void Update(float deltaTime) {
        if (moving && body) {
            btVector3 velocity = body->getLinearVelocity();
            btVector3 movement(0, velocity.getY(), 0); // Keep vertical component as-is
            Vector3 input = { 0 };
            float length = sqrtf(input.x * input.x + input.z * input.z);
            if (length > 0.0f) {
                input.x /= length;
                input.z /= length;

                movement.setX(input.x * Constants::MOVE_SPEED);
                movement.setZ(input.z * Constants::MOVE_SPEED);
            }

            // Apply movement
            body->setLinearVelocity(movement);

             // Check for ground contact
            btTransform trans;
            body->getMotionState()->getWorldTransform(trans);
            float y = trans.getOrigin().getY();

            if (y <= Constants::GROUND_Y +  height / 2.0f + 0.01f) {
                grounded = true;
            } else {
               grounded = false;
            }
        } else {
            moving = false;
        }
    }

    void Draw() const {
        Color color = selected ? RED : BLUE;
        DrawCube(position, 0.5f, 0.5f, 0.5f, color);
    }

    void CreateUnit(World* world) {
        btCollisionShape* capsuleShape = new btCapsuleShape(radius, height - 2 * radius);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(position.x, position.y, position.z));

        btScalar mass = 1.0f;
        btVector3 localInertia(0, 0, 0);
        capsuleShape->calculateLocalInertia(mass, localInertia);

        btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, capsuleShape, localInertia);
        body = new btRigidBody(rbInfo);
        body->setActivationState(DISABLE_DEACTIVATION);
        body->setFriction(0.9f);
        body->setSleepingThresholds(0, 0);
        body->setAngularFactor(btVector3(0, 0, 0)); // Prevent rotation

        world->dynamicsWorld->addRigidBody(body);
    }
};
