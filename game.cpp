#include "raylib.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include "btBulletDynamicsCommon.h"


#define TILE_SIZE 32
#define ATTACK_RANGE 30.0f
#define UNIT_SPEED 2.0f
#define GRID_SIZE 25
#define MAX_UNITS 10

#define GROUND_Y 0.0f
#define GRAVITY -9.8f
#define MOVE_SPEED 5.0f
#define JUMP_FORCE 5.0f

btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
btCollisionConfiguration* collisionConfiguration = nullptr;
btDispatcher* dispatcher = nullptr;
btBroadphaseInterface* overlappingPairCache = nullptr;
btConstraintSolver* solver = nullptr;

// class World {
//     public:
//         btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
//         btCollisionConfiguration* collisionConfiguration = nullptr;
//         btDispatcher* dispatcher = nullptr;
//         btBroadphaseInterface* overlappingPairCache = nullptr;
//         btConstraintSolver* solver = nullptr;
    
//         World() {    
//         }
    
// };

struct CapsuleObj {
    Vector3 position;
    float radius;
    float height;
    Vector3 velocity;
    bool grounded;
    btRigidBody* body = nullptr;
};

class Unit {
public:
    bool active = true;
    Vector3 position{};
    Vector3 target{};
    bool selected = false;
    bool moving = false;
    bool attacking = false;
    int targetIndex = -1;

    Unit() {
        position = { static_cast<float>(GetRandomValue(0, 10)), 0, static_cast<float>(GetRandomValue(0, 10)) };
        target = position;
    }

    void Update(float deltaTime) {
        if (moving) {
            Vector3 direction = { target.x - position.x, 0, target.z - position.z };
            float distance = std::sqrt(direction.x * direction.x + direction.z * direction.z);
            if (distance > 0.1f) {
                direction.x /= distance;
                direction.z /= distance;
                position.x += direction.x * UNIT_SPEED * deltaTime;
                position.z += direction.z * UNIT_SPEED * deltaTime;
            } else {
                moving = false;
            }
        }
    }

    void Draw() const {
        Color color = selected ? RED : BLUE;
        DrawCube(position, 0.5f, 0.5f, 0.5f, color);
    }

};

class Capsule {
    public:
        Vector3 position;
        float radius;
        float height;
        Vector3 velocity;
        bool grounded;
        btRigidBody* body = nullptr;
    
    
        Capsule() {
        
        }
    
        void Update(float deltaTime) {
            
        }
    
        void Draw() const {
            
        }

        void UpdateCapsule(CapsuleObj* capsule, float delta) {
            Vector3 direction = {0};
    
            if (IsKeyDown(KEY_W)) direction.z -= 1.0f;
            if (IsKeyDown(KEY_S)) direction.z += 1.0f;
            if (IsKeyDown(KEY_A)) direction.x -= 1.0f;
            if (IsKeyDown(KEY_D)) direction.x += 1.0f;
    
            float len = sqrtf(direction.x * direction.x + direction.z * direction.z);
            if (len > 0.0f) {
                direction.x /= len;
                direction.z /= len;
                capsule->position.x += direction.x * MOVE_SPEED * delta;
                capsule->position.z += direction.z * MOVE_SPEED * delta;
            }
    
            if (IsKeyPressed(KEY_SPACE) && capsule->grounded) {
                capsule->velocity.y = JUMP_FORCE;
                capsule->grounded = false;
            }
    
            capsule->velocity.y += GRAVITY * delta;
            capsule->position.y += capsule->velocity.y * delta;
    
            if (capsule->position.y <= GROUND_Y + capsule->height / 2.0f) {
                capsule->position.y = GROUND_Y + capsule->height / 2.0f;
                capsule->velocity.y = 0.0f;
                capsule->grounded = true;
            }
        }

        void CreateCapsule(CapsuleObj* capsule) {
            btCollisionShape* capsuleShape = new btCapsuleShape(capsule->radius, capsule->height - capsule->radius * 2);
            btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(capsule->position.x, capsule->position.y, capsule->position.z)));
            btScalar mass = 1.0f;
            btVector3 inertia(0, 0, 0);
            capsuleShape->calculateLocalInertia(mass, inertia);
            btRigidBody::btRigidBodyConstructionInfo capsuleCI(mass, motionState, capsuleShape, inertia);
            capsule->body = new btRigidBody(capsuleCI);
            capsule->body->setActivationState(DISABLE_DEACTIVATION);
            dynamicsWorld->addRigidBody(capsule->body);
        }
    };

class Game {
private:
    std::vector<Unit> units;
    Camera3D camera{};
    Vector2 dragStart{}, dragEnd{};
    bool isDragging = false;
    Capsule capsule;
    CapsuleObj player;

public:
    Game() {
        InitWindow(800, 600, "3D Isometric RTS");
        SetTargetFPS(60);

        // Capsule Setup
        player = {
            .position = {0, 2, 0},
            .radius = 0.5f,
            .height = 2.0f,
            .velocity = {0},
            .grounded = false
        };
       

        // Init Bullet
        InitializePhysics();
        CreateGround();
        capsule.CreateCapsule(&player);

        
        camera.position = { 10.0f, 10.0f, 10.0f };
        camera.target = { 0.0f, 0.0f, 0.0f };
        camera.up = { 0.0f, 1.0f, 0.0f };
        camera.fovy = 45.0f;
        camera.projection = CAMERA_PERSPECTIVE;

        for (int i = 0; i < MAX_UNITS; i++) {
            units.emplace_back();
        }
    }

    ~Game() {
        ShutdownPhysics();
        CloseWindow();
    }

    void Run() {
        while (!WindowShouldClose()) {
            float deltaTime = GetFrameTime();
            HandleInput();
            Update(deltaTime);
            Draw();
        }
    }

private:
    void Update(float deltaTime) {
        for (auto& unit : units) {
            unit.Update(deltaTime);
        }

        UpdateCamera();
        //UpdateCapsuleCamera();
        capsule.UpdateCapsule(&player, deltaTime);
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

    void CreateGround() {
        btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
        btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, GROUND_Y, 0)));
        btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
        btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
        dynamicsWorld->addRigidBody(groundRigidBody);
    }
        

    void HandleInput() {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            dragStart = GetMousePosition();
            isDragging = true;
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            dragEnd = GetMousePosition();
            isDragging = false;
            Rectangle selection = {
                std::min(dragStart.x, dragEnd.x),
                std::min(dragStart.y, dragEnd.y),
                std::fabs(dragEnd.x - dragStart.x),
                std::fabs(dragEnd.y - dragStart.y)
            };
            SelectUnitsInRectangle(selection);
        }

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            Ray ray = GetMouseRay(GetMousePosition(), camera);
            Vector3 groundPoint = GetRayGroundIntersection(ray);
            for (auto& unit : units) {
                if (unit.selected) {
                    unit.target = groundPoint;
                    unit.moving = true;
                }
            }
        }
    }

    void SelectUnitsInRectangle(Rectangle selection) {
        for (auto& unit : units) {
            unit.selected = false;
            Vector2 screenPos = GetWorldToScreen(unit.position, camera);
            if (CheckCollisionPointRec(screenPos, selection)) {
                unit.selected = true;
            }
        }
    }

    Vector3 GetRayGroundIntersection(const Ray& ray) {
        if (ray.direction.y == 0) return ray.position;
        float t = -ray.position.y / ray.direction.y;
        return {
            ray.position.x + t * ray.direction.x,
            0.0f,
            ray.position.z + t * ray.direction.z
        };
    }

    void UpdateCamera() {
        if (IsKeyDown(KEY_W)) camera.position.z -= 0.1f;
        if (IsKeyDown(KEY_S)) camera.position.z += 0.1f;
        if (IsKeyDown(KEY_A)) camera.position.x -= 0.1f;
        if (IsKeyDown(KEY_D)) camera.position.x += 0.1f;

        camera.target = { 0.0f, 0.0f, 0.0f }; // Lock target to center


      
    }

    void UpdateCapsuleCamera() {
          // Camera follows player
        camera.position = {player.position.x, player.position.y + 2.0f, player.position.z + 6.0f};
        camera.target = {player.position.x, player.position.y + 1.0f, player.position.z};
    }

    void Draw() {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawGrid(20, 1.0f);
        for (const auto& unit : units) unit.Draw();
        DrawPlayer();
        EndMode3D();
        DrawText("3D Isometric RTS", 10, 10, 20, BLACK);

        if (isDragging) {
            Vector2 mousePos = GetMousePosition();
            Rectangle rect = {
                std::min(dragStart.x, mousePos.x),
                std::min(dragStart.y, mousePos.y),
                std::fabs(mousePos.x - dragStart.x),
                std::fabs(mousePos.y - dragStart.y)
            };
            DrawRectangleLinesEx(rect, 1, GREEN);
        }

        EndDrawing();
    }

    void DrawPlayer() {
         // Draw capsule
         DrawCylinder({player.position.x, player.position.y, player.position.z}, player.radius, player.radius, player.height - player.radius * 2, 16, BLUE);
         DrawSphere({player.position.x, player.position.y + (player.height / 2.0f - player.radius), player.position.z}, player.radius, BLUE);
         DrawSphere({player.position.x, player.position.y - (player.height / 2.0f - player.radius), player.position.z}, player.radius, BLUE);
    }
};

int main() {
    Game game;
    game.Run();
    return 0;
}
