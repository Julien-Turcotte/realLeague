#include "core/Timer.h"

void Timer::start() {
    lastTick = SDL_GetPerformanceCounter();
}

float Timer::tick() {
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 freq = SDL_GetPerformanceFrequency();
    deltaTime = static_cast<float>(now - lastTick) / static_cast<float>(freq);
    if (deltaTime > 0.05f) deltaTime = 0.05f;
    lastTick = now;
    totalTime += deltaTime;
    return deltaTime;
}
