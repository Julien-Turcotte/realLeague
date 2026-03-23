#include "systems/RenderSystem.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>

// ── Map ───────────────────────────────────────────────────────────────────────

void RenderSystem::renderMap(Renderer& renderer, Map& map, float camX, float camY) {
    map.render(renderer, camX, camY);
}

// ── Entities ──────────────────────────────────────────────────────────────────

void RenderSystem::renderEntities(World& world, Renderer& renderer, float camX, float camY) {
    for (EntityID id : world.entities) {
        if (world.renderables.count(id) == 0) continue;
        if (world.transforms.count(id) == 0) continue;
        if (world.projectiles.count(id)) continue; // handled separately
        if (world.healths.count(id) && world.healths[id].isDead) continue;

        const auto& rend = world.renderables[id];
        const auto& tr   = world.transforms[id];

        renderer.setColor(rend.colorR, rend.colorG, rend.colorB);
        renderer.drawWorldRect(
            tr.position.x - rend.width  * 0.5f,
            tr.position.y - rend.height * 0.5f,
            rend.width, rend.height, camX, camY);
    }
}

// ── Health bars ───────────────────────────────────────────────────────────────

void RenderSystem::renderHealthBars(World& world, Renderer& renderer, float camX, float camY) {
    for (EntityID id : world.entities) {
        if (world.healths.count(id) == 0) continue;
        if (world.transforms.count(id) == 0) continue;
        if (world.healths[id].isDead) continue;
        if (world.projectiles.count(id)) continue;

        const auto& hp = world.healths[id];
        const auto& tr = world.transforms[id];

        float spriteHalfH = world.renderables.count(id)
                            ? world.renderables[id].height * 0.5f
                            : 15.0f; // fallback half-height
        float barW   = 40.0f;
        float barH   = 5.0f;
        float barX   = tr.position.x - barW * 0.5f;
        float barY   = tr.position.y - spriteHalfH - 10.0f;
        float fillW  = barW * (hp.current / hp.max);

        // Background
        renderer.setColor(80, 0, 0);
        renderer.drawWorldRect(barX, barY, barW, barH, camX, camY);
        // Fill
        renderer.setColor(0, 200, 0);
        renderer.drawWorldRect(barX, barY, fillW, barH, camX, camY);
    }
}

// ── Projectiles ───────────────────────────────────────────────────────────────

void RenderSystem::renderProjectiles(World& world, Renderer& renderer, float camX, float camY) {
    for (auto& [id, proj] : world.projectiles) {
        if (world.transforms.count(id) == 0) continue;
        const auto& tr = world.transforms[id];
        renderer.setColor(255, 220, 0);
        renderer.drawWorldCircle(tr.position.x, tr.position.y, 6.0f, camX, camY);
    }
}

// ── Fog of war ────────────────────────────────────────────────────────────────

void RenderSystem::renderFogOfWar(World& world, Renderer& renderer,
                                   float camX, float camY, int screenW, int screenH) {
    // Draw semi-transparent dark overlay
    renderer.setColor(0, 0, 0, 100);
    renderer.drawRect(0, 0, static_cast<float>(screenW), static_cast<float>(screenH));

    // Draw visibility circles around friendly entities (team 0 = blue / player)
    SDL_SetRenderDrawBlendMode(renderer.getSDLRenderer(), SDL_BLENDMODE_NONE);
    for (EntityID id : world.entities) {
        if (world.teamComponents.count(id) == 0) continue;
        if (world.teamComponents[id].teamId != 0) continue;
        if (world.transforms.count(id) == 0) continue;
        if (world.healths.count(id) && world.healths[id].isDead) continue;

        const Vec2& pos = world.transforms[id].position;
        float vr = 300.0f; // visibility radius

        // Draw a lighter filled circle (approximate with a rect for simplicity)
        renderer.setColor(60, 120, 60, 255); // match map grass color
        float sx = pos.x - camX;
        float sy = pos.y - camY;
        // Draw filled circle using concentric horizontal lines
        for (float dy = -vr; dy <= vr; dy += 1.0f) {
            float dx = std::sqrt(vr * vr - dy * dy);
            renderer.drawLine(sx - dx, sy + dy, sx + dx, sy + dy);
        }
    }
    SDL_SetRenderDrawBlendMode(renderer.getSDLRenderer(), SDL_BLENDMODE_BLEND);
}

// ── Highlights ────────────────────────────────────────────────────────────────

namespace {
// Scale applied to an entity's bounding box to produce the highlight ring radius
constexpr float HIGHLIGHT_RADIUS_SCALE = 0.65f;

void drawThickCircle(Renderer& renderer, float cx, float cy, float radius, float camX, float camY) {
    renderer.drawWorldCircle(cx, cy, radius,     camX, camY);
    renderer.drawWorldCircle(cx, cy, radius + 1, camX, camY);
    renderer.drawWorldCircle(cx, cy, radius + 2, camX, camY);
}
} // namespace

void RenderSystem::renderHighlights(World& world, Renderer& renderer, float camX, float camY) {
    // Determine the player's explicit attack target (if any)
    EntityID attackTarget = INVALID_ENTITY;
    for (auto& [id, pc] : world.playerControlled) {
        if (pc.attackTarget != INVALID_ENTITY) {
            attackTarget = pc.attackTarget;
            break;
        }
    }

    // Draw attack-target ring (red)
    if (attackTarget != INVALID_ENTITY &&
        world.transforms.count(attackTarget) &&
        !(world.healths.count(attackTarget) && world.healths[attackTarget].isDead)) {
        const auto& tr = world.transforms[attackTarget];
        float radius = 20.0f;
        if (world.renderables.count(attackTarget)) {
            const auto& rend = world.renderables[attackTarget];
            radius = std::max(rend.width, rend.height) * HIGHLIGHT_RADIUS_SCALE;
        }
        renderer.setColor(255, 60, 60, 220);
        drawThickCircle(renderer, tr.position.x, tr.position.y, radius, camX, camY);
    }

    // Draw hover ring (yellow) – shown even over the attack target
    EntityID hovered = world.hoveredEnemy;
    if (hovered != INVALID_ENTITY &&
        world.transforms.count(hovered) &&
        !(world.healths.count(hovered) && world.healths[hovered].isDead)) {
        const auto& tr = world.transforms[hovered];
        float radius = 20.0f;
        if (world.renderables.count(hovered)) {
            const auto& rend = world.renderables[hovered];
            radius = std::max(rend.width, rend.height) * HIGHLIGHT_RADIUS_SCALE;
        }
        renderer.setColor(255, 230, 0, 220);
        drawThickCircle(renderer, tr.position.x, tr.position.y, radius, camX, camY);
    }
}


void RenderSystem::render(World& world, Renderer& renderer, Map& map,
                          UIManager& ui, float camX, float camY) {
    // Map background
    map.render(renderer, camX, camY);

    // Fog of war (drawn before entities so entities appear on top)
    renderFogOfWar(world, renderer, camX, camY,
                   renderer.getWidth(), renderer.getHeight());

    // Entities and UI (drawn after fog so they are always visible)
    renderEntities(world, renderer, camX, camY);
    renderHealthBars(world, renderer, camX, camY);
    renderProjectiles(world, renderer, camX, camY);

    // Hover and attack-target highlights (drawn after fog so always visible)
    renderHighlights(world, renderer, camX, camY);

    // UI overlay (drawn in screen space)
    ui.render(world, renderer, renderer.getWidth(), renderer.getHeight());
}
