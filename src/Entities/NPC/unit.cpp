#include "raylib.h"
#include <cmath>
#include <vector>
#include <Common/constants.hpp>
#include <btBulletDynamicsCommon.h>
#include <Scenes/World/world.h>
#include <Common/grid.h> 
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
        position = { position.x, Constants::GROUND_Y + height / 2.0f + 0.1f, position.z }; // Displace unit slightly above ground to prevent floating point issues
        target = position;
        CreateUnit(world);
    }

    void Update(float deltaTime, Grid* navGrid) {
        if (moving && body) {
            navGrid->cells[position.z][position.x].walkable = true;
            
            btVector3 velocity = body->getLinearVelocity();
            btVector3 movement(0, velocity.getY(), 0); // Keep vertical component as-is 

            Vector3 direction = { target.x - position.x, 0, target.z - position.z };
            float length = std::sqrt(direction.x * direction.x + direction.z * direction.z);
            if (length > 0.1f) {

                direction.x /= length;
                direction.z /= length;
               
                movement.setX(direction.x * Constants::UNIT_SPEED );
                movement.setZ(direction.z * Constants::UNIT_SPEED );
            } if (length < 0.3f && body->getLinearVelocity().length() < 0.2f) {
                moving = false;
            }

            // Apply movement
            btVector3 desiredVel(direction.x * Constants::UNIT_SPEED, velocity.getY(), direction.z * Constants::UNIT_SPEED);
            btVector3 currentVel = body->getLinearVelocity();
            btVector3 steering = desiredVel - currentVel;

            // Allow stronger steering force, but avoid crazy impulses
            float maxForce = 20.0f; // Try 20-50, tweak based on mass and friction
            TraceLog(LOG_INFO, "maxForce %.2f, ",  steering.length());
            if (steering.length() < maxForce) {
                steering = steering.normalized() * maxForce;
            }

            body->applyCentralForce(steering * body->getMass());

           

             // Check for ground contact
            btTransform trans;
            body->getMotionState()->getWorldTransform(trans);
            
            float y = trans.getOrigin().getY();

            if (y <= Constants::GROUND_Y +  height / 2.0f + 0.01f) {
                grounded = true;
            } else {
               grounded = false;
            }
            // TraceLog(LOG_INFO, "Set position: %.2f, %.2f %.2f",  btPos.getX(), btPos.getY(), btPos.getZ() );
            // TraceLog(LOG_INFO, "Set velocity: %.2f, %.2f", movement.getX(), movement.getZ());
            // Sync Bullet position back to Raylib's Vector3
             btVector3 btPos = trans.getOrigin();
             position = { btPos.getX(), btPos.getY(), btPos.getZ() };

            
        
        } else {
            moving = false;
            navGrid->cells[position.z][position.x].walkable = false;
        }
    }

    void Draw() const {
        Color color = selected ? RED : BLUE;
        DrawCube(position, radius , height, radius , color);
    }

    void CreateUnit(World* world) {
        btCollisionShape* capsuleShape = new btCapsuleShape(radius, height - 2 * radius);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(position.x, position.y, position.z));
        
        btScalar mass = 1.0f;
        btVector3 localInertia(0, 0, 0);
        capsuleShape->calculateLocalInertia(mass, localInertia);
        capsuleShape->setMargin(0.05f);  // Helps separation
        btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, capsuleShape, localInertia);
        body = new btRigidBody(rbInfo);
        body->setActivationState(DISABLE_DEACTIVATION);
        body->setFriction(0.4f);  // Lower value for smoother movement
        body->setRollingFriction(0.0f);
        body->setSleepingThresholds(0, 0);
        body->setAngularFactor(btVector3(0, 0, 0)); // Prevent rotation
        body->setRestitution(0.0f);              // No bouncing
        body->setContactProcessingThreshold(BT_LARGE_FLOAT); // Always process contact
        body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE); // Ensure collisions are resolved
        body->setCcdMotionThreshold(0.001f);
        body->setCcdSweptSphereRadius(0.9f);  // slightly less than radius

        world->dynamicsWorld->addRigidBody(body,  Constants::COL_UNIT,  Constants::COL_ALL);
    }
};
