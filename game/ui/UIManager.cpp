#include "ui/UIManager.h"
#include "map/Map.h"
#include <cmath>

// ── Ability bar ───────────────────────────────────────────────────────────────

void UIManager::renderAbilityBar(World& world, EntityID player,
                                  Renderer& renderer, int screenW, int screenH) {
    if (player == INVALID_ENTITY) return;
    if (world.champions.count(player) == 0) return;

    const auto& champ = world.champions[player];
    const char* labels[] = {"Q", "W", "E", "R"};
    const float boxW = 60.0f;
    const float boxH = 60.0f;
    const float spacing = 10.0f;
    float totalW = 4.0f * boxW + 3.0f * spacing;
    float startX = (static_cast<float>(screenW) - totalW) * 0.5f;
    float startY = static_cast<float>(screenH) - boxH - 20.0f;

    for (int i = 0; i < 4; ++i) {
        float x = startX + static_cast<float>(i) * (boxW + spacing);
        float y = startY;

        // Box background
        renderer.setColor(50, 50, 50, 220);
        renderer.drawRect(x, y, boxW, boxH);

        // Ability icon (colored square)
        int iconColors[4][3] = {{200,50,50},{50,200,50},{50,50,200},{200,180,0}};
        renderer.setColor(iconColors[i][0], iconColors[i][1], iconColors[i][2], 200);
        renderer.drawRect(x + 5, y + 5, boxW - 10, boxH - 10);

        // Cooldown overlay
        float cd    = champ.abilityCooldowns[i];
        float maxCd = champ.abilityMaxCooldowns[i];
        if (cd > 0.0f && maxCd > 0.0f) {
            float pct = cd / maxCd;
            renderer.setColor(0, 0, 0, 180);
            renderer.drawRect(x + 5, y + 5, boxW - 10, (boxH - 10) * pct);
        }

        // Box border
        renderer.setColor(200, 200, 200, 255);
        renderer.drawRect(x, y, boxW, boxH, false);

        (void)labels[i]; // label rendering would require a font; skipped for now
    }
}

// ── Minimap ───────────────────────────────────────────────────────────────────

void UIManager::renderMinimap(World& world, Renderer& renderer, int screenW, int screenH) {
    constexpr float mmW = 200.0f;
    constexpr float mmH = 200.0f;
    float mmX = static_cast<float>(screenW) - mmW - 10.0f;
    float mmY = static_cast<float>(screenH) - mmH - 10.0f;

    // Background
    renderer.setColor(20, 50, 20, 220);
    renderer.drawRect(mmX, mmY, mmW, mmH);

    // Entities
    for (EntityID id : world.entities) {
        if (world.transforms.count(id) == 0) continue;
        if (world.healths.count(id) && world.healths[id].isDead) continue;

        const Vec2& pos = world.transforms[id].position;
        float px = mmX + (pos.x / static_cast<float>(MAP_WIDTH))  * mmW;
        float py = mmY + (pos.y / static_cast<float>(MAP_HEIGHT)) * mmH;

        if (world.teamComponents.count(id)) {
            int team = world.teamComponents[id].teamId;
            renderer.setColor(team == 0 ? 100 : 255,
                              team == 0 ? 150 : 50,
                              team == 0 ? 255 : 50, 255);
        } else {
            renderer.setColor(200, 200, 200, 255);
        }
        renderer.drawRect(px - 2, py - 2, 4.0f, 4.0f);
    }

    // Minimap border
    renderer.setColor(180, 180, 180, 255);
    renderer.drawRect(mmX, mmY, mmW, mmH, false);
}

// ── Level info ────────────────────────────────────────────────────────────────

void UIManager::renderLevelInfo(World& world, EntityID player, Renderer& renderer) {
    if (player == INVALID_ENTITY) return;
    if (world.champions.count(player) == 0) return;

    int level = world.champions[player].level;
    // Draw a simple level indicator box
    renderer.setColor(0, 0, 0, 180);
    renderer.drawRect(10, 10, 120, 30);
    renderer.setColor(255, 220, 50, 255);
    renderer.drawRect(10, 10, 120, 30, false);

    // Represent level as filled squares (one per level, max 18)
    float sqW = std::min(6.0f, 110.0f / 18.0f);
    for (int i = 0; i < level && i < 18; ++i) {
        renderer.setColor(255, 220, 50, 255);
        renderer.drawRect(14.0f + static_cast<float>(i) * (sqW + 1.0f), 17.0f, sqW, 16.0f);
    }
}

// ── Respawn timer ─────────────────────────────────────────────────────────────

void UIManager::renderRespawnTimer(World& world, EntityID player,
                                    Renderer& renderer, int screenW, int screenH) {
    if (player == INVALID_ENTITY) return;
    if (world.respawnComponents.count(player) == 0) return;

    const auto& rc = world.respawnComponents[player];

    // Dark overlay to signal the player is dead
    renderer.setColor(0, 0, 0, 160);
    renderer.drawRect(0, 0, static_cast<float>(screenW), static_cast<float>(screenH));

    // Progress bar: shows how much of the respawn time has elapsed
    constexpr float barW = 300.0f;
    constexpr float barH =  20.0f;
    float barX = (static_cast<float>(screenW) - barW) * 0.5f;
    float barY = static_cast<float>(screenH) * 0.5f - barH * 0.5f + 30.0f;

    // Background
    renderer.setColor(60, 0, 0, 220);
    renderer.drawRect(barX, barY, barW, barH);

    // Fill (shrinks as the timer counts down to zero)
    float fill = (rc.maxTimer > 0.0f) ? (rc.timer / rc.maxTimer) : 0.0f;
    renderer.setColor(220, 60, 60, 255);
    renderer.drawRect(barX, barY, barW * fill, barH);

    // Border
    renderer.setColor(200, 200, 200, 255);
    renderer.drawRect(barX, barY, barW, barH, false);

    // Individual second ticks along the bar
    int totalSeconds = static_cast<int>(rc.maxTimer);
    for (int s = 1; s < totalSeconds; ++s) {
        float tx = barX + barW * (1.0f - static_cast<float>(s) / rc.maxTimer);
        renderer.setColor(200, 200, 200, 180);
        renderer.drawLine(tx, barY, tx, barY + barH);
    }

    // Small squares above the bar – one per second remaining (capped at 30)
    int secsLeft = static_cast<int>(std::ceil(rc.timer));
    if (secsLeft > 30) secsLeft = 30;
    constexpr float sqW = 8.0f;
    constexpr float sqH = 8.0f;
    constexpr float sqSpacing = 2.0f;
    float totalSqW = static_cast<float>(secsLeft) * (sqW + sqSpacing) - sqSpacing;
    float sqStartX = (static_cast<float>(screenW) - totalSqW) * 0.5f;
    float sqY = barY - sqH - 6.0f;
    for (int i = 0; i < secsLeft; ++i) {
        float sx = sqStartX + static_cast<float>(i) * (sqW + sqSpacing);
        renderer.setColor(220, 80, 80, 230);
        renderer.drawRect(sx, sqY, sqW, sqH);
    }
}

// ── Public entry ─────────────────────────────────────────────────────────────

void UIManager::render(World& world, Renderer& renderer, int screenW, int screenH, float dt, float camX, float camY) {
    renderLevelInfo(world, world.playerEntity, renderer);
    renderAbilityBar(world, world.playerEntity, renderer, screenW, screenH);
    renderMinimap(world, renderer, screenW, screenH);
    renderRespawnTimer(world, world.playerEntity, renderer, screenW, screenH);
    renderHealthBars(world, renderer, camX, camY, dt);
}

// ── Health bars ───────────────────────────────────────────────────────────────

void UIManager::renderHealthBars(World& world, Renderer& renderer, float camX, float camY, float dt) {
    for (EntityID id : world.entities) {
        if (world.healths.count(id) == 0) continue;
        if (world.transforms.count(id) == 0) continue;
        if (world.healths[id].isDead) continue;
        if (world.projectiles.count(id)) continue;

        const auto& hp = world.healths[id];
        const auto& tr = world.transforms[id];

        // Smooth displayed health
        float& disp = displayedHealth[id];
        if (disp <= 0.0f) disp = hp.current; // initialize
        float speed = 6.0f; // higher = snappier
        float alpha = std::min(1.0f, speed * dt);
        disp = disp + (hp.current - disp) * alpha; // use dt for consistent smoothing

        float spriteHalfH = world.renderables.count(id)
                            ? world.renderables[id].height * 0.5f
                            : 15.0f; // fallback half-height
        float barW   = 40.0f;
        float barH   = 5.0f;
        float barX   = tr.position.x - barW * 0.5f;
        float barY   = tr.position.y - spriteHalfH - 10.0f;
        float fillW  = barW * (disp / hp.max);

        // Background
        renderer.setColor(40, 20, 20, 200);
        renderer.drawWorldRect(barX, barY, barW, barH, camX, camY);
        // Smooth gradient-like fill (two tones)
        renderer.setColor(200, 80, 80, 255);
        renderer.drawWorldRect(barX, barY, fillW * 0.6f, barH, camX, camY);
        renderer.setColor(220, 180, 80, 220);
        renderer.drawWorldRect(barX + fillW * 0.6f, barY, std::max(0.0f, fillW - fillW * 0.6f), barH, camX, camY);

        // Border
        renderer.setColor(0, 0, 0, 180);
        renderer.drawWorldRect(barX - 1, barY - 1, barW + 2, barH + 2, camX, camY, false);
    }
}
