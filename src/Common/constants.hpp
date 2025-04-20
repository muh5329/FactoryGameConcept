// Constants.hpp
#pragma once

namespace Constants {
    // Tile/Grid Constants
    constexpr int TILE_SIZE = 32;
    constexpr int GRID_SIZE = 25;
    constexpr int MAX_UNITS = 10;

    // Unit Behavior
    constexpr float ATTACK_RANGE = 30.0f;
    constexpr float UNIT_SPEED = 2.0f;

    // Physics Constants
    constexpr float GROUND_Y = 0.0f;
    constexpr float GRAVITY = -9.8f;
    constexpr float MOVE_SPEED = 5.0f;
    constexpr float JUMP_FORCE = 5.0f;
}

