#pragma once

#include <raymath.h>
#include <vector>

struct GridCell {
    bool walkable;
    Vector3 worldPosition;
};

class Grid {
public:
    int width, height;
    float cellSize;
    std::vector<std::vector<GridCell>> cells;

    Grid(int width, int height, float cellSize);

    void InitializeGrid();
    GridCell& GetCellFromWorldPosition(Vector3 position);
    Vector3 GridToWorld(int x, int z);
    Vector2 WorldToGrid(Vector3 pos);
    std::vector<std::vector<int>> ConvertGridToIntMap(const Grid& grid);
    Vector3 HexToWorld(int q, int r, float size = 1.0f);
    Vector2 WorldToHex(Vector3 pos, float size = 1.0f);
};
