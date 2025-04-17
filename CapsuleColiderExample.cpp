// #include "raylib.h"
// #include <raymath.h>
// #include <cmath>
// #include <iostream>
// #include "btBulletDynamicsCommon.h"

// #define GROUND_Y 0.0f
// #define GRAVITY -9.8f
// #define MOVE_SPEED 5.0f
// #define JUMP_FORCE 5.0f

// struct Capsule {
//     Vector3 position;
//     float radius;
//     float height;
//     Vector3 velocity;
//     bool grounded;
//     btRigidBody* body = nullptr;
// };

// btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
// btCollisionConfiguration* collisionConfiguration = nullptr;
// btDispatcher* dispatcher = nullptr;
// btBroadphaseInterface* overlappingPairCache = nullptr;
// btConstraintSolver* solver = nullptr;

// void InitializePhysics();
// void ShutdownPhysics();
// void UpdateCapsule(Capsule* capsule, float delta);
// void CreateGround();
// void CreateCapsule(Capsule* capsule);

// int main() {
//     InitWindow(800, 600, "Raylib with Bullet Physics");
//     SetTargetFPS(60);

//     Camera3D camera = {0};
//     camera.position = { 0.0f, 2.0f, 10.0f };
//     camera.target = { 0.0f, 1.0f, 0.0f };
//     camera.up = { 0.0f, 1.0f, 0.0f };
//     camera.fovy = 45.0f;
//     camera.projection = CAMERA_PERSPECTIVE;

//     Capsule player = {
//         .position = {0, 2, 0},
//         .radius = 0.5f,
//         .height = 2.0f,
//         .velocity = {0},
//         .grounded = false
//     };

//     InitializePhysics();
//     CreateGround();
//     CreateCapsule(&player);

//     while (!WindowShouldClose()) {
//         float dt = GetFrameTime();

//         // Camera follows player
//         camera.position = {player.position.x, player.position.y + 2.0f, player.position.z + 6.0f};
//         camera.target = {player.position.x, player.position.y + 1.0f, player.position.z};

//         UpdateCapsule(&player, dt);

//         BeginDrawing();
//         ClearBackground(RAYWHITE);

//         BeginMode3D(camera);

//         DrawPlane({0, GROUND_Y, 0}, {20, 20}, LIGHTGRAY);

//         // Draw capsule
//         DrawCylinder({player.position.x, player.position.y, player.position.z}, player.radius, player.radius, player.height - player.radius * 2, 16, BLUE);
//         DrawSphere({player.position.x, player.position.y + (player.height / 2.0f - player.radius), player.position.z}, player.radius, BLUE);
//         DrawSphere({player.position.x, player.position.y - (player.height / 2.0f - player.radius), player.position.z}, player.radius, BLUE);

//         EndMode3D();

//         DrawText("Use WASD to move, SPACE to jump", 10, 10, 20, DARKGRAY);

//         EndDrawing();
//     }

//     ShutdownPhysics();
//     CloseWindow();
//     return 0;
// }

// void InitializePhysics() {
//     collisionConfiguration = new btDefaultCollisionConfiguration();
//     dispatcher = new btCollisionDispatcher(collisionConfiguration);
//     overlappingPairCache = new btDbvtBroadphase();
//     solver = new btSequentialImpulseConstraintSolver();
//     dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
//     dynamicsWorld->setGravity(btVector3(0, GRAVITY, 0));
// }

// void ShutdownPhysics() {
//     delete dynamicsWorld;
//     delete solver;
//     delete overlappingPairCache;
//     delete dispatcher;
//     delete collisionConfiguration;
// }

// void UpdateCapsule(Capsule* capsule, float delta) {
//     Vector3 direction = {0};

//     if (IsKeyDown(KEY_W)) direction.z -= 1.0f;
//     if (IsKeyDown(KEY_S)) direction.z += 1.0f;
//     if (IsKeyDown(KEY_A)) direction.x -= 1.0f;
//     if (IsKeyDown(KEY_D)) direction.x += 1.0f;

//     float len = sqrtf(direction.x * direction.x + direction.z * direction.z);
//     if (len > 0.0f) {
//         direction.x /= len;
//         direction.z /= len;
//         capsule->position.x += direction.x * MOVE_SPEED * delta;
//         capsule->position.z += direction.z * MOVE_SPEED * delta;
//     }

//     if (IsKeyPressed(KEY_SPACE) && capsule->grounded) {
//         capsule->velocity.y = JUMP_FORCE;
//         capsule->grounded = false;
//     }

//     capsule->velocity.y += GRAVITY * delta;
//     capsule->position.y += capsule->velocity.y * delta;

//     if (capsule->position.y <= GROUND_Y + capsule->height / 2.0f) {
//         capsule->position.y = GROUND_Y + capsule->height / 2.0f;
//         capsule->velocity.y = 0.0f;
//         capsule->grounded = true;
//     }
// }

// void CreateGround() {
//     btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
//     btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, GROUND_Y, 0)));
//     btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
//     btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
//     dynamicsWorld->addRigidBody(groundRigidBody);
// }

// void CreateCapsule(Capsule* capsule) {
//     btCollisionShape* capsuleShape = new btCapsuleShape(capsule->radius, capsule->height - capsule->radius * 2);
//     btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(capsule->position.x, capsule->position.y, capsule->position.z)));
//     btScalar mass = 1.0f;
//     btVector3 inertia(0, 0, 0);
//     capsuleShape->calculateLocalInertia(mass, inertia);
//     btRigidBody::btRigidBodyConstructionInfo capsuleCI(mass, motionState, capsuleShape, inertia);
//     capsule->body = new btRigidBody(capsuleCI);
//     capsule->body->setActivationState(DISABLE_DEACTIVATION);
//     dynamicsWorld->addRigidBody(capsule->body);
// }
