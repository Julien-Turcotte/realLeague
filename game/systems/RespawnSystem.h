#pragma once
#include "ecs/ECS.h"

class RespawnSystem {
public:
    void update(World& world, float dt);

private:
    static constexpr float BASE_RESPAWN_TIME  = 10.0f; // seconds at 0 deaths
    static constexpr float TIME_PER_DEATH     =  3.0f; // extra seconds per additional death
};
