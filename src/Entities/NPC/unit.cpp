#include "raylib.h"
#include <cmath>
#include <vector>
#include <Common/constants.hpp>
#include <raylib.h>
#include <btBulletDynamicsCommon.h>

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
        Vector3 velocity;
        bool grounded = false;
        bool selected = false;
        bool moving = false;
        bool attacking = false;
        int targetIndex = -1;
    
        Unit(World *world) {
            position = { static_cast<float>(GetRandomValue(0, 10)), 0, static_cast<float>(GetRandomValue(0, 10)) };
            target = position;
            CreateUnit(world)
        }
    
        void Update(float deltaTime) {
            if (moving) {
                Vector3 direction = { target.x - position.x, 0, target.z - position.z };
                float distance = std::sqrt(direction.x * direction.x + direction.z * direction.z);
                if (distance > 0.1f) {
                    direction.x /= distance;
                    direction.z /= distance;
                    position.x += direction.x * Constants::UNIT_SPEED * deltaTime;
                    position.z += direction.z * Constants::UNIT_SPEED * deltaTime;
                    
                } else {
                    moving = false;
                }
            }
        }
    
        void Draw() const {
            Color color = selected ? RED : BLUE;
            DrawCube(position, 0.5f, 0.5f, 0.5f, color);
        }

        void CreateUnit( World* world){
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