#pragma once
#include <SDL3/SDL.h>
#include "Vec2.h"
#include <array>

class InputManager {
public:
    void update();
    bool processEvent(const SDL_Event& event);

    bool isKeyDown(SDL_Scancode sc) const;
    bool isKeyJustPressed(SDL_Scancode sc) const;
    bool isMouseButtonDown(int button) const;
    bool isMouseButtonJustPressed(int button) const;
    Vec2 getMousePosition() const { return mousePos; }
    bool shouldQuit() const { return quit; }

private:
    bool quit = false;
    Vec2 mousePos;
    std::array<bool, 512> keys{};
    std::array<bool, 512> prevKeys{};
    std::array<bool, 8> mouseButtons{};
    std::array<bool, 8> prevMouseButtons{};
};
