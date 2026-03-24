#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "Vec2.h"

#if defined(__has_include)
#  if __has_include(<SDL_ttf.h>)
#    include <SDL_ttf.h>
#    define REALLEAGUE_HAS_TTF 1
#  elif __has_include(<SDL3_ttf/SDL_ttf.h>)
#    include <SDL3_ttf/SDL_ttf.h>
#    define REALLEAGUE_HAS_TTF 1
#  else
#    define REALLEAGUE_HAS_TTF 0
#  endif
#else
#  include <SDL_ttf.h>
#  define REALLEAGUE_HAS_TTF 1
#endif

class Renderer {
public:
    bool init(const std::string& title, int width, int height);
    void shutdown();
    void clear(int r = 20, int g = 20, int b = 20);
    void present();
    void setColor(int r, int g, int b, int a = 255);
    void drawRect(float x, float y, float w, float h, bool filled = true);
    void drawCircle(float cx, float cy, float radius);
    void drawLine(float x1, float y1, float x2, float y2);

    // World-space draw (applies camera offset)
    void drawWorldRect(float x, float y, float w, float h, float camX, float camY, bool filled = true);
    void drawWorldCircle(float cx, float cy, float radius, float camX, float camY);
    void drawWorldLine(float x1, float y1, float x2, float y2, float camX, float camY);

    // Fog of war overlay (drawn in screen space, no camera offset needed)
    void drawFogOverlay(int screenW, int screenH);

    SDL_Renderer* getSDLRenderer() { return renderer; }
    SDL_Window* getWindow() { return window; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    bool loadFont(const std::string& path, int size);
    void drawText(const std::string& text, float x, float y, SDL_Color color);

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int width = 1280;
    int height = 720;
#if REALLEAGUE_HAS_TTF
    TTF_Font* font = nullptr;
#else
    void* font = nullptr; // no TTF support
#endif
    void drawCirclePoints(int cx, int cy, int x, int y);
};
