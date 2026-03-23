#include "systems/CollisionSystem.h"
#include <cmath>

bool CollisionSystem::circlesOverlap(const Vec2& a, float ra,
                                      const Vec2& b, float rb) const {
    float distSq = (a - b).lengthSq();
    float radSum = ra + rb;
    return distSq < radSum * radSum;
}

void CollisionSystem::update(World& world, float /*dt*/) {
    // Projectile vs damageable entities
    std::vector<EntityID> projToDestroy;

    for (auto& [projId, proj] : world.projectiles) {
        if (world.transforms.count(projId) == 0) continue;
        const Vec2& projPos = world.transforms[projId].position;
        float projRad = world.collisions.count(projId) ? world.collisions[projId].radius : 6.0f;

        for (EntityID eid : world.entities) {
            if (eid == projId) continue;
            if (eid == proj.owner) continue;
            if (world.healths.count(eid) == 0) continue;
            if (world.healths[eid].isDead) continue;
            if (world.transforms.count(eid) == 0) continue;
            if (world.teamComponents.count(eid)) {
                if (world.teamComponents[eid].teamId == proj.ownerTeam) continue;
            }

            float entRad = world.collisions.count(eid) ? world.collisions[eid].radius : 15.0f;
            if (circlesOverlap(projPos, projRad, world.transforms[eid].position, entRad)) {
                // Deal damage
                auto& hp = world.healths[eid];
                float armor  = hp.armor;
                float actual = proj.damage - armor * 0.1f;
                if (actual < 1.0f) actual = 1.0f;
                hp.current -= actual;
                if (hp.current <= 0.0f) { hp.current = 0.0f; hp.isDead = true; }
                projToDestroy.push_back(projId);
                break;
            }
        }
    }

    for (EntityID id : projToDestroy)
        world.destroyEntity(id);

    // Simple separation for non-static solid entities
    for (std::size_t i = 0; i < world.entities.size(); ++i) {
        EntityID a = world.entities[i];
        if (world.collisions.count(a) == 0) continue;
        if (world.collisions[a].isStatic) continue;
        if (world.transforms.count(a) == 0) continue;
        if (world.projectiles.count(a)) continue;
        if (world.healths.count(a) && world.healths[a].isDead) continue;

        for (std::size_t j = i + 1; j < world.entities.size(); ++j) {
            EntityID b = world.entities[j];
            if (world.collisions.count(b) == 0) continue;
            if (world.transforms.count(b) == 0) continue;
            if (world.projectiles.count(b)) continue;
            if (world.healths.count(b) && world.healths[b].isDead) continue;

            float ra = world.collisions[a].radius;
            float rb = world.collisions[b].radius;
            Vec2& pa = world.transforms[a].position;
            Vec2& pb = world.transforms[b].position;

            if (!circlesOverlap(pa, ra, pb, rb)) continue;

            Vec2 diff = pa - pb;
            float dist = diff.length();
            if (dist < 1e-4f) { pa.x += 0.5f; pb.x -= 0.5f; continue; }

            float overlap = (ra + rb) - dist;
            Vec2  push    = diff / dist * (overlap * 0.5f);

            if (!world.collisions[a].isStatic) pa += push;
            if (!world.collisions[b].isStatic) pb -= push;
        }
    }
}
