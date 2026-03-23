#include "abilities/Fireball.h"

Fireball createFireball(const Vec2& from, const Vec2& to, float damage) {
    Fireball fb;
    fb.damage    = damage;
    fb.direction = (to - from).normalized();
    return fb;
}
