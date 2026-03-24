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

        // Shadow
        renderer.setColor(0, 0, 0, 120);
        renderer.drawRect(x + 3, y + 3, boxW, boxH);

        // Box background (gradient effect by layering)
        renderer.setColor(60, 60, 60, 240);
        renderer.drawRect(x, y, boxW, boxH);
        renderer.setColor(80, 80, 80, 180);
        renderer.drawRect(x + 2, y + 2, boxW - 4, boxH - 4);

        // Ability icon (colored square)
        int iconColors[4][3] = {{200,50,50},{50,200,50},{50,50,200},{200,180,0}};
        renderer.setColor(iconColors[i][0], iconColors[i][1], iconColors[i][2], 220);
        renderer.drawRect(x + 8, y + 8, boxW - 16, boxH - 16);

        // Cooldown overlay (semi-transparent dark overlay)
        float cd    = champ.abilityCooldowns[i];
        float maxCd = champ.abilityMaxCooldowns[i];
        if (cd > 0.0f && maxCd > 0.0f) {
            float pct = cd / maxCd;
            renderer.setColor(0, 0, 0, 180);
            renderer.drawRect(x + 8, y + 8 + (boxH - 16) * (1.0f - pct), boxW - 16, (boxH - 16) * pct);
        }

        // Box border (thicker, more visible)
        renderer.setColor(220, 220, 220, 255);
        renderer.drawRect(x, y, boxW, boxH, false);
        renderer.setColor(120, 120, 120, 255);
        renderer.drawRect(x - 1, y - 1, boxW + 2, boxH + 2, false);

        // Tooltip (simple: highlight Q with a different border)
        if (i == 0) {
            renderer.setColor(255, 255, 100, 180);
            renderer.drawRect(x - 2, y - 2, boxW + 4, boxH + 4, false);
        }
    }
}

// ── Minimap ───────────────────────────────────────────────────────────────────

void UIManager::renderMinimap(World& world, Renderer& renderer, int screenW, int screenH) {
    constexpr float mmW = 200.0f;
    constexpr float mmH = 200.0f;
    float mmX = static_cast<float>(screenW) - mmW - 10.0f;
    float mmY = static_cast<float>(screenH) - mmH - 10.0f;

    // Shadow
    renderer.setColor(0, 0, 0, 120);
    renderer.drawRect(mmX + 4, mmY + 4, mmW, mmH);

    // Background
    renderer.setColor(30, 60, 30, 230);
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
        renderer.drawRect(px - 3, py - 3, 6.0f, 6.0f);
    }

    // Minimap border (thicker, more visible)
    renderer.setColor(220, 220, 220, 255);
    renderer.drawRect(mmX, mmY, mmW, mmH, false);
    renderer.setColor(80, 80, 80, 255);
    renderer.drawRect(mmX - 2, mmY - 2, mmW + 4, mmH + 4, false);
}

// ── Level info ────────────────────────────────────────────────────────────────

void UIManager::renderLevelInfo(World& world, EntityID player, Renderer& renderer) {
    if (player == INVALID_ENTITY) return;
    if (world.champions.count(player) == 0) return;

    int level = world.champions[player].level;
    // Draw a simple level indicator box with shadow
    renderer.setColor(0, 0, 0, 120);
    renderer.drawRect(13, 13, 120, 30);
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

// ── Gold ─────────────────────────────────────────────────────────────────────

void UIManager::renderGold(World& world, int gold, Renderer& renderer, int screenW) {
    // Shadow
    renderer.setColor(0, 0, 0, 120);
    renderer.drawRect(screenW - 143, 13, 120, 30);
    renderer.setColor(255, 215, 0, 220);
    renderer.drawRect(screenW - 140, 10, 120, 30);
    renderer.setColor(255, 215, 0, 255);
    renderer.drawRect(screenW - 140, 10, 120, 30, false);
    // Draw gold amount as text
    renderer.drawText("Gold: " + std::to_string(gold), screenW - 130, 15, SDL_Color{60, 40, 0, 255});
}

// ── Kill/Death ────────────────────────────────────────────────────────────────

void UIManager::renderKillDeath(World& world, int kills, int deaths, Renderer& renderer, int screenW) {
    // Shadow
    renderer.setColor(0, 0, 0, 120);
    renderer.drawRect(screenW/2 - 57, 13, 120, 30);
    renderer.setColor(200, 200, 200, 220);
    renderer.drawRect(screenW/2 - 60, 10, 120, 30);
    renderer.setColor(255, 0, 0, 255);
    renderer.drawRect(screenW/2 - 60, 10, 120, 30, false);
    // Draw K/D as text
    renderer.drawText("K: " + std::to_string(kills) + " / D: " + std::to_string(deaths),
                      screenW/2 - 50, 15, SDL_Color{100, 0, 0, 255});
}

// ── Recall ─────────────────────────────────────────────────────────────────────

void UIManager::renderRecall(World& world, bool recalling, float recallTimer, Renderer& renderer, int screenW, int screenH) {
    if (!recalling) return;
    // Shadow
    renderer.setColor(0, 0, 0, 120);
    renderer.drawRect(screenW/2 - 77, screenH - 97, 160, 30);
    renderer.setColor(0, 200, 255, 180);
    renderer.drawRect(screenW/2 - 80, screenH - 100, 160, 30);
    renderer.setColor(0, 200, 255, 255);
    renderer.drawRect(screenW/2 - 80, screenH - 100, 160 * (recallTimer/3.0f), 30);
}

// ── Game over ──────────────────────────────────────────────────────────────────

void UIManager::renderGameOver(bool gameOver, int winnerTeam, Renderer& renderer, int screenW, int screenH) {
    if (!gameOver) return;
    renderer.setColor(0, 0, 0, 200);
    renderer.drawRect(0, 0, screenW, screenH);
    renderer.setColor(255, 255, 255, 255);
    renderer.drawRect(screenW/2-150, screenH/2-40, 300, 80, false);
    // Add a colored border for the winning team
    if (winnerTeam == 0)
        renderer.setColor(100, 200, 255, 255);
    else
        renderer.setColor(255, 100, 100, 255);
    renderer.drawRect(screenW/2-155, screenH/2-45, 310, 90, false);
}

// ── Public entry ─────────────────────────────────────────────────────────────

void UIManager::render(World& world, Renderer& renderer, int screenW, int screenH) {
    renderLevelInfo(world, world.playerEntity, renderer);
    renderAbilityBar(world, world.playerEntity, renderer, screenW, screenH);
    renderMinimap(world, renderer, screenW, screenH);
    renderRespawnTimer(world, world.playerEntity, renderer, screenW, screenH);
    // Polished UI: show gold, K/D, recall, game over overlays
    renderGold(world, world.playerGold, renderer, screenW);
    renderKillDeath(world, world.playerKills, world.playerDeaths, renderer, screenW);
    renderRecall(world, world.recalling, world.recallTimer, renderer, screenW, screenH);
    renderGameOver(world.gameOver, world.winnerTeam, renderer, screenW, screenH);
}
