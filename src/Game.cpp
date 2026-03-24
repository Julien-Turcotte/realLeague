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

#if REALLEAGUE_HAS_TTF
    if (!renderer.loadFont("assets/textures/Roboto-Bold.ttf", 20)) {
        std::cerr << "Failed to load font\n";
        return false;
    }
#endif

    audio.init();
    map.init();
    pathfinding.init(MAP_WIDTH, MAP_HEIGHT, 100);

    spawnInitialEntities();
    playerGold = 0;
    playerKills = 0;
    playerDeaths = 0;
    pingActive = false;
    recalling = false;
    recallTimer = 0.0f;
    paused = false;
    gameOver = false;
    winnerTeam = -1;

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

    // Pause feature
    if (input.isKeyJustPressed(SDL_SCANCODE_P)) {
        paused = !paused;
    }
    if (paused) return;

    // Recall feature (B key)
    if (!recalling && input.isKeyJustPressed(SDL_SCANCODE_B)) {
        recalling = true;
        recallTimer = 3.0f;
    }

    // Sprint feature (Shift key)
    if (world.playerEntity != INVALID_ENTITY && world.velocities.count(world.playerEntity)) {
        auto& vel = world.velocities[world.playerEntity];
        if (input.isKeyDown(SDL_SCANCODE_LSHIFT) || input.isKeyDown(SDL_SCANCODE_RSHIFT)) {
            vel.speed = 250.0f;
        } else {
            vel.speed = 150.0f;
        }
    }

    // Minimap click-to-move
    Vec2 mouse = input.getMousePosition();
    int screenW = renderer.getWidth();
    int screenH = renderer.getHeight();
    float mmW = 200.0f, mmH = 200.0f;
    float mmX = static_cast<float>(screenW) - mmW - 10.0f;
    float mmY = static_cast<float>(screenH) - mmH - 10.0f;
    if (input.isMouseButtonJustPressed(SDL_BUTTON_LEFT) &&
        mouse.x >= mmX && mouse.x <= mmX + mmW &&
        mouse.y >= mmY && mouse.y <= mmY + mmH) {
        float wx = (mouse.x - mmX) / mmW * MAP_WIDTH;
        float wy = (mouse.y - mmY) / mmH * MAP_HEIGHT;
        if (world.playerEntity != INVALID_ENTITY && world.playerControlled.count(world.playerEntity)) {
            auto& pc = world.playerControlled[world.playerEntity];
            pc.moveTarget = {wx, wy};
            pc.hasTarget = true;
        }
    }

    // Ping system (Alt+click)
    if ((input.isKeyDown(SDL_SCANCODE_LALT) || input.isKeyDown(SDL_SCANCODE_RALT)) &&
        input.isMouseButtonJustPressed(SDL_BUTTON_LEFT)) {
        Vec2 mouseScreen = input.getMousePosition();
        pingLocation = {mouseScreen.x + camX, mouseScreen.y + camY};
        pingActive = true;
        pingTimer = 2.0f;
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
    if (paused || gameOver) return;

    // Health regen for champions
    for (auto& [id, champ] : world.champions) {
        if (world.healths.count(id) && !world.healths[id].isDead) {
            auto& hp = world.healths[id];
            hp.current = std::min(hp.max, hp.current + 5.0f * dt);
        }
    }

    // Recall logic
    if (recalling && world.playerEntity != INVALID_ENTITY && world.healths.count(world.playerEntity)) {
        recallTimer -= dt;
        if (recallTimer <= 0.0f) {
            recalling = false;
            // Teleport to spawn
            if (world.transforms.count(world.playerEntity) && world.champions.count(world.playerEntity)) {
                world.transforms[world.playerEntity].position = world.champions[world.playerEntity].spawnPosition;
            }
        }
    }
    if (recalling && (input.isKeyDown(SDL_SCANCODE_W) || input.isKeyDown(SDL_SCANCODE_A) ||
                      input.isKeyDown(SDL_SCANCODE_S) || input.isKeyDown(SDL_SCANCODE_D) ||
                      input.isMouseButtonDown(SDL_BUTTON_RIGHT))) {
        recalling = false; // Cancel recall on movement
    }

    // Ping timer
    if (pingActive) {
        pingTimer -= dt;
        if (pingTimer <= 0.0f) pingActive = false;
    }

    // Game over check (nexus destroyed)
    for (EntityID id : world.entities) {
        if (world.champions.count(id) == 0 && world.healths.count(id) &&
            (world.transforms.count(id) && (
                (world.transforms[id].position - Vec2{200.0f, 2800.0f}).length() < 70.0f ||
                (world.transforms[id].position - Vec2{2800.0f, 200.0f}).length() < 70.0f))) {
            if (world.healths[id].isDead) {
                gameOver = true;
                winnerTeam = (world.transforms[id].position.x < 1000) ? 1 : 0;
            }
        }
    }

    movementSystem.update(world, dt);
    aiSystem.update(world, map, dt);
    combatSystem.update(world, dt);
    abilitySystem.update(world, dt, input, camX, camY);
    collisionSystem.update(world, dt);
    respawnSystem.update(world, dt);
    world.flushDestroyQueue();
    updateCamera();
    if (moveIndicatorTime > 0.0f) moveIndicatorTime -= dt;

    // Sync game state to world for UIManager
    world.playerGold = playerGold;
    world.playerKills = playerKills;
    world.playerDeaths = playerDeaths;
    world.recalling = recalling;
    world.recallTimer = recallTimer;
    world.gameOver = gameOver;
    world.winnerTeam = winnerTeam;
}

void Game::render() {
    renderer.clear(20, 20, 20);
    renderSystem.render(world, renderer, map, ui, camX, camY);

    // Ping indicator
    if (pingActive) {
        renderer.setColor(255, 255, 0, 180);
        renderer.drawWorldCircle(pingLocation.x, pingLocation.y, 32.0f, camX, camY);
    }

    // Recall indicator
    if (recalling && world.playerEntity != INVALID_ENTITY && world.transforms.count(world.playerEntity)) {
        auto& pos = world.transforms[world.playerEntity].position;
        renderer.setColor(0, 200, 255, 180);
        renderer.drawWorldCircle(pos.x, pos.y, 32.0f, camX, camY);
    }

    // Game over screen
    if (gameOver) {
        renderer.setColor(0, 0, 0, 200);
        renderer.drawRect(0, 0, renderer.getWidth(), renderer.getHeight());
        renderer.setColor(255, 255, 255, 255);
        // No font: just draw a white rectangle as a placeholder
        renderer.drawRect(renderer.getWidth()/2-150, renderer.getHeight()/2-40, 300, 80, false);
        // Could add SDL_ttf for text, but not in this minimal example
    }

    renderer.present();
}

void Game::run() {
    timer.start();
    running = true;
    while (running) {
        float dt = timer.tick();
        handleInput();
        update(dt);
        render();
    }
}

void Game::shutdown() {
    audio.shutdown();
    renderer.shutdown();
    SDL_Quit();
}
