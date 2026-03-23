#include "systems/AISystem.h"
#include <algorithm>

// ── Helpers ──────────────────────────────────────────────────────────────────

EntityID AISystem::findNearestEnemy(World& world, EntityID self, float range) const {
    if (world.transforms.count(self) == 0) return INVALID_ENTITY;
    if (world.teamComponents.count(self) == 0) return INVALID_ENTITY;

    int myTeam = world.teamComponents[self].teamId;
    Vec2 myPos = world.transforms[self].position;
    EntityID best = INVALID_ENTITY;
    float bestDist = range + 1.0f;

    for (EntityID eid : world.entities) {
        if (eid == self) continue;
        if (world.teamComponents.count(eid) == 0) continue;
        if (world.teamComponents[eid].teamId == myTeam) continue;
        if (world.healths.count(eid) && world.healths[eid].isDead) continue;
        if (world.transforms.count(eid) == 0) continue;
        float d = myPos.distance(world.transforms[eid].position);
        if (d < bestDist) { bestDist = d; best = eid; }
    }
    return best;
}

// ── Minion update ─────────────────────────────────────────────────────────────

void AISystem::updateMinion(World& world, EntityID id, float /*dt*/, const Map& map) {
    if (world.transforms.count(id) == 0) return;
    auto& ai  = world.aiComponents[id];
    auto& vel = world.velocities[id];
    auto& hp  = world.healths[id];
    int team  = world.teamComponents[id].teamId;
    Vec2 pos  = world.transforms[id].position;

    switch (ai.state) {
        case AIComponent::State::Idle: {
            EntityID enemy = findNearestEnemy(world, id, ai.aggroRange);
            if (enemy != INVALID_ENTITY) {
                ai.state        = AIComponent::State::Attacking;
                ai.targetEntity = enemy;
                if (world.attacks.count(id))
                    world.attacks[id].currentTarget = enemy;
            } else {
                ai.state = AIComponent::State::Moving;
            }
            break;
        }
        case AIComponent::State::Attacking: {
            // Check if target is still valid
            bool targetValid = (ai.targetEntity != INVALID_ENTITY) &&
                               world.healths.count(ai.targetEntity) &&
                               !world.healths[ai.targetEntity].isDead;
            if (!targetValid) {
                ai.targetEntity = INVALID_ENTITY;
                if (world.attacks.count(id)) world.attacks[id].currentTarget = INVALID_ENTITY;
                ai.state = AIComponent::State::Moving;
                break;
            }
            // Move toward target if not in attack range
            float atkRange = world.attacks.count(id) ? world.attacks[id].range : 120.0f;
            float d = pos.distance(world.transforms[ai.targetEntity].position);
            if (d > atkRange) {
                vel.hasTarget = true;
                vel.target    = world.transforms[ai.targetEntity].position;
            } else {
                vel.hasTarget = false;
            }
            // Check retreat
            if (hp.current < ai.retreatHP)
                ai.state = AIComponent::State::Retreating;
            break;
        }
        case AIComponent::State::Moving: {
            // Check for enemies to aggro
            EntityID enemy = findNearestEnemy(world, id, ai.aggroRange);
            if (enemy != INVALID_ENTITY) {
                ai.state        = AIComponent::State::Attacking;
                ai.targetEntity = enemy;
                if (world.attacks.count(id))
                    world.attacks[id].currentTarget = enemy;
                break;
            }
            // Advance along lane
            int wpIdx = map.getNearestWaypointIndex(ai.laneIndex, team, pos);
            if (!map.isLastWaypoint(ai.laneIndex, team, wpIdx)) {
                Vec2 nextWP = map.getNextWaypoint(ai.laneIndex, team, wpIdx);
                vel.hasTarget = true;
                vel.target    = nextWP;
            } else {
                vel.hasTarget = false;
            }
            break;
        }
        case AIComponent::State::Retreating: {
            // Move back toward own nexus
            Vec2 nexus = (team == 0) ? map.getBlueNexus() : map.getRedNexus();
            vel.hasTarget = true;
            vel.target    = nexus;
            if (hp.current >= ai.retreatHP * 2.0f)
                ai.state = AIComponent::State::Moving;
            break;
        }
    }
}

// ── Bot champion update ───────────────────────────────────────────────────────

void AISystem::updateBotChampion(World& world, EntityID id, float /*dt*/, const Map& map) {
    if (world.transforms.count(id) == 0) return;
    auto& ai  = world.aiComponents[id];
    auto& vel = world.velocities[id];
    auto& hp  = world.healths[id];
    int team  = world.teamComponents[id].teamId;
    Vec2 pos  = world.transforms[id].position;

    // Retreat if HP below 25%
    float hpPct = hp.current / hp.max;
    if (hpPct < 0.25f && ai.state != AIComponent::State::Retreating) {
        ai.state = AIComponent::State::Retreating;
    }

    switch (ai.state) {
        case AIComponent::State::Retreating: {
            Vec2 nexus = (team == 0) ? map.getBlueNexus() : map.getRedNexus();
            vel.hasTarget = true;
            vel.target    = nexus;
            if (hpPct >= 0.6f)
                ai.state = AIComponent::State::Moving;
            break;
        }
        case AIComponent::State::Attacking: {
            bool targetValid = (ai.targetEntity != INVALID_ENTITY) &&
                               world.healths.count(ai.targetEntity) &&
                               !world.healths[ai.targetEntity].isDead;
            if (!targetValid) {
                ai.targetEntity = INVALID_ENTITY;
                if (world.attacks.count(id)) world.attacks[id].currentTarget = INVALID_ENTITY;
                ai.state = AIComponent::State::Moving;
                break;
            }
            float atkRange = world.attacks.count(id) ? world.attacks[id].range : 175.0f;
            float d = pos.distance(world.transforms[ai.targetEntity].position);
            if (d > atkRange) {
                vel.hasTarget = true;
                vel.target    = world.transforms[ai.targetEntity].position;
            } else {
                vel.hasTarget = false;
            }
            break;
        }
        default: {
            EntityID enemy = findNearestEnemy(world, id, ai.aggroRange);
            if (enemy != INVALID_ENTITY) {
                ai.state        = AIComponent::State::Attacking;
                ai.targetEntity = enemy;
                if (world.attacks.count(id))
                    world.attacks[id].currentTarget = enemy;
                break;
            }
            // Follow mid lane
            int wpIdx = map.getNearestWaypointIndex(ai.laneIndex, team, pos);
            if (!map.isLastWaypoint(ai.laneIndex, team, wpIdx)) {
                vel.hasTarget = true;
                vel.target    = map.getNextWaypoint(ai.laneIndex, team, wpIdx);
            } else {
                vel.hasTarget = false;
            }
            ai.state = AIComponent::State::Moving;
            break;
        }
    }
}

// ── Main update ───────────────────────────────────────────────────────────────

void AISystem::update(World& world, const Map& map, float dt) {
    for (EntityID id : world.entities) {
        if (world.aiComponents.count(id) == 0) continue;
        if (world.healths.count(id) && world.healths[id].isDead) continue;

        if (world.minions.count(id))
            updateMinion(world, id, dt, map);
        else
            updateBotChampion(world, id, dt, map);
    }
}
