#include "Game.h"
#include "entities/EntityFactory.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <algorithm>

bool Game::init() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return false;
    }

    if (!renderer.init("realLeague", SCREEN_WIDTH, SCREEN_HEIGHT)) {
        std::cerr << "Renderer init failed: " << SDL_GetError() << "\n";
        return false;
    }

    audio.init();
    map.init();
    pathfinding.init(MAP_WIDTH, MAP_HEIGHT, 100);

    spawnInitialEntities();
    return true;
}

void Game::spawnInitialEntities() {
    // Player champion – blue team
    EntityID player = EntityFactory::createPlayerChampion(world, {300.0f, 2700.0f});
    world.playerEntity = player;

    // Bot champion – red team, mid lane
    EntityFactory::createBotChampion(world, {2700.0f, 300.0f});

    // Blue minions (one per lane)
    EntityFactory::createMinion(world, 0, 0, {250.0f, 2600.0f}); // top
    EntityFactory::createMinion(world, 0, 1, {400.0f, 2400.0f}); // mid
    EntityFactory::createMinion(world, 0, 2, {700.0f, 2700.0f}); // bot

    // Red minions (one per lane)
    EntityFactory::createMinion(world, 1, 0, {2750.0f, 400.0f});  // top
    EntityFactory::createMinion(world, 1, 1, {2600.0f, 600.0f});  // mid
    EntityFactory::createMinion(world, 1, 2, {2300.0f, 300.0f});  // bot

    // Towers
    EntityFactory::createTower(world, 0, {500.0f,  2500.0f}); // blue
    EntityFactory::createTower(world, 1, {2500.0f,  500.0f}); // red
}

void Game::updateCamera() {
    if (world.playerEntity == INVALID_ENTITY) return;
    if (world.transforms.count(world.playerEntity) == 0) return;

    const Vec2& pos = world.transforms[world.playerEntity].position;
    camX = pos.x - static_cast<float>(SCREEN_WIDTH)  * 0.5f;
    camY = pos.y - static_cast<float>(SCREEN_HEIGHT) * 0.5f;
    camX = std::clamp(camX, 0.0f, static_cast<float>(MAP_WIDTH  - SCREEN_WIDTH));
    camY = std::clamp(camY, 0.0f, static_cast<float>(MAP_HEIGHT - SCREEN_HEIGHT));

    // Screen shake from world state
    if (world.screenShakeTime > 0.0f) {
        world.screenShakeTime = std::max(0.0f, world.screenShakeTime - timer.getDeltaTime());
        float intensity = world.screenShakeIntensity * (world.screenShakeTime / 0.25f);
        // simple random offset
        float ox = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * intensity;
        float oy = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * intensity;
        camX += ox;
        camY += oy;
        if (world.screenShakeTime <= 0.0f) world.screenShakeIntensity = 0.0f;
    }
}

void Game::handleInput() {
    input.update();
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        input.processEvent(event);
    }

    if (input.shouldQuit()) {
        running = false;
        return;
    }

    // Update hovered enemy every frame based on current mouse position
    if (world.playerEntity != INVALID_ENTITY &&
        world.teamComponents.count(world.playerEntity)) {
        Vec2 mouseScreen = input.getMousePosition();
        Vec2 worldPos{mouseScreen.x + camX, mouseScreen.y + camY};
        int myTeam = world.teamComponents[world.playerEntity].teamId;
        world.hoveredEnemy = findEnemyAt(worldPos, myTeam);
    }

    // Right-click → attack enemy or move player
    if (input.isMouseButtonJustPressed(SDL_BUTTON_RIGHT)) {
        if (world.playerEntity != INVALID_ENTITY &&
            world.playerControlled.count(world.playerEntity)) {
            Vec2 mouseScreen = input.getMousePosition();
            Vec2 worldPos{mouseScreen.x + camX, mouseScreen.y + camY};

            EntityID enemyAt = world.hoveredEnemy;
            auto& pc = world.playerControlled[world.playerEntity];
            if (enemyAt != INVALID_ENTITY) {
                // Right-clicked on enemy: set as explicit attack target
                pc.attackTarget = enemyAt;
                pc.hasTarget    = false;
            } else {
                // Right-clicked on empty space: move and clear attack target
                pc.attackTarget = INVALID_ENTITY;
                pc.moveTarget   = worldPos;
                pc.hasTarget    = true;
            }
        }
    }
}

EntityID Game::findEnemyAt(const Vec2& worldPos, int myTeam) {
    for (EntityID id : world.entities) {
        if (!world.teamComponents.count(id)) continue;
        if (world.teamComponents[id].teamId == myTeam) continue;
        if (world.healths.count(id) && world.healths[id].isDead) continue;
        if (!world.transforms.count(id)) continue;
        if (!world.renderables.count(id)) continue;

        const auto& tr   = world.transforms[id];
        const auto& rend = world.renderables[id];
        float radius = std::max(rend.width, rend.height) * 0.5f;
        if (worldPos.distance(tr.position) <= radius)
            return id;
    }
    return INVALID_ENTITY;
}

void Game::update(float dt) {
    movementSystem.update(world, dt);
    aiSystem.update(world, map, dt);
    combatSystem.update(world, dt);
    abilitySystem.update(world, dt, input, camX, camY);
    collisionSystem.update(world, dt);
    respawnSystem.update(world, dt);
    world.flushDestroyQueue();
    updateCamera();
    if (moveIndicatorTime > 0.0f) moveIndicatorTime -= dt;
}

void Game::render() {
    renderer.clear(20, 20, 20);
    renderSystem.render(world, renderer, map, ui, camX, camY);

    // Right-click move indicator animation (drawn on top of everything)
    if (moveIndicatorTime > 0.0f) {
        float progress = 1.0f - (moveIndicatorTime / MOVE_INDICATOR_DURATION);
        float radius   = MOVE_INDICATOR_START_RADIUS
                         - progress * (MOVE_INDICATOR_START_RADIUS - MOVE_INDICATOR_END_RADIUS);
        int   alpha    = static_cast<int>((moveIndicatorTime / MOVE_INDICATOR_DURATION) * 230.0f);
        renderer.setColor(50, 255, 100, alpha);
        renderer.drawWorldCircle(moveIndicatorPos.x, moveIndicatorPos.y, radius,       camX, camY);
        renderer.drawWorldCircle(moveIndicatorPos.x, moveIndicatorPos.y, radius - 3.0f, camX, camY);
    }

    renderer.present();
}

void Game::run() {
    timer.start();
    running = true;
    while (running) {
        float dt = timer.tick();

        handleInput();

        // If a hit-stop is active, consume its time and skip simulation update to create a micro-freeze.
        float simDt = dt;
        if (world.hitStopTime > 0.0f) {
            world.hitStopTime = std::max(0.0f, world.hitStopTime - dt);
            simDt = 0.0f;
        }

        update(simDt);
        render();
    }
}

void Game::shutdown() {
    audio.shutdown();
    renderer.shutdown();
    SDL_Quit();
}
