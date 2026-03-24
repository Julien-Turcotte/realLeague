#include "map/Map.h"
#include "render/Renderer.h"
#include <cmath>
#include <algorithm>
#include <limits>

// ── Lane waypoints ──────────────────────────────────────────────────────────

void Map::initLanes() {
    // Lane 0 – Top lane
    // Blue travels: base → top-left corner → red base
    lanes[0].blueWaypoints = {
        {{200.0f, 2800.0f}},
        {{200.0f, 2200.0f}},
        {{200.0f, 1500.0f}},
        {{200.0f,  800.0f}},
        {{200.0f,  200.0f}},
        {{800.0f,  200.0f}},
        {{1500.0f, 200.0f}},
        {{2200.0f, 200.0f}},
        {{2800.0f, 200.0f}},
    };
    // Red travels the same points in reverse
    lanes[0].redWaypoints = std::vector<LaneWaypoint>(
        lanes[0].blueWaypoints.rbegin(), lanes[0].blueWaypoints.rend());

    // Lane 1 – Mid lane (diagonal)
    lanes[1].blueWaypoints = {
        {{200.0f,  2800.0f}},
        {{600.0f,  2400.0f}},
        {{1000.0f, 2000.0f}},
        {{1500.0f, 1500.0f}},
        {{2000.0f, 1000.0f}},
        {{2400.0f,  600.0f}},
        {{2800.0f,  200.0f}},
    };
    lanes[1].redWaypoints = std::vector<LaneWaypoint>(
        lanes[1].blueWaypoints.rbegin(), lanes[1].blueWaypoints.rend());

    // Lane 2 – Bot lane
    // Blue travels: base → right → up to red base
    lanes[2].blueWaypoints = {
        {{200.0f,  2800.0f}},
        {{800.0f,  2800.0f}},
        {{1500.0f, 2800.0f}},
        {{2200.0f, 2800.0f}},
        {{2800.0f, 2800.0f}},
        {{2800.0f, 2200.0f}},
        {{2800.0f, 1500.0f}},
        {{2800.0f,  800.0f}},
        {{2800.0f,  200.0f}},
    };
    lanes[2].redWaypoints = std::vector<LaneWaypoint>(
        lanes[2].blueWaypoints.rbegin(), lanes[2].blueWaypoints.rend());
}

void Map::initObstacles() {
    // River obstacles (approximated as circles along the diagonal)
    for (int i = 0; i < 10; ++i) {
        float t = (i + 0.5f) / 10.0f;
        obstacles.push_back({{t * 3000.0f, (1.0f - t) * 3000.0f}, 80.0f});
    }
}

void Map::init() {
    initLanes();
    initObstacles();
}

// ── Waypoint helpers ─────────────────────────────────────────────────────────

static const std::vector<LaneWaypoint>& pickWaypoints(const Lane& lane, int teamId) {
    return (teamId == 0) ? lane.blueWaypoints : lane.redWaypoints;
}

int Map::getNearestWaypointIndex(int laneIndex, int teamId, const Vec2& pos) const {
    const auto& wps = pickWaypoints(lanes[laneIndex], teamId);
    int best = 0;
    float bestDist = std::numeric_limits<float>::max();
    for (int i = 0; i < static_cast<int>(wps.size()); ++i) {
        float d = pos.distance(wps[i].position);
        if (d < bestDist) { bestDist = d; best = i; }
    }
    return best;
}

Vec2 Map::getNextWaypoint(int laneIndex, int teamId, int currentIndex) const {
    const auto& wps = pickWaypoints(lanes[laneIndex], teamId);
    int next = std::min(currentIndex + 1, static_cast<int>(wps.size()) - 1);
    return wps[next].position;
}

bool Map::isLastWaypoint(int laneIndex, int teamId, int currentIndex) const {
    const auto& wps = pickWaypoints(lanes[laneIndex], teamId);
    return currentIndex >= static_cast<int>(wps.size()) - 1;
}

bool Map::isObstacle(const Vec2& pos, float radius) const {
    for (const auto& obs : obstacles) {
        if (pos.distance(obs.position) < radius + obs.radius)
            return true;
    }
    return false;
}

// ── Rendering ────────────────────────────────────────────────────────────────

void Map::render(Renderer& renderer, float camX, float camY) {
    const float W = static_cast<float>(MAP_WIDTH);
    const float H = static_cast<float>(MAP_HEIGHT);

    // ── Background (dark jungle) ──────────────────────────────────────────────
    renderer.setColor(22, 38, 28);
    renderer.drawWorldRect(0, 0, W, H, camX, camY);

    // ── River (diagonal teal-blue band, ~240 units wide) ─────────────────────
    renderer.setColor(28, 52, 75, 210);
    for (int i = -15; i <= 15; ++i) {
        float off = static_cast<float>(i) * 8.0f;
        renderer.drawWorldLine(off, H + off, W + off, off, camX, camY);
    }

    // ── Lane paths (muted stone / sand) ──────────────────────────────────────
    renderer.setColor(88, 80, 62);
    // Top lane – vertical left segment
    renderer.drawWorldRect(150.0f, 150.0f, 90.0f, H - 300.0f, camX, camY);
    // Top lane – horizontal top segment
    renderer.drawWorldRect(150.0f, 150.0f, W - 300.0f, 90.0f, camX, camY);
    // Bot lane – horizontal bottom segment
    renderer.drawWorldRect(150.0f, H - 240.0f, W - 300.0f, 90.0f, camX, camY);
    // Bot lane – vertical right segment
    renderer.drawWorldRect(W - 240.0f, 150.0f, 90.0f, H - 300.0f, camX, camY);

    // Mid lane (diagonal, drawn as a band of parallel lines)
    renderer.setColor(88, 80, 62);
    for (int i = 0; i < 25; ++i) {
        float t  = static_cast<float>(i) / 25.0f;
        float t2 = static_cast<float>(i + 1) / 25.0f;
        float x1 = t  * W, y1 = H - t  * H;
        float x2 = t2 * W, y2 = H - t2 * H;
        for (int k = -4; k <= 4; ++k) {
            float d = static_cast<float>(k) * 6.0f;
            renderer.drawWorldLine(x1 + d, y1 - d, x2 + d, y2 - d, camX, camY);
        }
    }

    // ── Blue base zone (bottom-left, subtle highlight) ────────────────────────
    renderer.setColor(18, 28, 50, 160);
    renderer.drawWorldRect(0.0f, H - 600.0f, 600.0f, 600.0f, camX, camY);

    // ── Red base zone (top-right, subtle highlight) ───────────────────────────
    renderer.setColor(50, 18, 18, 160);
    renderer.drawWorldRect(W - 600.0f, 0.0f, 600.0f, 600.0f, camX, camY);

    // ── Nexus circles (triple ring) ───────────────────────────────────────────
    renderer.setColor(60, 130, 210);
    renderer.drawWorldCircle(blueNexus.x, blueNexus.y, 70.0f, camX, camY);
    renderer.drawWorldCircle(blueNexus.x, blueNexus.y, 60.0f, camX, camY);
    renderer.drawWorldCircle(blueNexus.x, blueNexus.y, 50.0f, camX, camY);
    renderer.setColor(210, 60, 60);
    renderer.drawWorldCircle(redNexus.x,  redNexus.y,  70.0f, camX, camY);
    renderer.drawWorldCircle(redNexus.x,  redNexus.y,  60.0f, camX, camY);
    renderer.drawWorldCircle(redNexus.x,  redNexus.y,  50.0f, camX, camY);

    // ── Map border ────────────────────────────────────────────────────────────
    renderer.setColor(40, 28, 15);
    renderer.drawWorldRect(0, 0, W, H, camX, camY, false);
}
