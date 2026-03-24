#pragma once
#include "ecs/ECS.h"
#include "render/Renderer.h"
#include <unordered_map>

class UIManager {
public:
    void render(World& world, Renderer& renderer, int screenW, int screenH, float dt, float camX, float camY);

private:
    void renderAbilityBar(World& world, EntityID player, Renderer& renderer,
                          int screenW, int screenH);
    void renderMinimap(World& world, Renderer& renderer, int screenW, int screenH);
    void renderLevelInfo(World& world, EntityID player, Renderer& renderer);
    void renderRespawnTimer(World& world, EntityID player, Renderer& renderer,
                            int screenW, int screenH);
    std::unordered_map<EntityID, float> displayedHealth; // for smooth health bar animation
};
