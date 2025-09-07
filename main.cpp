// main.cpp
// Build example (Linux):
//   g++ main.cpp -o platformer -O2 -std=c++17 -lraylib -lode -lm -lpthread -ldl
//
// On macOS you may need to use brew-installed libraries and link accordingly.
// Make sure both Raylib and ODE are installed and in your linker path.

// g++ -I/opt/homebrew/include -L/opt/homebrew/lib -lode -Iinclude -Llib/darwin -lraylib -o main main.cpp \
-framework CoreFoundation -framework Cocoa -framework IOKit -framework CoreVideo -framework CoreGraphics -framework AppKit


// brew install ode

#include <raylib.h>
#include <ode/ode.h>
#include <cmath>
#include <cstdio>
#include <vector>

// --- ODE globals ---
static dWorldID g_world = nullptr;
static dSpaceID g_space = nullptr;
static dJointGroupID g_contactgroup = nullptr;
static bool g_playerOnGround = false;
static bool g_playerWasOnGround = false;
static dGeomID g_playerGeom = nullptr;
static dBodyID g_playerBody = nullptr;

// Constants
const float STEP_SIZE = 1.0f / 120.0f; // physics fixed step
const int MAX_CONTACTS = 8;

// Utility: convert ODE vector to Raylib Vector3
static Vector3 odeToVec3(const dReal *v) {
    return Vector3{ float(v[0]), float(v[1]), float(v[2]) };
}

// nearCallback for collisions: create contact joints and detect ground for player
static void nearCallback(void * /*data*/, dGeomID o1, dGeomID o2) {
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);

    dContact contact[MAX_CONTACTS];
    int n = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact));
    for (int i = 0; i < n; ++i) {
        // basic contact surface params
        contact[i].surface.mode = dContactBounce | dContactApprox1;
        contact[i].surface.mu = dInfinity;
        contact[i].surface.bounce = 0.0;
        contact[i].surface.bounce_vel = 0.0;

        dJointID c = dJointCreateContact(g_world, g_contactgroup, &contact[i]);
        dJointAttach(c, b1, b2);

        // check if one of the geoms is the player and if normal points upward
        // contact[i].geom.normal is the contact normal
        const dReal *normal = contact[i].geom.normal;
        bool involvesPlayer = (o1 == g_playerGeom || o2 == g_playerGeom);
        if (involvesPlayer) {
            // Consider "ground" if normal's Y component points up sufficiently.
            // We assume world Y is up (consistent with Raylib's default).
            if (normal[1] > 0.5) {
                g_playerOnGround = true;
            }
        }
    }
}

// Helper: create a static box in ODE (no body, just geom)
dGeomID createStaticBox(dSpaceID space, float lx, float ly, float lz, const Vector3 &pos) {
    dGeomID box = dCreateBox(space, lx, ly, lz);
    dMatrix3 R; // identity rotation
    dGeomSetPosition(box, pos.x, pos.y, pos.z);
    return box;
}

// Helper: create a dynamic box body + geom (not used for player here, but available)
void createDynamicBox(dSpaceID space, dWorldID world, const Vector3 &size, const Vector3 &pos, double density=1.0) {
    dBodyID body = dBodyCreate(world);
    dMass m;
    dMassSetBox(&m, density, size.x, size.y, size.z);
    dBodySetMass(body, &m);
    dBodySetPosition(body, pos.x, pos.y, pos.z);
    dGeomID geom = dCreateBox(space, size.x, size.y, size.z);
    dGeomSetBody(geom, body);
}

// Create player capsule: returns geom and sets global body
void createPlayerCapsule(dSpaceID space, dWorldID world, float radius, float length, const Vector3 &pos) {
    // Create body
    g_playerBody = dBodyCreate(world);
    dMass m;
    double density = 1.0;
    // direction 1 = along Y (we want capsule upright in Y)
    dMassSetCapsule(&m, density, 1, radius, length);
    dBodySetMass(g_playerBody, &m);
    dBodySetPosition(g_playerBody, pos.x, pos.y, pos.z);
    // Create geom and attach to body
    g_playerGeom = dCreateCapsule(space, radius, length);
    dGeomSetBody(g_playerGeom, g_playerBody);
    // Give a bit of damping for stability
    dBodySetDamping(g_playerBody, 0.2, 0.2);
}

// Draw capsule (approx) using rays - draw cylinder and spheres on ends
void DrawCapsule(const Vector3 &pos, float radius, float length, Color color) {
    // Raylib has DrawCylinder and DrawSphere; to approximate a capsule draw cylinder and two hemispheres (spheres).
    // Capsule in this code is oriented with Y up. The cylinder height equals length, spheres at ends.
    DrawCylinderWires({pos.x, pos.y - length/2.0f, pos.z}, radius, radius, length, 16, DARKGRAY);
    DrawCylinder({pos.x, pos.y - length/2.0f, pos.z}, radius, radius, length, 16, color); // body
    DrawSphere({pos.x, pos.y - length/2.0f + length/2.0f, pos.z}, radius, color); // top? adjust below
    // Wait — above coordinates are wrong because of how DrawCylinder's position semantics vary.
    // We'll instead draw cylinder centered at pos and spheres at pos +/- length/2 in Y.
    DrawCylinder({pos.x, pos.y, pos.z}, radius, radius, length, 16, color);
    DrawSphere({pos.x, pos.y + length/2.0f, pos.z}, radius, color);
    DrawSphere({pos.x, pos.y - length/2.0f, pos.z}, radius, color);
}

int main() {
    // Raylib init
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Raylib + ODE 3D Platformer (Third-person)");
    SetTargetFPS(60);

    // Initialize ODE
    dInitODE();
    g_world = dWorldCreate();
    g_space = dHashSpaceCreate(0);
    g_contactgroup = dJointGroupCreate(0);
    // Set gravity (Y up)
    dWorldSetGravity(g_world, 0, -9.81, 0);
    dWorldSetERP(g_world, 0.2);
    dWorldSetCFM(g_world, 1e-5);

    // Create a plane ground in ODE (y = 0)
    dGeomID groundPlane = dCreatePlane(g_space, 0, 1, 0, 0);

    // Create a few static boxes (platforms)
    std::vector<dGeomID> platforms;
    // big ground box for visuals (we'll render large plane separately)
    // platform center positions and sizes
    struct Plat { Vector3 pos; Vector3 size; };
    std::vector<Plat> plats = {
        {{0, -0.5f, 0}, {50.0f, 1.0f, 50.0f}},
        {{4.0f, 1.5f, -2.0f}, {4.0f, 0.5f, 4.0f}},
        {{-5.0f, 3.0f, 3.0f}, {3.0f, 0.5f, 3.0f}},
        {{8.0f, 5.0f, 2.0f}, {2.0f, 0.5f, 2.0f}},
        {{-8.0f, 8.0f, -4.0f}, {3.0f, 0.5f, 3.0f}},
    };

    for (auto &p : plats) {
        dGeomID g = dCreateBox(g_space, p.size.x, p.size.y, p.size.z);
        dGeomSetPosition(g, p.pos.x, p.pos.y, p.pos.z);
        platforms.push_back(g);
    }

    // Create player capsule
    float playerRadius = 0.4f;
    float playerLength = 1.2f; // cylinder length (not including hemispheres)
    Vector3 playerStart = {0.0f, 2.0f, 0.0f};
    createPlayerCapsule(g_space, g_world, playerRadius, playerLength, playerStart);

    // Raylib camera (3rd-person)
    Camera camera = { 0 };
    camera.position = { 0.0f, 3.0f, 6.0f }; // camera position
    camera.target = { 0.0f, 1.0f, 0.0f };   // camera looks at player
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Simple world objects for rendering (matching platforms)
    Model cubeModel = {0};

    // Player control variables
    const float moveSpeed = 6.0f; // meters per second (approx)
    const float jumpImpulse = 100.0f;
    const float maxSlopeGroundDot = 0.5f;

    // Time tracking for physics stepping
    double accumulator = 0.0;
    double prevTime = GetTime();

    // Hide cursor for game feel
    DisableCursor();


    // Main loop
    while (!WindowShouldClose()) {
        // --- Input & camera handling ---
        // Mouse to rotate camera around player
        Vector2 mouseDelta = GetMouseDelta();
        static float yaw = 0.0f;
        static float pitch = -10.0f;
        yaw += mouseDelta.x * 0.15f;
        pitch += mouseDelta.y * 0.12f;
        if (pitch > 45) pitch = 45;
        if (pitch < -80) pitch = -80;

        // Movement relative to camera forward
        bool forward = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
        bool back = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
        bool left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
        bool right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
        bool jumpPressed = IsKeyPressed(KEY_SPACE);

        // Compute desired movement direction in world space based on camera yaw
        float radYaw = yaw * DEG2RAD;
        Vector3 camForward = { sinf(radYaw), 0.0f, cosf(radYaw) }; // forward relative to yaw
        Vector3 camRight = { camForward.z, 0.0f, -camForward.x };  // right is perpendicular

        Vector3 moveDir = {0,0,0};
        if (forward) { moveDir.x += camForward.x; moveDir.z += camForward.z; }
        if (back)    { moveDir.x -= camForward.x; moveDir.z -= camForward.z; }
        if (left)    { moveDir.x -= camRight.x;   moveDir.z -= camRight.z; }
        if (right)   { moveDir.x += camRight.x;   moveDir.z += camRight.z; }

        // Normalize moveDir
        float len = sqrtf(moveDir.x*moveDir.x + moveDir.z*moveDir.z);
        if (len > 0.001f) {
            moveDir.x /= len; moveDir.z /= len;
        }

        // --- Physics stepping (fixed) ---
        double current = GetTime();
        double frameTime = current - prevTime;
        prevTime = current;
        accumulator += frameTime;

        // Track previous grounded state
        g_playerWasOnGround = g_playerOnGround;
        // Reset per-frame ground flag; nearCallback will set it if contact occurs this step
        g_playerOnGround = false;

        bool jumpQueued = false;
        // Only queue jump if just pressed and was on ground last frame
        if (jumpPressed && g_playerWasOnGround) {
            jumpQueued = true;
        }

        // Run ODE collisions & step multiple times if needed
        while (accumulator >= STEP_SIZE) {
            // Collision detection
            dSpaceCollide(g_space, 0, &nearCallback);

            // Apply basic player control: set lateral velocity directly for stable platformer feel
            const dReal* pos = dBodyGetPosition(g_playerBody);
            const dReal* vel = dBodyGetLinearVel(g_playerBody);
            // compute desired horizontal velocity
            dReal desiredVx = moveDir.x * moveSpeed;
            dReal desiredVz = moveDir.z * moveSpeed;

            // We keep vertical velocity untouched (y)
            dReal vy = vel[1];

            // Smooth velocity: compute impulse to reach desired velocity (simple P-control)
            dMass m;
            dBodyGetMass(g_playerBody, &m);
            dReal mass = m.mass;
            dReal k = 25.0; // responsiveness
            dReal fx = k * (desiredVx - vel[0]);
            dReal fz = k * (desiredVz - vel[2]);
            dBodyAddForce(g_playerBody, fx * mass, 0, fz * mass);

            // Jump impulse: only apply once per jump
            if (jumpQueued) {
                dBodySetLinearVel(g_playerBody, vel[0], vel[1] + jumpImpulse, vel[2]);
                jumpQueued = false;
            }

            // Step the simulation
            dWorldStep(g_world, STEP_SIZE);

            // Remove contact joints
            dJointGroupEmpty(g_contactgroup);

            accumulator -= STEP_SIZE;
        }

        // Update camera target to player's position
        const dReal* ppos = dBodyGetPosition(g_playerBody);
        Vector3 playerPos = odeToVec3(ppos);

        // Smooth camera follow: desired camera position is behind and above player
        float camDistance = 6.0f;
        float camHeight = 2.2f;
        float yawRad = yaw * DEG2RAD;
        Vector3 desiredCamPos = {
            playerPos.x - sinf(yawRad) * camDistance,
            playerPos.y + camHeight,
            playerPos.z - cosf(yawRad) * camDistance
        };
        // Smooth interp
        camera.position.x = camera.position.x * 0.90f + desiredCamPos.x * 0.10f;
        camera.position.y = camera.position.y * 0.90f + desiredCamPos.y * 0.10f;
        camera.position.z = camera.position.z * 0.90f + desiredCamPos.z * 0.10f;

        // camera target is player + small offset
        camera.target.x = camera.target.x * 0.85f + (playerPos.x) * 0.15f;
        camera.target.y = camera.target.y * 0.85f + (playerPos.y + 0.6f) * 0.15f;
        camera.target.z = camera.target.z * 0.85f + (playerPos.z) * 0.15f;

        // --- Rendering ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // Draw ground plane
        DrawPlane({0,0,0}, {50, 50}, LIGHTGRAY); // large plane
        DrawGrid(20, 1.0f);

        // Draw platforms
        for (size_t i = 0; i < plats.size(); ++i) {
            auto &p = plats[i];
            Vector3 pos = p.pos;
            Vector3 size = p.size;
            // Draw a box at pos with extents size (DrawCube takes full widths)
            DrawCube(pos, size.x, size.y, size.z, DARKBROWN);
            DrawCubeWires(pos, size.x, size.y, size.z, BLACK);
        }

        // Draw player (approx capsule). Note: ODE capsule orientation: cylinder central axis along Y (we created as such).
        DrawCapsule(playerPos, playerRadius, playerLength, RED);

        // Optionally draw player velocity arrow
        const dReal* vel = dBodyGetLinearVel(g_playerBody);
        DrawLine3D(playerPos, {playerPos.x + float(vel[0]), playerPos.y + float(vel[1]), playerPos.z + float(vel[2])}, BLUE);

        EndMode3D();

        // HUD
        DrawText("WASD to move, SPACE to jump, mouse to rotate camera", 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Player Y: %.2f", playerPos.y), 10, 40, 18, BLACK);
        DrawText(TextFormat("Grounded: %s", g_playerOnGround ? "YES":"NO"), 10, 64, 18, g_playerOnGround ? GREEN : RED);

        EndDrawing();
    }

    // Cleanup ODE
    dJointGroupDestroy(g_contactgroup);
    dSpaceDestroy(g_space);
    dWorldDestroy(g_world);
    dCloseODE();

    CloseWindow();
    return 0;
}
