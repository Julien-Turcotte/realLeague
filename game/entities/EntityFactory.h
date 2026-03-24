#pragma once
#include "ecs/ECS.h"
#include "map/Map.h"

class EntityFactory {
public:
    static EntityID createPlayerChampion(World& world, const Vec2& pos);
    static EntityID createBotChampion(World& world, const Vec2& pos);
    static EntityID createMinion(World& world, int teamId, int laneIndex, const Vec2& pos);
    static EntityID createTower(World& world, int teamId, const Vec2& pos);
    static EntityID createFireball(World& world, const Vec2& pos, const Vec2& dir,
                                   float damage, int ownerTeam, EntityID owner,
                                   EntityID targetEntity = INVALID_ENTITY);

    // Visual effects
    static EntityID createVfxRing(World& world, const Vec2& pos, float maxRadius,
                                   float duration, int r, int g, int b);
    static EntityID createVfxFlash(World& world, const Vec2& pos, float radius,
                                    float duration, int r, int g, int b);
};
