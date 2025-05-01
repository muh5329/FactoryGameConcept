#include "raylib.h"
#include <cmath>
#include <vector>
#include <Common/constants.hpp>

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
                    position.x += direction.x * Constants::UNIT_SPEED * deltaTime;
                    position.z += direction.z * Constants::UNIT_SPEED * deltaTime;
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