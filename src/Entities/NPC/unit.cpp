#include "raylib.h"
#include <cmath>
#include <vector>
#include <Common/constants.hpp>
#include <btBulletDynamicsCommon.h>
#include <Scenes/World/world.h>
#include <Common/grid.h> 
#include <Common/pathfinding/Pathfinder.h>
#include <future>
// External constants and physics world assumed to be defined elsewhere
extern float MOVE_SPEED;
extern float JUMP_FORCE;
extern float GRAVITY;
extern float GROUND_Y;

class Unit {
public:
  
    Unit(Unit&&) = default;
    Unit& operator=(Unit&&) = default;

    World* world = nullptr;
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
    std::vector<Node> path;
    bool pathRequested = false;
    std::future<std::vector<Node>> futurePath;

    Unit(World* world_) : world(world_) {
        printf("[Unit] Constructor: world=%p\n", world);
        position = { static_cast<float>(GetRandomValue(0, 10)), 0, static_cast<float>(GetRandomValue(0, 10)) };
        position = { position.x, Constants::GROUND_Y + height / 2.0f + 0.1f, position.z };
        target = position;
        CreateUnit(world);
    }

    ~Unit() {
        printf("[Unit] Destructor: world=%p, body=%p\n", world, body);
        if (body) {
            if (world && world->dynamicsWorld) {
                printf("[Unit] Removing rigid body from dynamicsWorld=%p\n", world->dynamicsWorld);
                world->dynamicsWorld->removeRigidBody(body);
            } else {
                printf("[Unit] WARNING: world or dynamicsWorld is null!\n");
            }
            btMotionState* motionState = body->getMotionState();
            btCollisionShape* shape = body->getCollisionShape();
            if (motionState) delete motionState;
            if (shape) delete shape;
            delete body;
            body = nullptr;
        }
    }

    void Update(float deltaTime, Grid* navGrid) {
        CheckPathReady(navGrid);

        if (moving && body) {
            navGrid->cells[position.z][position.x].walkable = true;

            btVector3 currentVelocity = body->getLinearVelocity();
            btVector3 movement(0, currentVelocity.getY(), 0); // Preserve vertical velocity

            if (!path.empty()) {
                Node nextNode = path.front();
                Vector3 nextTarget = navGrid->GridToWorld(nextNode.x, nextNode.y);
                target = nextTarget; // Update the target for reference

                Vector3 direction = { nextTarget.x - position.x, 0, nextTarget.z - position.z };
                float length = std::sqrt(direction.x * direction.x + direction.z * direction.z);
                if (length > 0.1f) {
                    direction.x /= length;
                    direction.z /= length;
                    movement.setX(direction.x * Constants::UNIT_SPEED);
                    movement.setZ(direction.z * Constants::UNIT_SPEED);
                } 
                if (length < 0.3f && currentVelocity.length() < 0.2f) {
                    // Node reached: remove it from the path
                    path.erase(path.begin());
                    // If no more nodes left, stop moving.
                    if (path.empty()) {
                        moving = false;
                    }
                }

                // Apply movement force
                btVector3 desiredVel(direction.x * Constants::UNIT_SPEED, currentVelocity.getY(), direction.z * Constants::UNIT_SPEED);
                btVector3 currentVel = body->getLinearVelocity();
                btVector3 steering = desiredVel - currentVel;
                float maxForce = 20.0f;
                if (steering.length() < maxForce) {
                    steering = steering.normalized() * maxForce;
                }
                body->applyCentralForce(steering * body->getMass());
            } else {
                // If there are no nodes, ensure we are not marked as moving and update grid status
                moving = false;
                navGrid->cells[position.z][position.x].walkable = false;
            }

            // Check for ground contact
            btTransform trans;
            body->getMotionState()->getWorldTransform(trans);
            float y = trans.getOrigin().getY();
            grounded = (y <= Constants::GROUND_Y + height / 2.0f + 0.01f);

            // Sync Bullet position back to Raylib's Vector3
            btVector3 btPos = trans.getOrigin();
            position = { btPos.getX(), btPos.getY(), btPos.getZ() };
        } else {
            moving = false;
            navGrid->cells[position.z][position.x].walkable = false;
        }
    }

    void RequestPath(Grid* navGrid) {
        
        Vector2 startGrid = navGrid->WorldToGrid(position);
        Vector2 goalGrid = navGrid->WorldToGrid(target);

        Node startNode(static_cast<int>(startGrid.x), static_cast<int>(startGrid.y));
        Node goalNode(static_cast<int>(goalGrid.x), static_cast<int>(goalGrid.y));

        auto intGrid = navGrid->ConvertGridToIntMap(*navGrid);
        futurePath = std::async(std::launch::async, FindPath, intGrid, startNode, goalNode);
        pathRequested = true;
        
    }

    void CheckPathReady(Grid* navGrid) {
        if (pathRequested && futurePath.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready) {
            auto foundPath = futurePath.get();
            path = SmoothPath(foundPath, navGrid);
            pathRequested = false;
            moving = true;
        }
    }

   
    // Checks if a grid cell is walkable (no obstacle) using navGrid
    bool IsWalkable(int x, int y, Grid* navGrid) {
        // Bounds check
        if (!navGrid) return false;
        if (x < 0 || y < 0 || x >= navGrid->width || y >= navGrid->height) return false;
        return navGrid->cells[y][x].walkable;
    }

    // Overload for HasLineOfSight to use navGrid
    bool HasLineOfSight(const Node& a, const Node& b, Grid* navGrid) {
        int x0 = a.x, y0 = a.y, x1 = b.x, y1 = b.y;
        int dx = abs(x1 - x0), dy = abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx - dy;
        int x = x0, y = y0;
        while (true) {
            if (!IsWalkable(x, y, navGrid)) return false;
            if (x == x1 && y == y1) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x += sx; }
            if (e2 < dx) { err += dx; y += sy; }
        }
        return true;
    }

    // Update SmoothPath to use navGrid
    std::vector<Node> SmoothPath(const std::vector<Node>& inputPath, Grid* navGrid) {
        if (inputPath.size() <= 2) return inputPath;
        std::vector<Node> smoothed;
        size_t i = 0;
        while (i < inputPath.size()) {
            smoothed.push_back(inputPath[i]);
            size_t furthest = i + 1;
            for (size_t j = inputPath.size() - 1; j > i; --j) {
                if (HasLineOfSight(inputPath[i], inputPath[j], navGrid)) {
                    furthest = j;
                    break;
                }
            }
            i = furthest;
        }
        if (smoothed.back().x != inputPath.back().x || smoothed.back().y != inputPath.back().y)
            smoothed.push_back(inputPath.back());
        return smoothed;
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
