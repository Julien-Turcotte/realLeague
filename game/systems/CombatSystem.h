#pragma once
#include "ecs/ECS.h"

class CombatSystem {
public:
    void update(World& world, float dt);

private:
    void processTowers(World& world, float dt);
    void processAttackers(World& world, float dt);
    EntityID findNearestEnemy(World& world, EntityID attacker, float range) const;
    void dealDamage(World& world, EntityID target, float rawDamage);
};
