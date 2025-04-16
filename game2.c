#include "raylib.h"
#include "btBulletDynamicsCommon.h"

#define GROUND_Y 0.0f
#define GRAVITY -9.8f
#define MOVE_SPEED 5.0f
#define JUMP_FORCE 5.0f

typedef struct Capsule {
    Vector3 position;
    float radius;
    float height;
    Vector3 velocity;
    bool grounded;
    btRigidBody* body;
} Capsule;

btDiscreteDynamicsWorld* dynamicsWorld;
btCollisionConfiguration* collisionConfiguration;
btDispatcher* dispatcher;
btBroadphaseInterface* overlappingPairCache;
btConstraintSolver* solver;
btDefaultCollisionConstructionInfo collisionInfo;

void InitializePhysics();
void ShutdownPhysics();
void UpdateCapsule(Capsule* capsule, float delta);
void CreateGround();
void CreateCapsule(Capsule* capsule);

int main(void) {
    InitWindow(800, 600, "Raylib with Bullet Physics");
    SetTargetFPS(60);
    
    Camera3D camera = {0};
    camera.position = (Vector3){ 0.0f, 2.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Capsule player = {
        .position = (Vector3){0, 2, 0},
        .radius = 0.5f,
        .height = 2.0f,
        .velocity = {0},
        .grounded = false,
        .body = NULL
    };

    InitializePhysics();
    CreateGround();
    CreateCapsule(&player);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Camera follows player
        camera.position = (Vector3){player.position.x, player.position.y + 2.0f, player.position.z + 6.0f};
        camera.target = (Vector3){player.position.x, player.position.y + 1.0f, player.position.z};

        UpdateCapsule(&player, dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // Draw ground
        DrawPlane((Vector3){0, GROUND_Y, 0}, (Vector2){20, 20}, LIGHTGRAY);

        // Draw capsule
        DrawCylinder((Vector3){player.position.x, player.position.y, player.position.z}, player.radius, player.radius, player.height - player.radius * 2, 16, BLUE);
        DrawSphere((Vector3){player.position.x, player.position.y + (player.height / 2.0f - player.radius), player.position.z}, player.radius, BLUE);
        DrawSphere((Vector3){player.position.x, player.position.y - (player.height / 2.0f - player.radius), player.position.z}, player.radius, BLUE);

        EndMode3D();

        DrawText("Use WASD to move, SPACE to jump", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    ShutdownPhysics();
    CloseWindow();
    return 0;
}

void InitializePhysics() {
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, GRAVITY, 0));
}

void ShutdownPhysics() {
    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;
}

void UpdateCapsule(Capsule* capsule, float delta) {
    Vector3 direction = {0};
87
    if (IsKeyDown(KEY_W)) direction.z -= 1.0f;
    if (IsKeyDown(KEY_S)) direction.z += 1.0f;
    if (IsKeyDown(KEY_A)) direction.x -= 1.0f;
    if (IsKeyDown(KEY_D)) direction.x += 1.0f;

    // Normalize direction
    float len = sqrtf(direction.x * direction.x + direction.z * direction.z);
    if (len > 0) {
        direction.x /= len;
        direction.z /= len;
        capsule->position.x += direction.x * MOVE_SPEED * delta;
        capsule->
::contentReference[oaicite:0]{index=0}
 
