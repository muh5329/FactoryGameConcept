#include <raymath.h>
#include <vector>
#include <algorithm> 
using std::vector;


struct GridCell {
    bool walkable;
    Vector3 worldPosition;
};

class Grid {
public:
    int width, height;
    float cellSize;
    std::vector<std::vector<GridCell>> cells;

    Grid(int width, int height, float cellSize)
        : width(width), height(height), cellSize(cellSize) {
        cells.resize(height, std::vector<GridCell>(width));
        InitializeGrid();
    }

    void InitializeGrid() {
        for (int z = 0; z < height; z++) {
            for (int x = 0; x < width; x++) {
                Vector3 worldPos = {
                    x * cellSize + cellSize / 2,
                    0,
                    z * cellSize + cellSize / 2
                };
                cells[z][x] = { true, worldPos }; // default to walkable
            }
        }
    }

    GridCell& GetCellFromWorldPosition(Vector3 position) {
        int x = position.x / cellSize;
        int z = position.z / cellSize;
        x = std::clamp(x, 0, width - 1);
        z = std::clamp(z, 0, height - 1);
        return cells[z][x];
    } 

    
};
