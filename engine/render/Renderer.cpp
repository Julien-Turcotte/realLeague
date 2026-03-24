#include "render/Renderer.h"
#include <cmath>
#include <SDL_ttf.h>

bool Renderer::init(const std::string& title, int width, int height) {
    this->width = width;
    this->height = height;

    window = SDL_CreateWindow(title.c_str(), width, height, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_DestroyWindow(window);
        window = nullptr;
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (TTF_Init() == -1) return false;

    return true;
}

void Renderer::shutdown() {
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window)   { SDL_DestroyWindow(window);   window   = nullptr; }
    if (font) { TTF_CloseFont(font); font = nullptr; }
    TTF_Quit();
}

void Renderer::clear(int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), 255);
    SDL_RenderClear(renderer);
}

void Renderer::present() {
    SDL_RenderPresent(renderer);
}

void Renderer::setColor(int r, int g, int b, int a) {
    SDL_SetRenderDrawColor(renderer, static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a));
}

void Renderer::drawRect(float x, float y, float w, float h, bool filled) {
    SDL_FRect rect{x, y, w, h};
    if (filled)
        SDL_RenderFillRect(renderer, &rect);
    else
        SDL_RenderRect(renderer, &rect);
}

void Renderer::drawCirclePoints(int cx, int cy, int x, int y) {
    SDL_RenderPoint(renderer, static_cast<float>(cx + x), static_cast<float>(cy + y));
    SDL_RenderPoint(renderer, static_cast<float>(cx - x), static_cast<float>(cy + y));
    SDL_RenderPoint(renderer, static_cast<float>(cx + x), static_cast<float>(cy - y));
    SDL_RenderPoint(renderer, static_cast<float>(cx - x), static_cast<float>(cy - y));
    SDL_RenderPoint(renderer, static_cast<float>(cx + y), static_cast<float>(cy + x));
    SDL_RenderPoint(renderer, static_cast<float>(cx - y), static_cast<float>(cy + x));
    SDL_RenderPoint(renderer, static_cast<float>(cx + y), static_cast<float>(cy - x));
    SDL_RenderPoint(renderer, static_cast<float>(cx - y), static_cast<float>(cy - x));
}

void Renderer::drawCircle(float cx, float cy, float radius) {
    int icx = static_cast<int>(cx);
    int icy = static_cast<int>(cy);
    int r   = static_cast<int>(radius);
    int x   = 0;
    int y   = r;
    int d   = 3 - 2 * r;
    drawCirclePoints(icx, icy, x, y);
    while (y >= x) {
        ++x;
        if (d > 0) { --y; d += 4 * (x - y) + 10; }
        else        { d += 4 * x + 6; }
        drawCirclePoints(icx, icy, x, y);
    }
}

void Renderer::drawLine(float x1, float y1, float x2, float y2) {
    SDL_RenderLine(renderer, x1, y1, x2, y2);
}

void Renderer::drawWorldRect(float x, float y, float w, float h, float camX, float camY, bool filled) {
    drawRect(x - camX, y - camY, w, h, filled);
}

void Renderer::drawWorldCircle(float cx, float cy, float radius, float camX, float camY) {
    drawCircle(cx - camX, cy - camY, radius);
}

void Renderer::drawWorldLine(float x1, float y1, float x2, float y2, float camX, float camY) {
    drawLine(x1 - camX, y1 - camY, x2 - camX, y2 - camY);
}

void Renderer::drawFogOverlay(int screenW, int screenH) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
    SDL_FRect full{0, 0, static_cast<float>(screenW), static_cast<float>(screenH)};
    SDL_RenderFillRect(renderer, &full);
}

bool Renderer::loadFont(const std::string& path, int size) {
    if (font) { TTF_CloseFont(font); font = nullptr; }
    font = TTF_OpenFont(path.c_str(), size);
    return font != nullptr;
}

void Renderer::drawText(const std::string& text, float x, float y, SDL_Color color) {
    if (!font) return;
    SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FRect dst = {x, y, static_cast<float>(surf->w), static_cast<float>(surf->h)};
    SDL_RenderTexture(renderer, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}
