#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include "ecs/Components.h"  // for SpriteType

// Manages SDL_Texture objects built from hand-crafted pixel-art data.
class PixelArtSprites {
public:
    // Build all sprite textures.  Must be called after the SDL renderer exists.
    void init(SDL_Renderer* renderer);
    void shutdown();

    // Returns the texture for the given sprite type (nullptr if None/unknown).
    SDL_Texture* get(SpriteType type) const;

private:
    // Build a texture from a run-length character map + RGBA palette.
    // charMap: one character per pixel, row-major, exactly w*h characters.
    // palette: maps each character to an RGBA8888 Uint32 colour; '.' is always
    //          treated as transparent regardless of palette contents.
    static SDL_Texture* makeTexture(SDL_Renderer* rend,
                                    const char* charMap, int w, int h,
                                    const std::unordered_map<char, Uint32>& palette);

    std::unordered_map<int, SDL_Texture*> m_textures;
};
