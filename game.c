#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_UNITS 10
#define TILE_SIZE 32
#define ATTACK_RANGE 30.0f
#define UNIT_SPEED 2.0f
#define GRID_SIZE 25

typedef struct {
    Vector3 position;
    Vector3 target;
    bool selected;
    bool moving;
    bool attacking;
    int targetIndex;
} Unit;

Unit units[MAX_UNITS];
int selectedUnit = -1;

void InitUnits() {
    for (int i = 0; i < MAX_UNITS; i++) {
        units[i].position = (Vector3){ GetRandomValue(0, 10), 0, GetRandomValue(0, 10) };
        units[i].target = units[i].position;
        units[i].selected = false;
        units[i].moving = false;
        units[i].attacking = false;
        units[i].targetIndex = -1;
    }
}

void DrawUnits() {
    for (int i = 0; i < MAX_UNITS; i++) {
        Color color = (i == selectedUnit) ? RED : BLUE;
        DrawCube(units[i].position, 0.5f, 0.5f, 0.5f, color);
    }
}

void UpdateUnits() {
    for (int i = 0; i < MAX_UNITS; i++) {
        if (units[i].moving) {
            Vector3 direction = { units[i].target.x - units[i].position.x, 0, units[i].target.z - units[i].position.z };
            float distance = sqrtf(direction.x * direction.x + direction.z * direction.z);
            if (distance > 0.1f) {
                direction.x /= distance;
                direction.z /= distance;
                units[i].position.x += direction.x * UNIT_SPEED * GetFrameTime();
                units[i].position.z += direction.z * UNIT_SPEED * GetFrameTime();
            } else {
                units[i].moving = false;
            }
        }
    }
}

Vector3 GetRayGroundIntersection(Ray ray) {
    if (ray.direction.y == 0) return ray.position; // Prevent division by zero
    float t = -ray.position.y / ray.direction.y;
    return (Vector3){ ray.position.x + t * ray.direction.x, 0, ray.position.z + t * ray.direction.z };
}

void HandleInput(Camera3D *camera) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Ray ray = GetMouseRay(GetMousePosition(), *camera);
        for (int i = 0; i < MAX_UNITS; i++) {
            if (GetRayCollisionBox(ray, (BoundingBox){
                (Vector3){units[i].position.x - 0.25f, 0, units[i].position.z - 0.25f},
                (Vector3){units[i].position.x + 0.25f, 0.5f, units[i].position.z + 0.25f}}).hit) {
                selectedUnit = i;
                break;
            }
        }
    }
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && selectedUnit != -1) {
        Ray ray = GetMouseRay(GetMousePosition(), *camera);
        Vector3 targetPos = GetRayGroundIntersection(ray);
        units[selectedUnit].target = targetPos;
        units[selectedUnit].moving = true;
    }
}

void UpdateCameraManual(Camera3D *camera) {
    // Camera movement using keyboard
    if (IsKeyDown(KEY_W)) camera->position.z -= 0.1f;
    if (IsKeyDown(KEY_S)) camera->position.z += 0.1f;
    if (IsKeyDown(KEY_A)) camera->position.x -= 0.1f;
    if (IsKeyDown(KEY_D)) camera->position.x += 0.1f;

    // Keep the target fixed (no pitch/yaw change from mouse)
    camera->target = (Vector3){ 0.0f, 0.0f, 0.0f };
}

int main(void) {
    InitWindow(800, 600, "3D Isometric RTS");
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;

    Vector2 dragStart = { 0 }, dragEnd = { 0 };
    bool isDragging = false;

    InitUnits();
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

         // Input: selection
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            dragStart = GetMousePosition();
            isDragging = true;
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            dragEnd = GetMousePosition();
            isDragging = false;
            Rectangle selection = (Rectangle){ 
                fminf(dragStart.x, dragEnd.x), fminf(dragStart.y, dragEnd.y), 
                fabsf(dragStart.x - dragEnd.x), fabsf(dragStart.y - dragEnd.y)
            };
            //SelectUnitAtMouse();
        }


        UpdateCameraManual(&camera);  // Manually control camera
        HandleInput(&camera);
        UpdateUnits();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        DrawGrid(20, 1.0f);
        DrawUnits();
        EndMode3D();
        DrawText("3D Isometric RTS", 10, 10, 20, BLACK);

        if (isDragging) {
            Vector2 mousePos = GetMousePosition();
            DrawRectangleLinesEx((Rectangle){
                fminf(dragStart.x, mousePos.x),
                fminf(dragStart.y, mousePos.y),
                fabsf(mousePos.x - dragStart.x),
                fabsf(mousePos.y - dragStart.y)
            }, 1, GREEN);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}