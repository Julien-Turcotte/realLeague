#pragma once
#include "ecs/ECS.h"
#include "map/Map.h"

class AISystem {
public:
    void update(World& world, const Map& map, float dt);

private:
    void updateMinion(World& world, EntityID id, float dt, const Map& map);
    void updateBotChampion(World& world, EntityID id, float dt, const Map& map);
    EntityID findNearestEnemy(World& world, EntityID self, float range) const;
};
