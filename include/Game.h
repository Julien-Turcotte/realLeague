#pragma once
#include <memory>
#include "ecs/ECS.h"
#include "render/Renderer.h"
#include "input/InputManager.h"
#include "audio/AudioManager.h"
#include "core/Timer.h"
#include "map/Map.h"
#include "map/Pathfinding.h"
#include "ui/UIManager.h"
#include "systems/MovementSystem.h"
#include "systems/CombatSystem.h"
#include "systems/AbilitySystem.h"
#include "systems/AISystem.h"
#include "systems/RenderSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/RespawnSystem.h"

class Game {
public:
    bool init();
    void run();
    void shutdown();

private:
    void handleInput();
    void update(float dt);
    void render();
    void spawnInitialEntities();
    void updateCamera();
    EntityID findEnemyAt(const Vec2& worldPos, int myTeam);

    static constexpr int SCREEN_WIDTH  = 1280;
    static constexpr int SCREEN_HEIGHT = 720;

    World          world;
    Renderer       renderer;
    InputManager   input;
    AudioManager   audio;
    Timer          timer;
    Map            map;
    Pathfinding    pathfinding;
    UIManager      ui;

    MovementSystem  movementSystem;
    CombatSystem    combatSystem;
    AbilitySystem   abilitySystem;
    AISystem        aiSystem;
    RenderSystem    renderSystem;
    CollisionSystem collisionSystem;
    RespawnSystem   respawnSystem;

    float camX   = 0.0f;
    float camY   = 0.0f;
    bool  running = false;

    // Right-click move indicator animation
    Vec2  moveIndicatorPos;
    float moveIndicatorTime = 0.0f;
    static constexpr float MOVE_INDICATOR_DURATION     = 0.6f;
    static constexpr float MOVE_INDICATOR_START_RADIUS = 28.0f;
    static constexpr float MOVE_INDICATOR_END_RADIUS   = 6.0f;
};
