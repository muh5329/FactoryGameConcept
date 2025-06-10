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
#include <Common/grid.h> 
#include <Common/pathfinding/Pathfinder.h>
#include <Entities/NPC/unit.cpp>

struct RaycastHit {
    Vector3 position;
    bool hit;
};

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
            unit.Update(deltaTime, navGrid);
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
            CalculateUnitsPath();
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

    void DrawDebugGrid() {
        for (int z = 0; z < navGrid->height; z++) {
            for (int x = 0; x < navGrid->width; x++) {
                Vector3 pos = navGrid->cells[z][x].worldPosition;
                Color color = navGrid->cells[z][x].walkable ? LIGHTGRAY : RED;
                DrawCube(pos, 0.9f, 0.1f, 0.9f, color);
            }
        }
    }

    void CalculateUnitsPath(){
        for (auto& unit : units){
            Vector2 startGrid = navGrid->WorldToGrid(unit.position);
            Vector2 goalGrid = navGrid->WorldToGrid(unit.target);

            Node startNode(static_cast<int>(startGrid.x), static_cast<int>(startGrid.y));
            Node goalNode(static_cast<int>(goalGrid.x), static_cast<int>(goalGrid.y));

            auto intGrid = navGrid->ConvertGridToIntMap(*navGrid);
            std::vector<Node> path = FindPath(intGrid, startNode, goalNode);
            unit.path = path;
        } 
    }

    void Draw() {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawGrid(20, 1.0f);

        // Pathfinding Logic
        DrawDebugGrid();
        for (const auto& unit : units){
            // Visualize path 
            for (const Node& node : unit.path) {
                Vector3 pos = navGrid->GridToWorld(node.x, node.y);
                DrawCubeWires(pos, 1.0f, 0.1f, 1.0f, BLUE);
            }
           unit.Draw(); 
        } 


        EndMode3D();

        DrawText("3D Isometric RTS", 10, 10, 20, BLACK);

        if (isDragging) {
            DrawRectangleDrag();
        }

        EndDrawing();

        world.dynamicsWorld->debugDrawWorld();
    }

    void DrawRectangleDrag() {
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
