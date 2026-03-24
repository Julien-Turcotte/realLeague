#include "entities/EntityFactory.h"

EntityID EntityFactory::createPlayerChampion(World& world, const Vec2& pos) {
    EntityID id = world.createEntity();
    world.transforms[id]       = {pos, 0.0f};
    world.velocities[id]       = {};
    world.healths[id]          = {500.0f, 500.0f, 10.0f, false};
    world.attacks[id]          = {40.0f, 175.0f, 1.0f, 0.0f, INVALID_ENTITY};
    world.renderables[id]      = {50, 100, 255, 24.0f, 24.0f};
    world.teamComponents[id]   = {0};
    world.collisions[id]       = {12.0f, false};
    world.playerControlled[id] = {};
    ChampionComponent champ;
    champ.spawnPosition = pos;
    world.champions[id] = champ;
    return id;
}

EntityID EntityFactory::createBotChampion(World& world, const Vec2& pos) {
    EntityID id = world.createEntity();
    world.transforms[id]     = {pos, 0.0f};
    world.velocities[id]     = {};
    world.healths[id]        = {500.0f, 500.0f, 10.0f, false};
    world.attacks[id]        = {40.0f, 175.0f, 1.0f, 0.0f, INVALID_ENTITY};
    world.renderables[id]    = {255, 50, 50, 24.0f, 24.0f};
    world.teamComponents[id] = {1};
    world.collisions[id]     = {12.0f, false};
    ChampionComponent champ;
    champ.spawnPosition = pos;
    world.champions[id]  = champ;
    AIComponent ai;
    ai.aggroRange   = 400.0f;
    ai.retreatHP    = 125.0f; // 25 % of 500
    ai.laneIndex    = 1;      // mid lane
    ai.state        = AIComponent::State::Moving;
    ai.laneTarget   = {1500.0f, 1500.0f};
    world.aiComponents[id] = ai;
    return id;
}

EntityID EntityFactory::createMinion(World& world, int teamId, int laneIndex, const Vec2& pos) {
    EntityID id = world.createEntity();
    world.transforms[id]     = {pos, 0.0f};
    world.velocities[id]     = {};
    world.healths[id]        = {100.0f, 100.0f, 5.0f, false};
    world.attacks[id]        = {15.0f, 120.0f, 0.8f, 0.0f, INVALID_ENTITY};
    if (teamId == 0)
        world.renderables[id] = {100, 150, 255, 20.0f, 20.0f};
    else
        world.renderables[id] = {255, 100, 100, 20.0f, 20.0f};
    world.teamComponents[id] = {teamId};
    world.collisions[id]     = {10.0f, false};
    MinionComponent mc;
    mc.laneIndex = laneIndex;
    mc.bounty    = 25.0f;
    world.minions[id] = mc;
    AIComponent ai;
    ai.aggroRange = 300.0f;
    ai.retreatHP  = 20.0f;
    ai.laneIndex  = laneIndex;
    ai.state      = AIComponent::State::Moving;
    world.aiComponents[id] = ai;
    return id;
}

EntityID EntityFactory::createTower(World& world, int teamId, const Vec2& pos) {
    EntityID id = world.createEntity();
    world.transforms[id]     = {pos, 0.0f};
    world.healths[id]        = {1500.0f, 1500.0f, 20.0f, false};
    if (teamId == 0)
        world.renderables[id] = {0, 0, 200, 50.0f, 80.0f};
    else
        world.renderables[id] = {200, 0, 0, 50.0f, 80.0f};
    world.teamComponents[id] = {teamId};
    world.collisions[id]     = {25.0f, true};
    world.towers[id]         = {};
    return id;
}

EntityID EntityFactory::createFireball(World& world, const Vec2& pos, const Vec2& dir,
                                        float damage, int ownerTeam, EntityID owner,
                                        EntityID targetEntity) {
    EntityID id = world.createEntity();
    world.transforms[id]  = {pos, 0.0f};
    world.renderables[id] = {255, 220, 0, 12.0f, 12.0f};
    world.collisions[id]  = {6.0f, false};
    ProjectileComponent proj;
    proj.direction    = dir.normalized();
    proj.speed        = 400.0f;
    proj.damage       = damage;
    proj.ownerTeam    = ownerTeam;
    proj.owner        = owner;
    proj.lifetime     = 3.0f;
    proj.targetEntity = targetEntity;
    world.projectiles[id] = proj;
    return id;
}

EntityID EntityFactory::createVfxRing(World& world, const Vec2& pos, float maxRadius,
                                       float duration, int r, int g, int b) {
    EntityID id = world.createEntity();
    VfxComponent vfx;
    vfx.type       = VfxComponent::Type::ExpandingRing;
    vfx.position   = pos;
    vfx.maxRadius  = maxRadius;
    vfx.duration   = duration;
    vfx.remaining  = duration;
    vfx.colorR     = r;
    vfx.colorG     = g;
    vfx.colorB     = b;
    world.vfxComponents[id] = vfx;
    return id;
}

EntityID EntityFactory::createVfxFlash(World& world, const Vec2& pos, float radius,
                                        float duration, int r, int g, int b) {
    EntityID id = world.createEntity();
    VfxComponent vfx;
    vfx.type       = VfxComponent::Type::SolidFlash;
    vfx.position   = pos;
    vfx.maxRadius  = radius;
    vfx.duration   = duration;
    vfx.remaining  = duration;
    vfx.colorR     = r;
    vfx.colorG     = g;
    vfx.colorB     = b;
    world.vfxComponents[id] = vfx;
    return id;
}
