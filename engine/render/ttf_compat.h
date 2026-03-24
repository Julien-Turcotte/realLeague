#pragma once

// Try common headers in order. If none available, define REALLEAGUE_HAS_TTF 0.
#if defined(__has_include)
#  if __has_include(<SDL3_ttf/SDL_ttf.h>)
#    include <SDL3_ttf/SDL_ttf.h>
#    define REALLEAGUE_HAS_TTF 1
#  elif __has_include(<SDL_ttf.h>)
#    include <SDL_ttf.h>
#    define REALLEAGUE_HAS_TTF 1
#  else
#    define REALLEAGUE_HAS_TTF 0
#  endif
#else
#  define REALLEAGUE_HAS_TTF 0
#endif

// If no real TTF header is available, provide minimal stubs so code compiles.
// The stubs are never used because REALLEAGUE_HAS_TTF == 0 in that case.
#if !REALLEAGUE_HAS_TTF
using TTF_Font = void;
inline int TTF_Init() { return 0; }
inline void TTF_Quit() {}
inline TTF_Font* TTF_OpenFont(const char*, int) { return nullptr; }
inline void TTF_CloseFont(TTF_Font*) {}
#endif
