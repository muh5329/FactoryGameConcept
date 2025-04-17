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
            position = {0, 2, 0};
            radius = 0.5f;
            height = 2.0f;
            velocity = {0};
            grounded = false;
            
        }
    
        void Update(float deltaTime) {
            
        }
    
        void Draw() const {
            
        }
    };

class Game {
private:
    std::vector<Unit> units;
    Camera3D camera{};
    Vector2 dragStart{}, dragEnd{};
    bool isDragging = false;

public:
    Game() {
        InitWindow(800, 600, "3D Isometric RTS");
        SetTargetFPS(60);

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

    void Draw() {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawGrid(20, 1.0f);
        for (const auto& unit : units) unit.Draw();
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
};

int main() {
    Game game;
    game.Run();
    return 0;
}
