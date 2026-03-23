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
    // Background (grass)
    renderer.setColor(40, 80, 40);
    renderer.drawWorldRect(0, 0, static_cast<float>(MAP_WIDTH), static_cast<float>(MAP_HEIGHT), camX, camY);

    // River band (diagonal blue-gray strip)
    renderer.setColor(60, 80, 120, 180);
    for (int i = -30; i <= 30; ++i) {
        float offset = static_cast<float>(i) * 5.0f;
        renderer.drawWorldLine(offset, static_cast<float>(MAP_HEIGHT) + offset,
                               static_cast<float>(MAP_WIDTH) + offset, offset,
                               camX, camY);
    }

    // Lane paths (gray rectangles approximated by thick lines)
    renderer.setColor(120, 110, 90);
    // Top lane (vertical then horizontal)
    renderer.drawWorldRect(160.0f, 160.0f, 80.0f, static_cast<float>(MAP_HEIGHT) - 320.0f, camX, camY);
    renderer.drawWorldRect(160.0f, 160.0f, static_cast<float>(MAP_WIDTH) - 320.0f, 80.0f, camX, camY);
    // Bot lane
    renderer.drawWorldRect(160.0f, static_cast<float>(MAP_HEIGHT) - 240.0f,
                           static_cast<float>(MAP_WIDTH) - 320.0f, 80.0f, camX, camY);
    renderer.drawWorldRect(static_cast<float>(MAP_WIDTH) - 240.0f, 160.0f,
                           80.0f, static_cast<float>(MAP_HEIGHT) - 320.0f, camX, camY);
    // Mid lane (diagonal – drawn as a series of small rects)
    for (int i = 0; i < 20; ++i) {
        float t  = static_cast<float>(i) / 20.0f;
        float t2 = static_cast<float>(i + 1) / 20.0f;
        float x1 = t  * static_cast<float>(MAP_WIDTH);
        float y1 = static_cast<float>(MAP_HEIGHT) - t  * static_cast<float>(MAP_HEIGHT);
        float x2 = t2 * static_cast<float>(MAP_WIDTH);
        float y2 = static_cast<float>(MAP_HEIGHT) - t2 * static_cast<float>(MAP_HEIGHT);
        renderer.setColor(120, 110, 90);
        renderer.drawWorldLine(x1, y1, x2, y2, camX, camY);
    }

    // Nexus circles
    renderer.setColor(100, 150, 255);
    renderer.drawWorldCircle(blueNexus.x, blueNexus.y, 60.0f, camX, camY);
    renderer.setColor(255, 100, 100);
    renderer.drawWorldCircle(redNexus.x, redNexus.y, 60.0f, camX, camY);

    // Map border
    renderer.setColor(80, 60, 30);
    renderer.drawWorldRect(0, 0, static_cast<float>(MAP_WIDTH), static_cast<float>(MAP_HEIGHT), camX, camY, false);
}
