#pragma once
#include <array>
#include "Vec2.h"

using EntityID = int;
constexpr EntityID INVALID_ENTITY = -1;

// Identifies which pixel-art sprite to use when rendering an entity.
enum class SpriteType : int {
    None = 0,
    PlayerChampion,
    BotChampion,
    BlueMinion,
    RedMinion,
    BlueTower,
    RedTower,
    Fireball,
};

struct Transform {
    Vec2 position;
    float rotation = 0.0f;
};

struct Velocity {
    Vec2 velocity;
    float speed = 150.0f;
    bool hasTarget = false;
    Vec2 target;
};

struct Health {
    float current = 100.0f;
    float max = 100.0f;
    float armor = 5.0f;
    bool isDead = false;
};

struct Attack {
    float damage = 20.0f;
    float range = 150.0f;
    float attackSpeed = 1.0f;
    float attackTimer = 0.0f;
    EntityID currentTarget = INVALID_ENTITY;
};

struct Ability {
    int id = 0;
    float cooldown = 5.0f;
    float currentCooldown = 0.0f;
    bool isActive = false;
};

struct Renderable {
    int colorR = 255;
    int colorG = 255;
    int colorB = 255;
    float width = 30.0f;
    float height = 30.0f;
    SpriteType spriteType = SpriteType::None;
};

struct AIComponent {
    enum class State { Idle, Moving, Attacking, Retreating };
    State state = State::Idle;
    EntityID targetEntity = INVALID_ENTITY;
    float aggroRange = 300.0f;
    float retreatHP = 20.0f;
    Vec2 laneTarget;
    int laneIndex = 0;
};

struct PlayerControlled {
    Vec2 moveTarget;
    bool hasTarget = false;
    EntityID attackTarget = INVALID_ENTITY;
};

struct TeamComponent {
    int teamId = 0; // 0 = blue, 1 = red
};

struct TowerComponent {
    float range = 400.0f;
    float attackTimer = 0.0f;
    float attackSpeed = 0.8f;
    float damage = 35.0f;
    EntityID currentTarget = INVALID_ENTITY;
};

struct ProjectileComponent {
    Vec2 direction;
    float speed = 400.0f;
    float damage = 0.0f;
    int ownerTeam = 0;
    EntityID owner = INVALID_ENTITY;
    float lifetime = 3.0f;
    EntityID targetEntity = INVALID_ENTITY; // homing target (INVALID_ENTITY = straight shot)
};

struct ChampionComponent {
    int level = 1;
    int experience = 0;
    std::array<float, 4> abilityCooldowns = {0, 0, 0, 0};
    std::array<float, 4> abilityMaxCooldowns = {5, 8, 12, 120};
    int deathCount = 0;
    Vec2 spawnPosition;
};

struct RespawnComponent {
    float timer = 0.0f;       // time remaining until respawn
    float maxTimer = 0.0f;    // total respawn duration (for UI display)
    Vec2 spawnPosition;       // world position to teleport to on respawn
};

struct MinionComponent {
    int laneIndex = 0;
    float bounty = 25.0f;
};

struct CollisionComponent {
    float radius = 15.0f;
    bool isStatic = false;
};
