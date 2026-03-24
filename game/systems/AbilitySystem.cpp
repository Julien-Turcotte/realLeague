#include "systems/AbilitySystem.h"
#include "entities/EntityFactory.h"
#include <algorithm>
#include <vector>

void AbilitySystem::update(World& world, float dt,
                           const InputManager& input, float camX, float camY) {
    // Tick cooldowns for all champions
    for (auto& [id, champ] : world.champions) {
        for (auto& cd : champ.abilityCooldowns)
            if (cd > 0.0f) cd = std::max(0.0f, cd - dt);
    }

    // Tick VFX lifetimes and queue expired ones for destruction
    std::vector<EntityID> vfxToDestroy;
    for (auto& [id, vfx] : world.vfxComponents) {
        vfx.remaining -= dt;
        if (vfx.remaining <= 0.0f)
            vfxToDestroy.push_back(id);
    }
    for (EntityID id : vfxToDestroy)
        world.destroyEntity(id);

    // Player input
    EntityID player = world.playerEntity;
    if (player == INVALID_ENTITY) return;
    if (world.champions.count(player) == 0) return;
    if (world.healths.count(player) && world.healths[player].isDead) return;

    Vec2 mouseWorld{input.getMousePosition().x + camX,
                    input.getMousePosition().y + camY};

    struct KeySlot { SDL_Scancode sc; int slot; };
    constexpr KeySlot bindings[] = {
        {SDL_SCANCODE_Q, 0},
        {SDL_SCANCODE_W, 1},
        {SDL_SCANCODE_E, 2},
        {SDL_SCANCODE_R, 3},
    };

    for (const auto& b : bindings) {
        if (input.isKeyJustPressed(b.sc)) {
            auto& champ = world.champions[player];
            if (champ.abilityCooldowns[b.slot] <= 0.0f)
                fireAbility(world, b.slot, player, mouseWorld);
        }
    }
}

void AbilitySystem::fireAbility(World& world, int slot, EntityID champion, const Vec2& targetPos) {
    if (world.champions.count(champion) == 0) return;
    auto& champ = world.champions[champion];

    // Set cooldown
    champ.abilityCooldowns[slot] = champ.abilityMaxCooldowns[slot];

    if (slot == 0) {
        // Q – Fireball skillshot toward mouse cursor
        if (world.transforms.count(champion) == 0) return;
        Vec2 origin = world.transforms[champion].position;
        Vec2 dir    = targetPos - origin;
        int team    = world.teamComponents.count(champion) ? world.teamComponents[champion].teamId : 0;
        EntityFactory::createFireball(world, origin, dir, 80.0f, team, champion);
        // Small launch flash at cast origin
        EntityFactory::createVfxFlash(world, origin, 25.0f, 0.2f, 255, 220, 0);
    } else if (slot == 1) {
        // W – Healing Pulse: restore 150 HP and emit green rings
        if (world.healths.count(champion)) {
            auto& hp = world.healths[champion];
            hp.current = std::min(hp.max, hp.current + 150.0f);
        }
        if (world.transforms.count(champion)) {
            Vec2 pos = world.transforms[champion].position;
            EntityFactory::createVfxFlash(world, pos, 30.0f,  0.25f, 80,  255, 120);
            EntityFactory::createVfxRing (world, pos, 60.0f,  0.40f, 80,  255, 120);
            EntityFactory::createVfxRing (world, pos, 100.0f, 0.60f, 50,  200, 80);
            EntityFactory::createVfxRing (world, pos, 140.0f, 0.80f, 30,  160, 60);
        }
    } else if (slot == 2) {
        // E – Dash up to 300 units toward mouse cursor
        if (world.transforms.count(champion)) {
            Vec2& pos    = world.transforms[champion].position;
            Vec2  origin = pos;
            Vec2  dir    = targetPos - pos;
            float dist   = dir.length();
            if (dist > 1.0f) {
                float dashDist = std::min(dist, 300.0f);
                pos = origin + dir.normalized() * dashDist;
                // Cancel move target so champion doesn't slide after blinking
                if (world.playerControlled.count(champion)) {
                    world.playerControlled[champion].hasTarget = false;
                }
            }
            // Ghost trail at departure
            EntityFactory::createVfxFlash(world, origin, 30.0f, 0.35f, 100, 200, 255);
            EntityFactory::createVfxRing (world, origin, 50.0f, 0.45f, 100, 200, 255);
            // Landing indicator
            EntityFactory::createVfxFlash(world, pos, 40.0f, 0.30f, 180, 240, 255);
            EntityFactory::createVfxRing (world, pos, 55.0f, 0.50f, 150, 220, 255);
        }
    } else if (slot == 3) {
        // R – Area Explosion: deal 200 damage to all enemies within 300 units
        if (world.transforms.count(champion)) {
            Vec2  pos       = world.transforms[champion].position;
            float aoeRadius = 300.0f;
            int   team      = world.teamComponents.count(champion)
                              ? world.teamComponents[champion].teamId : 0;

            for (EntityID eid : world.entities) {
                if (eid == champion) continue;
                if (!world.healths.count(eid)) continue;
                if (world.healths[eid].isDead) continue;
                if (!world.transforms.count(eid)) continue;
                if (world.teamComponents.count(eid) &&
                    world.teamComponents[eid].teamId == team) continue;

                if (pos.distance(world.transforms[eid].position) <= aoeRadius) {
                    auto& hp  = world.healths[eid];
                    float dmg = 200.0f - hp.armor * 0.1f;
                    if (dmg < 1.0f) dmg = 1.0f;
                    hp.current -= dmg;
                    if (hp.current <= 0.0f) { hp.current = 0.0f; hp.isDead = true; }
                }
            }

            // VFX: central flash then three expanding rings
            EntityFactory::createVfxFlash(world, pos, aoeRadius * 0.40f, 0.30f, 255, 230, 80);
            EntityFactory::createVfxRing (world, pos, aoeRadius * 0.50f, 0.40f, 255, 180, 0);
            EntityFactory::createVfxRing (world, pos, aoeRadius * 0.75f, 0.60f, 255, 120, 0);
            EntityFactory::createVfxRing (world, pos, aoeRadius,         0.80f, 220,  60, 0);
        }
    }
}
