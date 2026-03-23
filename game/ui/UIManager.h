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
};
