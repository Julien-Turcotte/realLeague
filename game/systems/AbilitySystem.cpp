#include "systems/AbilitySystem.h"
#include "entities/EntityFactory.h"
#include <algorithm>

void AbilitySystem::update(World& world, float dt,
                           const InputManager& input, float camX, float camY) {
    // Tick cooldowns for all champions
    for (auto& [id, champ] : world.champions) {
        for (auto& cd : champ.abilityCooldowns)
            if (cd > 0.0f) cd = std::max(0.0f, cd - dt);
    }

    // Player input
    EntityID player = world.playerEntity;
    if (player == INVALID_ENTITY) return;
    if (world.champions.count(player) == 0) return;
    if (world.healths.count(player) && world.healths[player].isDead) return;

    Vec2 mouseWorld{input.getMousePosition().x + camX,
                    input.getMousePosition().y + camY};

    struct KeySlot { SDL_Scancode sc; int slot; };
    constexpr KeySlot bindings[] = {
        {SDL_SCANCODE_Q, 0},
        {SDL_SCANCODE_W, 1},
        {SDL_SCANCODE_E, 2},
        {SDL_SCANCODE_R, 3},
    };

    for (const auto& b : bindings) {
        if (input.isKeyJustPressed(b.sc)) {
            auto& champ = world.champions[player];
            if (champ.abilityCooldowns[b.slot] <= 0.0f)
                fireAbility(world, b.slot, player, mouseWorld);
        }
    }
}

void AbilitySystem::fireAbility(World& world, int slot, EntityID champion, const Vec2& targetPos) {
    if (world.champions.count(champion) == 0) return;
    auto& champ = world.champions[champion];

    // Set cooldown
    champ.abilityCooldowns[slot] = champ.abilityMaxCooldowns[slot];

    if (slot == 0) {
        // Q – Fireball
        if (world.transforms.count(champion) == 0) return;
        Vec2 origin = world.transforms[champion].position;
        Vec2 dir    = targetPos - origin;
        int team    = world.teamComponents.count(champion) ? world.teamComponents[champion].teamId : 0;
        EntityFactory::createFireball(world, origin, dir, 80.0f, team, champion);
        // Play fireball sound
        // audio.playFireballSound(); // Needs access to AudioManager
    }
    // W, E, R – placeholder: just start the cooldown (already set above)
}
