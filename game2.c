// #include "raylib.h"
// #include "Newton.h"
// #include <math.h>

// #define DEG2RAD (PI / 180.0f)

// NewtonBody* CreateCapsuleCharacter(NewtonWorld* world, Vector3 pos, float radius, float height, float mass) {
//     // Newton capsule is aligned along Y-axis
//     NewtonCollision* shape = NewtonCreateCapsule(world, radius, radius, height, 0, NULL);
//     NewtonBody* body = NewtonCreateDynamicBody(world, shape, MatrixIdentity());
//     NewtonReleaseCollision(world, shape);

//     // Set inertia
//     Vector3 com = { 0 };
//     Vector3 inertia;
//     NewtonConvexCollisionCalculateInertialMatrix(shape, (float*)&inertia, (float*)&com);
//     NewtonBodySetMassMatrix(body, mass, inertia.x, inertia.y, inertia.z);

//     // Position it
//     Matrix mat = MatrixTranslate(pos.x, pos.y, pos.z);
//     NewtonBodySetMatrix(body, (float*)&mat);

//     // Dampen rotation (keep upright)
//     NewtonBodySetAutoSleep(body, 0);
//     NewtonBodySetAngularDamping(body, (const dFloat[]){ 0.9f, 0.9f, 0.9f });

//     return body;
// }

// int main() {
//     InitWindow(800, 600, "Capsule Movement - Raylib + Newton");
//     Camera3D camera = { 0 };
//     camera.position = (Vector3){ 0, 5, 10 };
//     camera.target = (Vector3){ 0, 1, 0 };
//     camera.up = (Vector3){ 0, 1, 0 };
//     camera.fovy = 45.0f;
//     camera.projection = CAMERA_PERSPECTIVE;

//     // Newton world setup
//     NewtonWorld* world = NewtonCreate();
//     NewtonSetPlatformArchitecture(world);
//     NewtonSetSolverModel(world, 1);

//     // Ground
//     NewtonCollision* groundShape = NewtonCreateBox(world, 50, 1, 50, 0, NULL);
//     NewtonBody* ground = NewtonCreateBody(world, groundShape, MatrixIdentity());
//     NewtonReleaseCollision(world, groundShape);
//     Matrix groundMat = MatrixTranslate(0, -0.5f, 0);
//     NewtonBodySetMatrix(ground, (float*)&groundMat);

//     // Capsule unit
//     float capsuleRadius = 0.5f;
//     float capsuleHeight = 2.0f;
//     Vector3 startPos = { 0, 3, 0 };
//     NewtonBody* capsule = CreateCapsuleCharacter(world, startPos, capsuleRadius, capsuleHeight, 1.0f);
//     Model capsuleModel = LoadModelFromMesh(GenMeshCylinder(capsuleRadius, capsuleHeight));

//     SetTargetFPS(60);

//     while (!WindowShouldClose()) {
//         float dt = GetFrameTime();
//         NewtonUpdate(world, dt);

//         // Input movement
//         Vector3 move = { 0 };
//         if (IsKeyDown(KEY_W)) move.z -= 1;
//         if (IsKeyDown(KEY_S)) move.z += 1;
//         if (IsKeyDown(KEY_A)) move.x -= 1;
//         if (IsKeyDown(KEY_D)) move.x += 1;

//         // Normalize direction
//         float len = sqrtf(move.x * move.x + move.z * move.z);
//         if (len > 0.01f) {
//             move.x /= len;
//             move.z /= len;

//             // Apply force (or use velocity)
//             float speed = 10.0f;
//             move.x *= speed;
//             move.z *= speed;
//             Vector3 vel = { move.x, 0, move.z };
//             NewtonBodySetVelocity(capsule, (const dFloat*)&vel);
//         } else {
//             // Stop movement when no inputAÅ
//             Vector3 stop = { 0 };
//             NewtonBodySetVelocity(capsule, (const dFloat*)&stop);
//         }

//         // Get transform from physics
//         dFloat matrix[16];
//         NewtonBodyGetMatrix(capsule, matrix);
//         Matrix transform = *(Matrix*)matrix;

//         // Rendering
//         BeginDrawing();
//         ClearBackground(RAYWHITE);
//         BeginMode3D(camera);

//         DrawModel(capsuleModel, (Vector3){ transform.m12, transform.m13 - capsuleHeight/2, transform.m14 }, 1.0f, BLUE);
//         DrawCube((Vector3){ 0, -0.5f, 0 }, 50, 1, 50, GRAY);

//         EndMode3D();
//         DrawFPS(10, 10);
//         EndDrawing();
//     }

//     NewtonDestroyBody(world, capsule);
//     NewtonDestroyBody(world, ground);
//     NewtonDestroy(world);
//     UnloadModel(capsuleModel);
//     CloseWindow();
//     return 0;
// }