#include "systems/CombatSystem.h"
#include "entities/EntityFactory.h"
#include <algorithm>
#include <cmath>

// ── Helpers ──────────────────────────────────────────────────────────────────

void CombatSystem::dealDamage(World& world, EntityID target, float rawDamage) {
    if (world.healths.count(target) == 0) return;
    auto& hp = world.healths[target];
    if (hp.isDead) return;
    float armor  = hp.armor;
    float actual = rawDamage - armor * 0.1f;
    if (actual < 1.0f) actual = 1.0f;
    hp.current -= actual;
    if (hp.current <= 0.0f) {
        hp.current = 0.0f;
        hp.isDead  = true;
    }
}

EntityID CombatSystem::findNearestEnemy(World& world, EntityID attacker, float range) const {
    if (world.transforms.count(attacker) == 0) return INVALID_ENTITY;
    if (world.teamComponents.count(attacker) == 0) return INVALID_ENTITY;

    int myTeam = world.teamComponents[attacker].teamId;
    Vec2 myPos = world.transforms[attacker].position;
    EntityID best = INVALID_ENTITY;
    float bestDist = range + 1.0f;

    for (EntityID eid : world.entities) {
        if (eid == attacker) continue;
        if (world.teamComponents.count(eid) == 0) continue;
        if (world.teamComponents[eid].teamId == myTeam) continue;
        if (world.healths.count(eid) && world.healths[eid].isDead) continue;
        if (world.transforms.count(eid) == 0) continue;
        float d = myPos.distance(world.transforms[eid].position);
        if (d < bestDist) { bestDist = d; best = eid; }
    }
    return best;
}

// ── Tower attacks ─────────────────────────────────────────────────────────────

void CombatSystem::processTowers(World& world, float dt) {
    for (auto& [id, tower] : world.towers) {
        if (world.healths.count(id) && world.healths[id].isDead) continue;
        if (world.transforms.count(id) == 0) continue;
        if (world.teamComponents.count(id) == 0) continue;

        tower.attackTimer -= dt;

        // Validate current target
        if (tower.currentTarget != INVALID_ENTITY) {
            bool invalid = false;
            if (world.healths.count(tower.currentTarget) == 0 ||
                world.healths[tower.currentTarget].isDead)
                invalid = true;
            else if (world.transforms.count(tower.currentTarget)) {
                float d = world.transforms[id].position.distance(
                              world.transforms[tower.currentTarget].position);
                if (d > tower.range) invalid = true;
            }
            if (invalid) tower.currentTarget = INVALID_ENTITY;
        }

        if (tower.currentTarget == INVALID_ENTITY)
            tower.currentTarget = findNearestEnemy(world, id, tower.range);

        if (tower.currentTarget != INVALID_ENTITY && tower.attackTimer <= 0.0f) {
            dealDamage(world, tower.currentTarget, tower.damage);
            tower.attackTimer = 1.0f / tower.attackSpeed;
        }
    }
}

// ── Regular attackers ─────────────────────────────────────────────────────────

void CombatSystem::processAttackers(World& world, float dt) {
    for (auto& [id, atk] : world.attacks) {
        if (world.healths.count(id) && world.healths[id].isDead) continue;
        if (world.transforms.count(id) == 0) continue;
        if (world.teamComponents.count(id) == 0) continue;

        atk.attackTimer -= dt;

        // If the player set an explicit attack target, honour it
        bool hasExplicitTarget = false;
        if (world.playerControlled.count(id)) {
            auto& pc = world.playerControlled[id];
            if (pc.attackTarget != INVALID_ENTITY) {
                EntityID tgt = pc.attackTarget;
                if (!world.transforms.count(tgt) ||
                    (world.healths.count(tgt) && world.healths[tgt].isDead)) {
                    // Target died: clear it so movement/combat fall back to normal
                    pc.attackTarget    = INVALID_ENTITY;
                    atk.currentTarget = INVALID_ENTITY;
                } else {
                    atk.currentTarget = tgt;
                    hasExplicitTarget  = true;
                }
            }
        }

        if (!hasExplicitTarget) {
            // Validate current auto-acquired target
            if (atk.currentTarget != INVALID_ENTITY) {
                bool invalid = false;
                if (world.healths.count(atk.currentTarget) == 0 ||
                    world.healths[atk.currentTarget].isDead)
                    invalid = true;
                else if (world.transforms.count(atk.currentTarget)) {
                    float d = world.transforms[id].position.distance(
                                  world.transforms[atk.currentTarget].position);
                    if (d > atk.range * 1.2f) invalid = true; // 20 % leash prevents flickering near edge of range
                }
                if (invalid) atk.currentTarget = INVALID_ENTITY;
            }

            if (atk.currentTarget == INVALID_ENTITY)
                atk.currentTarget = findNearestEnemy(world, id, atk.range);
        }

        if (atk.currentTarget != INVALID_ENTITY && atk.attackTimer <= 0.0f) {
            // Only fire if the target is within attack range
            if (world.transforms.count(atk.currentTarget) &&
                world.teamComponents.count(id)) {
                float d = world.transforms[id].position.distance(
                              world.transforms[atk.currentTarget].position);
                if (d <= atk.range) {
                    // Spawn a homing projectile toward the target
                    Vec2 origin    = world.transforms[id].position;
                    Vec2 targetPos = world.transforms[atk.currentTarget].position;
                    Vec2 dir       = (targetPos - origin).normalized();
                    int  team      = world.teamComponents[id].teamId;
                    EntityFactory::createFireball(world, origin, dir,
                                                  atk.damage, team, id,
                                                  atk.currentTarget);
                    atk.attackTimer = 1.0f / atk.attackSpeed;
                }
            }
        }
    }
}

void CombatSystem::update(World& world, float dt) {
    processTowers(world, dt);
    processAttackers(world, dt);
}
