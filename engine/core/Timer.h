#pragma once
#include <SDL3/SDL.h>

class Timer {
public:
    void start();
    float tick(); // returns deltaTime in seconds, capped at 0.05
    float getDeltaTime() const { return deltaTime; }
    float getTotalTime() const { return totalTime; }

private:
    Uint64 lastTick = 0;
    float deltaTime = 0.0f;
    float totalTime = 0.0f;
};
