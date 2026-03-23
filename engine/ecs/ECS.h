#pragma once
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "Components.h"

class World {
public:
    EntityID createEntity() {
        EntityID id = nextID++;
        entities.push_back(id);
        return id;
    }

    void destroyEntity(EntityID id) {
        toDestroy.push_back(id);
    }

    void flushDestroyQueue() {
        for (EntityID id : toDestroy) {
            entities.erase(std::remove(entities.begin(), entities.end(), id), entities.end());
            transforms.erase(id);
            velocities.erase(id);
            healths.erase(id);
            attacks.erase(id);
            abilities.erase(id);
            renderables.erase(id);
            aiComponents.erase(id);
            playerControlled.erase(id);
            teamComponents.erase(id);
            towers.erase(id);
            projectiles.erase(id);
            champions.erase(id);
            minions.erase(id);
            collisions.erase(id);
            respawnComponents.erase(id);
        }
        toDestroy.clear();
    }

    std::unordered_map<EntityID, Transform> transforms;
    std::unordered_map<EntityID, Velocity> velocities;
    std::unordered_map<EntityID, Health> healths;
    std::unordered_map<EntityID, Attack> attacks;
    std::unordered_map<EntityID, Ability> abilities;
    std::unordered_map<EntityID, Renderable> renderables;
    std::unordered_map<EntityID, AIComponent> aiComponents;
    std::unordered_map<EntityID, PlayerControlled> playerControlled;
    std::unordered_map<EntityID, TeamComponent> teamComponents;
    std::unordered_map<EntityID, TowerComponent> towers;
    std::unordered_map<EntityID, ProjectileComponent> projectiles;
    std::unordered_map<EntityID, ChampionComponent> champions;
    std::unordered_map<EntityID, MinionComponent> minions;
    std::unordered_map<EntityID, CollisionComponent> collisions;
    std::unordered_map<EntityID, RespawnComponent> respawnComponents;

    std::vector<EntityID> entities;
    std::vector<EntityID> toDestroy;
    EntityID nextID = 0;
    EntityID playerEntity = INVALID_ENTITY;
    EntityID hoveredEnemy = INVALID_ENTITY;
};
