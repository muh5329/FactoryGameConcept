#include "raylib.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include "btBulletDynamicsCommon.h"
#include <Controllers/capsule.h>
#include <Scenes/World/world.h>
#include <Common/constants.hpp>
#include <Common/camera.cpp>

#include <Entities/NPC/unit.cpp>


class Game {
private:
    std::vector<Unit> units;
    Vector2 dragStart{}, dragEnd{};
    bool isDragging = false;
    RtsCamera camera;
    CapsuleObj player;
    Capsule playerCapsule;
    World world;
    int scrollSpeed = 4; 
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
        world.InitializePhysics();
        world.CreateGround();
        playerCapsule.CreateCapsule(&player, &world);

       
        for (int i = 0; i < Constants::MAX_UNITS; i++) {
            units.emplace_back();
        }
    }

    ~Game() {
        world.ShutdownPhysics();
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
        camera.Update();
        playerCapsule.UpdateCapsule(&player, deltaTime);
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
            DrawRectangleDrag();
        }

        EndDrawing();
    }

    void DrawRectangleDrag(){
        Vector2 mousePos = GetMousePosition();
        Rectangle rect = {
            std::min(dragStart.x, mousePos.x),
            std::min(dragStart.y, mousePos.y),
            std::fabs(mousePos.x - dragStart.x),
            std::fabs(mousePos.y - dragStart.y)
        };
        DrawRectangleLinesEx(rect, 1, GREEN);
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
