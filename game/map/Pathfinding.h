#pragma once
#include "Vec2.h"
#include <vector>
#include <utility>

class Pathfinding {
public:
    void init(int mapWidth, int mapHeight, int cellSize = 100);
    void setObstacle(int gridX, int gridY, bool blocked);
    void setObstacleWorldPos(float wx, float wy, float radius);

    // Returns world-space waypoints from start to end
    std::vector<Vec2> findPath(const Vec2& start, const Vec2& end) const;

private:
    int gridW   = 0;
    int gridH   = 0;
    int cellSize = 100;
    std::vector<std::vector<bool>> blocked; // [y][x]

    Vec2 gridToWorld(int gx, int gy) const;
    std::pair<int, int> worldToGrid(const Vec2& wp) const;

    float heuristic(int x1, int y1, int x2, int y2) const;
};
