#include "grid.h"
#include <algorithm>

Grid::Grid(int width, int height, float cellSize)
    : width(width), height(height), cellSize(cellSize) {
    cells.resize(height, std::vector<GridCell>(width));
    InitializeGrid();
}

void Grid::InitializeGrid() {
    for (int z = 0; z < height; z++) {
        for (int x = 0; x < width; x++) {
            Vector3 worldPos = {
                x * cellSize + cellSize / 2,
                0,
                z * cellSize + cellSize / 2
            };
            cells[z][x] = { true, worldPos };
        }
    }
}

GridCell& Grid::GetCellFromWorldPosition(Vector3 position) {
    int x = position.x / cellSize;
    int z = position.z / cellSize;
    x = std::clamp(x, 0, width - 1);
    z = std::clamp(z, 0, height - 1);
    return cells[z][x];
}

Vector3 Grid::GridToWorld(int x, int z) {
    return {
        x * this->cellSize + this->cellSize / 2,
        0,
        z * this->cellSize + this->cellSize / 2
    };
}

Vector2 Grid::WorldToGrid(Vector3 pos) {
    return {
        static_cast<float>(static_cast<int>(pos.x / this->cellSize)),
        static_cast<float>(static_cast<int>(pos.z / this->cellSize))
    };
}


Vector3 Grid::HexToWorld(int q, int r, float size = 1.0f) {
    float x = size * sqrtf(3.0f) * (q + r / 2.0f);
    float z = size * 1.5f * r;
    return { x, 0.0f, z };
}

Vector2 Grid::WorldToHex(Vector3 pos, float size = 1.0f) {
    float q = (sqrtf(3)/3 * pos.x - 1.0f/3 * pos.z) / size;
    float r = (2.0f/3 * pos.z) / size;
    return { q, r }; // Then round to nearest hex
}


std::vector<std::vector<int>> Grid::ConvertGridToIntMap(const Grid& grid) {
    std::vector<std::vector<int>> intMap(grid.width, std::vector<int>(grid.height, 1));
    for (int z = 0; z < grid.height; ++z) {
        for (int x = 0; x < grid.width; ++x) {
            intMap[x][z] = grid.cells[z][x].walkable ? 0 : 1;
        }
    }
    return intMap;
}
