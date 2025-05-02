#include "raylib.h"

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

class RtsCamera : public Camera3D{

    public:
        bool toggle;
        int scrollSpeed = 4; 

    public: 
        RtsCamera(){
            position = { 10.0f, 10.0f, 10.0f };
            target = { 0.0f, 0.0f, 0.0f };
            up = { 0.0f, 1.0f, 0.0f };
            fovy = 45.0f;
            projection = CAMERA_PERSPECTIVE;
        }
        ~RtsCamera(){

        }

        // Call this every frame to update the camera based on input
        void Update() {
            if (IsKeyDown(KEY_W)){
                position.z -= 0.1f;
                position.x -= 0.1f;
            } 
            if (IsKeyDown(KEY_S)){
                position.z += 0.1f;
            } 
            if (IsKeyDown(KEY_A)) {
                position.x -= 0.1f;
            }
            if (IsKeyDown(KEY_D)){
                position.x += 0.1f;
            } 
        
            // Test Pitch and Yaw
            position.y -= (int)(GetMouseWheelMove()*scrollSpeed);

            if (position.y < 0) position.y  = 0;
    
            target = { 0.0f, 0.0f, 0.0f }; // Lock target to center
        }
    private:
};