#pragma once
#include "Vec2.h"
#include <vector>
#include <array>

constexpr int MAP_WIDTH  = 3000;
constexpr int MAP_HEIGHT = 3000;

struct LaneWaypoint {
    Vec2 position;
};

struct Lane {
    std::vector<LaneWaypoint> blueWaypoints; // blue team advances along this list
    std::vector<LaneWaypoint> redWaypoints;  // red team advances along this list (reverse order)
};

struct MapObstacle {
    Vec2 position;
    float radius;
};

class Renderer; // forward declaration

class Map {
public:
    void init();
    void render(Renderer& renderer, float camX, float camY);

    int  getNearestWaypointIndex(int laneIndex, int teamId, const Vec2& pos) const;
    Vec2 getNextWaypoint(int laneIndex, int teamId, int currentIndex) const;
    bool isLastWaypoint(int laneIndex, int teamId, int currentIndex) const;

    const std::array<Lane, 3>& getLanes() const { return lanes; }
    Vec2 getBlueNexus() const { return blueNexus; }
    Vec2 getRedNexus()  const { return redNexus;  }

    bool isObstacle(const Vec2& pos, float radius) const;

private:
    std::array<Lane, 3> lanes; // 0=top, 1=mid, 2=bot
    std::vector<MapObstacle> obstacles;
    Vec2 blueNexus{200.0f, 2800.0f};
    Vec2 redNexus {2800.0f, 200.0f};

    void initLanes();
    void initObstacles();
};
