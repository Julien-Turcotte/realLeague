#pragma once
#include "ecs/ECS.h"
#include "render/Renderer.h"
#include "render/PixelSprite.h"
#include "map/Map.h"
#include "ui/UIManager.h"

class RenderSystem {
public:
    // Must be called once after the Renderer is initialised.
    void init(Renderer& renderer);
    void shutdown();

    void render(World& world, Renderer& renderer, Map& map,
                UIManager& ui, float camX, float camY);

private:
    void renderMap(Renderer& renderer, Map& map, float camX, float camY);
    void renderEntities(World& world, Renderer& renderer, float camX, float camY);
    void renderHealthBars(World& world, Renderer& renderer, float camX, float camY);
    void renderProjectiles(World& world, Renderer& renderer, float camX, float camY);
    void renderFogOfWar(World& world, Renderer& renderer, float camX, float camY,
                        int screenW, int screenH);
    void renderHighlights(World& world, Renderer& renderer, float camX, float camY);

    PixelArtSprites m_sprites;
};
