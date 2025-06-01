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
#include <Common/grid.cpp>

#include <Entities/NPC/unit.cpp>


class Game {
private:
    std::vector<Unit> units;
    Vector2 dragStart{}, dragEnd{};
    bool isDragging = false;
    RtsCamera camera;
    World world;
    int scrollSpeed = 4; 
    Grid* navGrid;
public:
    Game() {
        InitWindow(800, 600, "3D Isometric RTS");
        SetTargetFPS(60);
        SetTraceLogLevel(0);
        

        // Init Bullet
        world.InitializePhysics();
        world.CreateGround();

        // Init Nav Grid
        navGrid = new Grid(50, 50, 1.0f);  // 50x50 grid with 1 unit per cell

       
        for (int i = 0; i < Constants::MAX_UNITS; i++) {
            units.emplace_back(&world);
        }
    }

    ~Game() {
        world.ShutdownPhysics();
        delete navGrid;
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
        world.dynamicsWorld->stepSimulation(deltaTime, 10);
        for (auto& unit : units) {
            unit.Update(deltaTime);
        }
        camera.Update();
        
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

    Vector3 GridToWorld(int x, int z) {
        return {
            x * navGrid->cellSize + navGrid->cellSize / 2,
            0,
            z * navGrid->cellSize + navGrid->cellSize / 2
        };
    }

    Vector2i WorldToGrid(Vector3 pos) {
        return {
            (int)(pos.x / navGrid->cellSize),
            (int)(pos.z / navGrid->cellSize)
        };
    }

    void UpdateGridObstacles() {
        for (auto& row : navGrid->cells) {
            for (auto& cell : row) {
                // Raycast down to detect obstacle/ground
                Ray ray = { cell.worldPosition + Vector3{0, 10, 0}, Vector3{0, -1, 0} };
                RaycastHit hit;
                cell.walkable = !RayHitsObstacle(ray, &hit); // Your custom function
            }
        }
    }

    void DrawDebugGrid(){
        for (int z = 0; z < navGrid->height; z++) {
            for (int x = 0; x < navGrid->width; x++) {
                Vector3 pos = navGrid->cells[z][x].worldPosition;
                Color color = navGrid->cells[z][x].walkable ? LIGHTGRAY : RED;
                DrawCube(pos, 0.9f, 0.1f, 0.9f, color);
            }
        }
    }


    void Draw() {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        BeginMode3D(camera);
        DrawGrid(20, 1.0f);
        DrawDebugGrid()
        for (const auto& unit : units) unit.Draw();
        EndMode3D();
        DrawText("3D Isometric RTS", 10, 10, 20, BLACK);

        if (isDragging) {
            DrawRectangleDrag();
        }

        EndDrawing();

        world.dynamicsWorld->debugDrawWorld();
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

 
};

int main() {
    Game game;
    game.Run();
    return 0;
}
