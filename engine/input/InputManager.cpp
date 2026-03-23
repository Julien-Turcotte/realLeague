#include "input/InputManager.h"

void InputManager::update() {
    prevKeys = keys;
    prevMouseButtons = mouseButtons;
}

bool InputManager::processEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_QUIT:
            quit = true;
            return true;
        case SDL_EVENT_KEY_DOWN: {
            SDL_Scancode sc = event.key.scancode;
            if (sc >= 0 && sc < static_cast<int>(keys.size()))
                keys[static_cast<std::size_t>(sc)] = true;
            return true;
        }
        case SDL_EVENT_KEY_UP: {
            SDL_Scancode sc = event.key.scancode;
            if (sc >= 0 && sc < static_cast<int>(keys.size()))
                keys[static_cast<std::size_t>(sc)] = false;
            return true;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            int btn = event.button.button;
            if (btn >= 0 && btn < static_cast<int>(mouseButtons.size()))
                mouseButtons[static_cast<std::size_t>(btn)] = true;
            return true;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            int btn = event.button.button;
            if (btn >= 0 && btn < static_cast<int>(mouseButtons.size()))
                mouseButtons[static_cast<std::size_t>(btn)] = false;
            return true;
        }
        case SDL_EVENT_MOUSE_MOTION:
            mousePos.x = event.motion.x;
            mousePos.y = event.motion.y;
            return true;
        default:
            return false;
    }
}

bool InputManager::isKeyDown(SDL_Scancode sc) const {
    auto idx = static_cast<std::size_t>(sc);
    if (idx >= keys.size()) return false;
    return keys[idx];
}

bool InputManager::isKeyJustPressed(SDL_Scancode sc) const {
    auto idx = static_cast<std::size_t>(sc);
    if (idx >= keys.size()) return false;
    return keys[idx] && !prevKeys[idx];
}

bool InputManager::isMouseButtonDown(int button) const {
    auto idx = static_cast<std::size_t>(button);
    if (idx >= mouseButtons.size()) return false;
    return mouseButtons[idx];
}

bool InputManager::isMouseButtonJustPressed(int button) const {
    auto idx = static_cast<std::size_t>(button);
    if (idx >= mouseButtons.size()) return false;
    return mouseButtons[idx] && !prevMouseButtons[idx];
}
