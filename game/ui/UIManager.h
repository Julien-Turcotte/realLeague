#pragma once
#include "ecs/ECS.h"
#include "render/Renderer.h"

class UIManager {
public:
    void render(World& world, Renderer& renderer, int screenW, int screenH);

private:
    void renderAbilityBar(World& world, EntityID player, Renderer& renderer,
                          int screenW, int screenH);
    void renderMinimap(World& world, Renderer& renderer, int screenW, int screenH);
    void renderLevelInfo(World& world, EntityID player, Renderer& renderer);
    void renderRespawnTimer(World& world, EntityID player, Renderer& renderer,
                            int screenW, int screenH);
    void renderGold(World& world, int gold, Renderer& renderer, int screenW);
    void renderKillDeath(World& world, int kills, int deaths, Renderer& renderer, int screenW);
    void renderRecall(World& world, bool recalling, float recallTimer, Renderer& renderer, int screenW, int screenH);
    void renderGameOver(bool gameOver, int winnerTeam, Renderer& renderer, int screenW, int screenH);
};
