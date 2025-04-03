#include "raylib.h"
#include "raymath.h"
#include "bullet/btBulletDynamicsCommon.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_UNITS 100

// Structure for units with physics and pathfinding
typedef struct Unit {
    Vector3 position;
    Vector3 target;
    Model model;
    btRigidBody *rigidBody; // Bullet Physics
    bool selected;
} Unit;

Unit units[MAX_UNITS];
int numUnits = 0;

// Bullet Physics world
btDiscreteDynamicsWorld *dynamicsWorld;

// Function prototypes
void InitPhysics();
void InitUnits();
void UpdateUnits(float deltaTime);
void DrawUnits();
void ComputePathForUnit(Unit *unit, Vector3 destination);

void InitPhysics() {
    btDefaultCollisionConfiguration *collisionConfig = new btDefaultCollisionConfiguration();
    btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collisionConfig);
    btBroadphaseInterface *broadphase = new btDbvtBroadphase();
    btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
    dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

void InitUnits() {
    Model unitModel = LoadModel("resources/medieval_unit.obj");

    for (int i = 0; i < 10; i++) {
        units[i].position = (Vector3){ GetRandomValue(-10, 10), 0, GetRandomValue(-10, 10) };
        units[i].target = units[i].position;
        units[i].model = unitModel;
        units[i].selected = false;

        btCollisionShape *shape = new btSphereShape(0.5f);
        btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(units[i].position.x, units[i].position.y, units[i].position.z)));
        btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, motionState, shape);
        units[i].rigidBody = new btRigidBody(rbInfo);
        dynamicsWorld->addRigidBody(units[i].rigidBody);
    }
    numUnits = 10;
}

void UpdateUnits(float deltaTime) {
    dynamicsWorld->stepSimulation(deltaTime);
    for (int i = 0; i < numUnits; i++) {
        btTransform transform;
        units[i].rigidBody->getMotionState()->getWorldTransform(transform);
        units[i].position.x = transform.getOrigin().getX();
        units[i].position.y = transform.getOrigin().getY();
        units[i].position.z = transform.getOrigin().getZ();
    }
}

void ComputePathForUnit(Unit *unit, Vector3 destination) {
    // Placeholder for A* pathfinding (implement grid-based navigation)
    unit->target = destination;
}

void DrawUnits() {
    for (int i = 0; i < numUnits; i++) {
        DrawModel(units[i].model, units[i].position, 1.0f, WHITE);
    }
}

int main(void) {
    InitWindow(1280, 720, "Medieval RTS with Bullet Physics & A* Pathfinding");
    InitPhysics();
    InitUnits();
    Camera3D camera = { {20, 20, 20}, {0, 0, 0}, {0, 1, 0}, 45.0f, CAMERA_ORTHOGRAPHIC };
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        UpdateUnits(deltaTime);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        DrawUnits();
        EndMode3D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
