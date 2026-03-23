#include "map/Pathfinding.h"
#include <cmath>
#include <queue>
#include <vector>
#include <unordered_map>
#include <algorithm>

// ── Helpers ──────────────────────────────────────────────────────────────────

void Pathfinding::init(int mapWidth, int mapHeight, int cs) {
    cellSize = cs;
    gridW = (mapWidth  + cs - 1) / cs;
    gridH = (mapHeight + cs - 1) / cs;
    blocked.assign(gridH, std::vector<bool>(gridW, false));
}

void Pathfinding::setObstacle(int gx, int gy, bool isBlocked) {
    if (gx >= 0 && gx < gridW && gy >= 0 && gy < gridH)
        blocked[gy][gx] = isBlocked;
}

void Pathfinding::setObstacleWorldPos(float wx, float wy, float radius) {
    int cells = static_cast<int>(std::ceil(radius / static_cast<float>(cellSize)));
    auto [cx, cy] = worldToGrid({wx, wy});
    for (int dy = -cells; dy <= cells; ++dy)
        for (int dx = -cells; dx <= cells; ++dx)
            setObstacle(cx + dx, cy + dy, true);
}

Vec2 Pathfinding::gridToWorld(int gx, int gy) const {
    return {(static_cast<float>(gx) + 0.5f) * static_cast<float>(cellSize),
            (static_cast<float>(gy) + 0.5f) * static_cast<float>(cellSize)};
}

std::pair<int, int> Pathfinding::worldToGrid(const Vec2& wp) const {
    int gx = static_cast<int>(wp.x / static_cast<float>(cellSize));
    int gy = static_cast<int>(wp.y / static_cast<float>(cellSize));
    gx = std::clamp(gx, 0, gridW - 1);
    gy = std::clamp(gy, 0, gridH - 1);
    return {gx, gy};
}

float Pathfinding::heuristic(int x1, int y1, int x2, int y2) const {
    float dx = static_cast<float>(x1 - x2);
    float dy = static_cast<float>(y1 - y2);
    return std::sqrt(dx * dx + dy * dy);
}

// ── A* ───────────────────────────────────────────────────────────────────────

struct Node {
    int x, y;
    float g, h;
    int parentX = -1, parentY = -1;
    float f() const { return g + h; }
    bool operator>(const Node& o) const { return f() > o.f(); }
};

std::vector<Vec2> Pathfinding::findPath(const Vec2& start, const Vec2& end) const {
    auto [sx, sy] = worldToGrid(start);
    auto [ex, ey] = worldToGrid(end);

    if (sx == ex && sy == ey)
        return {end};

    // open set as min-heap
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open;
    // cost and parent maps using (y*gridW + x) as key
    std::vector<float> gCost(gridW * gridH, 1e30f);
    std::vector<bool>  closed(gridW * gridH, false);
    std::vector<int>   parentX(gridW * gridH, -1);
    std::vector<int>   parentY(gridW * gridH, -1);

    auto idx = [&](int x, int y){ return y * gridW + x; };

    gCost[idx(sx, sy)] = 0.0f;
    open.push({sx, sy, 0.0f, heuristic(sx, sy, ex, ey)});

    const int dx[] = {1,-1, 0, 0, 1,-1, 1,-1};
    const int dy[] = {0, 0, 1,-1, 1,-1,-1, 1};
    const float dc[] = {1,1,1,1,1.414f,1.414f,1.414f,1.414f};

    bool found = false;
    while (!open.empty()) {
        Node cur = open.top(); open.pop();
        int ci = idx(cur.x, cur.y);
        if (closed[ci]) continue;
        closed[ci] = true;

        if (cur.x == ex && cur.y == ey) { found = true; break; }

        for (int d = 0; d < 8; ++d) {
            int nx = cur.x + dx[d];
            int ny = cur.y + dy[d];
            if (nx < 0 || nx >= gridW || ny < 0 || ny >= gridH) continue;
            if (blocked[ny][nx]) continue;
            int ni = idx(nx, ny);
            if (closed[ni]) continue;
            float ng = gCost[ci] + dc[d];
            if (ng < gCost[ni]) {
                gCost[ni]   = ng;
                parentX[ni] = cur.x;
                parentY[ni] = cur.y;
                open.push({nx, ny, ng, heuristic(nx, ny, ex, ey)});
            }
        }
    }

    if (!found) return {end}; // fallback: go straight

    // Reconstruct path
    std::vector<Vec2> path;
    int cx = ex, cy2 = ey;
    while (!(cx == sx && cy2 == sy)) {
        path.push_back(gridToWorld(cx, cy2));
        int i  = idx(cx, cy2);
        int px = parentX[i];
        int py = parentY[i];
        cx = px; cy2 = py;
    }
    std::reverse(path.begin(), path.end());
    path.push_back(end);
    return path;
}
