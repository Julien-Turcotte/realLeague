#pragma once
#include "Vec2.h"

struct Fireball {
    float speed = 400.0f;
    float damage = 80.0f;
    Vec2 direction;
};

Fireball createFireball(const Vec2& from, const Vec2& to, float damage = 80.0f);
