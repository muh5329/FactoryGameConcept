#include "raylib.h"
#include "raymath.h"
class FpsCamera : public  Camera3D{

    public:
        // Constructor
        FpsCamera() {
            position = { 0.0f, 2.0f, 4.0f };
            target = { 0.0f, 2.0f, 0.0f };
            up = { 0.0f, 1.0f, 0.0f };
            fovy = 45.0f;
            projection = CAMERA_PERSPECTIVE;
        }

        // Call this every frame to update the camera based on input
        void Update() {
            if (IsKeyDown(KEY_W)) position.z -= 0.1f;
            if (IsKeyDown(KEY_S)) position.z += 0.1f;
            if (IsKeyDown(KEY_A)) position.x -= 0.1f;
            if (IsKeyDown(KEY_D)) position.x += 0.1f;
    
            target = { 0.0f, 0.0f, 0.0f }; // Lock target to center
        }

    private:


};

class RtsCamera : public Camera3D {
    public:
        bool toggle;
        int scrollSpeed = 4;
    
    public:
        RtsCamera() {
            position = { 10.0f, 10.0f, 10.0f };
            target = { position.x + -1.0f, position.y - 1.0f, position.z + -1.0f }; // Maintain isometric view
            up = { 0.0f, 1.0f, 0.0f };
            fovy = 45.0f;
            projection = CAMERA_PERSPECTIVE;
        }
    
        void Update() {
            float moveSpeed = 0.2f;
    
            // Right vector from camera direction
            Vector3 direction = Vector3Normalize(Vector3Subtract(target, position));
            Vector3 right = Vector3Normalize(Vector3CrossProduct(direction, up));
            Vector3 forward = Vector3Normalize(Vector3CrossProduct(up, right));
    
            if (IsKeyDown(KEY_W)) {
                position = Vector3Add(position, Vector3Scale(forward, moveSpeed));
                target = Vector3Add(target, Vector3Scale(forward, moveSpeed));
            }
            if (IsKeyDown(KEY_S)) {
                position = Vector3Subtract(position, Vector3Scale(forward, moveSpeed));
                target = Vector3Subtract(target, Vector3Scale(forward, moveSpeed));
            }
            if (IsKeyDown(KEY_A)) {
                position = Vector3Subtract(position, Vector3Scale(right, moveSpeed));
                target = Vector3Subtract(target, Vector3Scale(right, moveSpeed));
            }
            if (IsKeyDown(KEY_D)) {
                position = Vector3Add(position, Vector3Scale(right, moveSpeed));
                target = Vector3Add(target, Vector3Scale(right, moveSpeed));
            }
    
            // Zoom in/out (move up/down)
            float wheel = GetMouseWheelMove();
            if (wheel != 0.0f) {
                Vector3 zoomDir = Vector3Normalize(Vector3Subtract(target, position));
                Vector3 zoomMove = Vector3Scale(zoomDir, wheel * scrollSpeed);
                position = Vector3Add(position, zoomMove);
            }

             // Middle mouse drag movement
            if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
                Vector2 mouseDelta = GetMouseDelta();
                float dragSpeed = 0.01f; // Adjust as needed for sensitivity

                // Move opposite to mouse drag direction to simulate "grabbing" the world
                position = Vector3Subtract(position, Vector3Scale(right, mouseDelta.x * dragSpeed));
                target = Vector3Subtract(target, Vector3Scale(right, mouseDelta.x * dragSpeed));

                position = Vector3Add(position, Vector3Scale(forward, mouseDelta.y * dragSpeed));
                target = Vector3Add(target, Vector3Scale(forward, mouseDelta.y * dragSpeed));
            }
        }
};
    