#include "systems/RespawnSystem.h"

void RespawnSystem::update(World& world, float dt) {
    for (EntityID id : world.entities) {
        // Only champions have a respawn cycle
        if (world.champions.count(id) == 0) continue;
        if (world.healths.count(id) == 0)   continue;

        auto& hp = world.healths[id];

        if (!hp.isDead) continue;

        if (world.respawnComponents.count(id) == 0) {
            // Champion just died – create a respawn entry with an increasing timer
            auto& champ = world.champions[id];

            float respawnTime = BASE_RESPAWN_TIME
                                + static_cast<float>(champ.deathCount) * TIME_PER_DEATH;
            champ.deathCount++;

            RespawnComponent rc;
            rc.timer        = respawnTime;
            rc.maxTimer     = respawnTime;
            rc.spawnPosition = champ.spawnPosition;
            world.respawnComponents[id] = rc;

            // Freeze movement
            if (world.velocities.count(id)) {
                world.velocities[id].hasTarget = false;
                world.velocities[id].velocity  = {0.0f, 0.0f};
            }

            // Reset AI so the bot doesn't try to path immediately after respawn
            if (world.aiComponents.count(id)) {
                world.aiComponents[id].state        = AIComponent::State::Idle;
                world.aiComponents[id].targetEntity = INVALID_ENTITY;
            }
        } else {
            // Count down the timer
            auto& rc = world.respawnComponents[id];
            rc.timer -= dt;

            if (rc.timer <= 0.0f) {
                // Restore champion
                hp.current = hp.max;
                hp.isDead  = false;

                // Teleport to spawn point
                if (world.transforms.count(id))
                    world.transforms[id].position = rc.spawnPosition;

                // Reset velocity
                if (world.velocities.count(id)) {
                    world.velocities[id].hasTarget = false;
                    world.velocities[id].velocity  = {0.0f, 0.0f};
                }

                // Resume AI
                if (world.aiComponents.count(id)) {
                    world.aiComponents[id].state        = AIComponent::State::Moving;
                    world.aiComponents[id].targetEntity = INVALID_ENTITY;
                }

                // Clear stale combat targets
                if (world.attacks.count(id))
                    world.attacks[id].currentTarget = INVALID_ENTITY;

                if (world.playerControlled.count(id)) {
                    world.playerControlled[id].attackTarget = INVALID_ENTITY;
                    world.playerControlled[id].hasTarget    = false;
                }

                world.respawnComponents.erase(id);
            }
        }
    }
}
