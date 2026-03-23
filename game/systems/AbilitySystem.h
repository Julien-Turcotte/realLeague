#pragma once
#include "ecs/ECS.h"
#include "input/InputManager.h"

class AbilitySystem {
public:
    void update(World& world, float dt, const InputManager& input, float camX, float camY);

private:
    void fireAbility(World& world, int slot, EntityID champion, const Vec2& targetPos);
};
