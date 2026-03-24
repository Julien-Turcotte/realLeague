#include "systems/MovementSystem.h"
#include "map/Map.h"
#include <algorithm>

void MovementSystem::update(World& world, float dt) {
    // Sync PlayerControlled move targets → Velocity
    for (auto& [id, pc] : world.playerControlled) {
        if (world.velocities.count(id) == 0) continue;
        auto& vel = world.velocities[id];
        if (pc.hasTarget) {
            vel.hasTarget = true;
            vel.target    = pc.moveTarget;
        }
    }

    // Chase explicit attack targets: move toward enemy until within attack range
    for (auto& [id, pc] : world.playerControlled) {
        if (pc.attackTarget == INVALID_ENTITY) continue;
        if (world.velocities.count(id) == 0) continue;
        if (world.transforms.count(id) == 0) continue;
        if (world.attacks.count(id) == 0) continue;

        EntityID tgt = pc.attackTarget;

        // Invalidate target if it died or disappeared
        if (!world.transforms.count(tgt) ||
            (world.healths.count(tgt) && world.healths[tgt].isDead)) {
            pc.attackTarget = INVALID_ENTITY;
            continue;
        }

        const Vec2& myPos  = world.transforms[id].position;
        const Vec2& tgtPos = world.transforms[tgt].position;
        float dist  = myPos.distance(tgtPos);
        float range = world.attacks[id].range;

        auto& vel = world.velocities[id];
        if (dist > range) {
            // Move toward the enemy
            vel.hasTarget = true;
            vel.target    = tgtPos;
        } else {
            // In range: stop moving so the combat system can attack
            vel.hasTarget = false;
        }
    }

    // Move projectiles (they use velocity.velocity directly)
    for (auto& [id, proj] : world.projectiles) {
        if (world.transforms.count(id) == 0) continue;
        auto& tr = world.transforms[id];

        // Steer homing projectile toward its target
        if (proj.targetEntity != INVALID_ENTITY &&
            world.transforms.count(proj.targetEntity) &&
            world.healths.count(proj.targetEntity) &&
            !world.healths[proj.targetEntity].isDead)
        {
            Vec2 toTarget = world.transforms[proj.targetEntity].position - tr.position;
            proj.direction = toTarget.normalized();
        }

        tr.position += proj.direction * (proj.speed * dt);

        // Tick lifetime
        proj.lifetime -= dt;
        if (proj.lifetime <= 0.0f)
            world.destroyEntity(id);
    }

    // Move all entities with Transform + Velocity
    for (auto& [id, vel] : world.velocities) {
        if (world.transforms.count(id) == 0) continue;
        // Skip dead entities
        if (world.healths.count(id) && world.healths[id].isDead) continue;

        auto& tr = world.transforms[id];

        if (vel.hasTarget) {
            Vec2 diff = vel.target - tr.position;
            float dist = diff.length();
            if (dist < 5.0f) {
                vel.hasTarget  = false;
                vel.velocity   = {0, 0};
                tr.position    = vel.target;
                // Clear player move target too
                if (world.playerControlled.count(id))
                    world.playerControlled[id].hasTarget = false;
            } else {
                Vec2 dir = diff / dist;
                tr.position += dir * (vel.speed * dt);
            }
        } else {
            tr.position += vel.velocity * dt;
        }

        // Clamp to map bounds
        tr.position.x = std::clamp(tr.position.x, 0.0f, static_cast<float>(MAP_WIDTH));
        tr.position.y = std::clamp(tr.position.y, 0.0f, static_cast<float>(MAP_HEIGHT));
    }
}
