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

    // Move projectiles (they use velocity.velocity directly)
    for (auto& [id, proj] : world.projectiles) {
        if (world.transforms.count(id) == 0) continue;
        auto& tr = world.transforms[id];
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
