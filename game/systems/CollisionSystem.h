#pragma once
#include "ecs/ECS.h"

class CollisionSystem {
public:
    void update(World& world, float dt);

private:
    bool circlesOverlap(const Vec2& a, float ra, const Vec2& b, float rb) const;
};
