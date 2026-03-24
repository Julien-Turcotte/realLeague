#include "render/PixelSprite.h"
#include <cassert>

// ── Colour helpers ────────────────────────────────────────────────────────────
// Pack R,G,B,A into a Uint32 in SDL_PIXELFORMAT_RGBA8888 layout (0xRRGGBBAA).
#define C(r,g,b,a) \
    (((Uint32)(r)<<24)|((Uint32)(g)<<16)|((Uint32)(b)<<8)|(Uint32)(a))

// Build a texture from a character-map string and a char→colour palette.
// '.' is always transparent, regardless of palette contents.
SDL_Texture* PixelArtSprites::makeTexture(SDL_Renderer* rend,
                                           const char* charMap, int w, int h,
                                           const std::unordered_map<char,Uint32>& pal)
{
    SDL_Surface* surf = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA8888);
    if (!surf) return nullptr;
    const SDL_PixelFormatDetails* fmt =
        SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888);
    assert(fmt);
    auto* px = static_cast<Uint32*>(surf->pixels);

    for (int i = 0; i < w * h; ++i) {
        char ch = charMap[i];
        if (ch == '.') {
            px[i] = SDL_MapRGBA(fmt, nullptr, 0, 0, 0, 0);
        } else {
            auto it = pal.find(ch);
            Uint32 v = (it != pal.end()) ? it->second : C(255,0,255,255);
            px[i] = SDL_MapRGBA(fmt, nullptr,
                                (v>>24)&0xFF, (v>>16)&0xFF,
                                (v>> 8)&0xFF, (v    )&0xFF);
        }
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);
    SDL_DestroySurface(surf);
    if (tex) SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    return tex;
}

// ─────────────────────────────────────────────────────────────────────────────
// Sprite character maps (16×16 unless noted).
// Each string is exactly w*h characters, row-major, no whitespace.
// ─────────────────────────────────────────────────────────────────────────────

// ── Player Champion ─ 16×16 ─ Blue Mage ──────────────────────────────────────
//  K=outline  B=blue-hat  b=blue-body  d=dark-blue-legs
//  L=skin-light  M=skin-mid  m=skin-shadow  E=eye
//  G=gold  W=staff-tip  w=staff-shaft
static const char PLAYER_MAP[] =
    "....KBBKK......."   // row 0  hat tip
    "...KBBbBBK......"   // row 1  hat
    "..KBBbGbBBK....."   // row 2  hat (star detail)
    "..KLLLLLLK......"   // row 3  face top
    "..KLELMLEKK....."   // row 4  eyes
    "..KMMMmMMK......"   // row 5  face / subtle mouth
    "...KKKKKKK......"   // row 6  chin
    ".KGGbbbbGGK....."   // row 7  collar
    ".KbbbbbbbbKW...."   // row 8  body + staff tip
    ".KbbGbMGbbKw...."   // row 9  belt buckle
    ".KbbbbbbbbKw...."   // row 10 body
    ".KddddddddK....."   // row 11 hips
    "..KddddddK......"   // row 12 upper legs
    "..Kdd..ddK......"   // row 13 legs
    "..Kdd..ddK......"   // row 14 lower legs
    "..KKK..KKK......";  // row 15 feet

static const std::unordered_map<char,Uint32> PLAYER_PAL = {
    {'K', C( 20, 20, 30,255)},  // outline
    {'B', C( 72,118,224,255)},  // blue hat
    {'b', C( 90,148,242,255)},  // blue body
    {'d', C( 36, 68,150,255)},  // dark blue legs
    {'L', C(255,215,170,255)},  // skin light
    {'M', C(235,185,140,255)},  // skin mid
    {'m', C(200,140, 90,255)},  // skin shadow / mouth
    {'E', C( 48, 48,140,255)},  // eye
    {'G', C(255,208, 40,255)},  // gold
    {'W', C(238,238,252,255)},  // staff tip white
    {'w', C(158,158,178,255)},  // staff shaft gray
};

// ── Bot Champion ─ 16×16 ─ Red Warrior ───────────────────────────────────────
//  K=outline  H=red-helm  h=helm-dark  k=transparent-gap
//  S=silver  R=red-body  r=red-mid  D=dark-red-legs
//  L=skin-light  M=skin-mid  m=skin-shadow  E=glowing-eye
static const char BOT_MAP[] =
    "K.........K....."   // row 0  horn tips
    "Hk..HHHHHh.Hk..."   // row 1  horns
    ".KHHHHhHHHHK...."   // row 2  helm
    ".KLLLLLLLLhK...."   // row 3  face
    ".KLEMMLMLEhK...."   // row 4  glowing eyes
    ".KMMMMMMMmK....."   // row 5  face / chin
    "..KKKKKKKKK....."   // row 6  chin
    ".KSSRRRRRRSSKh.."   // row 7  armor shoulders
    ".KRRRRRRRRRRhK.."   // row 8  body
    ".KRrSRrRRrSRhK.."   // row 9  body details
    ".KRRRRRRRRRRhK.."   // row 10 body
    ".KDDDDDDDDDRhK.."   // row 11 hips
    "..KDDDDDDDKh...."   // row 12 upper legs
    "..KDD..DDKh....."   // row 13 legs
    "..KDD..DDK......"   // row 14 lower legs
    "..KKK..KKK......";  // row 15 feet

static const std::unordered_map<char,Uint32> BOT_PAL = {
    {'K', C( 20, 20, 30,255)},  // outline
    {'H', C(188, 48, 48,255)},  // red helm
    {'h', C(140, 28, 28,255)},  // helm dark
    {'k', C(  0,  0,  0,  0)},  // transparent gap
    {'S', C(175,175,180,255)},  // silver
    {'R', C(216, 60, 60,255)},  // red body
    {'r', C(164, 32, 32,255)},  // red mid
    {'D', C(108, 20, 20,255)},  // dark red legs
    {'L', C(255,215,170,255)},  // skin light
    {'M', C(235,185,140,255)},  // skin mid
    {'m', C(200,140, 90,255)},  // skin shadow
    {'E', C(255,115, 20,255)},  // glowing eye
};

// ── Blue Minion ─ 16×16 ──────────────────────────────────────────────────────
//  K=outline  k=soft-outline  B=bright-blue  b=mid-blue  d=dark-legs
//  L=skin  E=eye
static const char BLUE_MINION_MAP[] =
    "................"   // row 0
    "....KBBK........"   // row 1  helm
    "....KBbBK......."   // row 2  helm
    "....KLLK........"   // row 3  face
    "....KELEk......."   // row 4  eyes
    "....KLLLk......."   // row 5  face
    ".....KKKk......."   // row 6  chin
    "...KbbbbbK......"   // row 7  body
    "..KbbBBBbbK....."   // row 8  chest plate
    "..KbbBBBbbK....."   // row 9  chest plate
    "...KbbBbbK......"   // row 10 body
    "....KdddK......."   // row 11 hips
    "....Kd.dK......."   // row 12 legs
    "....Kd.dK......."   // row 13 legs
    "....KK.KK......."   // row 14 feet
    "................";  // row 15

static const std::unordered_map<char,Uint32> BLUE_MINION_PAL = {
    {'K', C( 20, 20, 30,255)},
    {'k', C( 20, 20, 30,180)},  // soft outline
    {'B', C( 90,148,242,255)},  // bright blue
    {'b', C( 48, 96,200,255)},  // mid blue
    {'d', C( 28, 60,140,255)},  // dark blue legs
    {'L', C(255,215,170,255)},  // skin
    {'E', C( 48, 48,140,255)},  // eye
};

// ── Red Minion ─ 16×16 ───────────────────────────────────────────────────────
//  Same silhouette as blue minion, red palette.
static const char RED_MINION_MAP[] =
    "................"
    "....KRRK........"
    "....KRrRK......."
    "....KLLK........"
    "....KELEk......."
    "....KLLLk......."
    ".....KKKk......."
    "...KrrrrrK......"
    "..KrrRRRrrK....."
    "..KrrRRRrrK....."
    "...KrrRrrK......"
    "....KdddK......."
    "....Kd.dK......."
    "....Kd.dK......."
    "....KK.KK......."
    "................";

static const std::unordered_map<char,Uint32> RED_MINION_PAL = {
    {'K', C( 20, 20, 30,255)},
    {'k', C( 20, 20, 30,180)},
    {'R', C(216, 60, 60,255)},  // bright red
    {'r', C(164, 32, 32,255)},  // mid red
    {'d', C(108, 20, 20,255)},  // dark red legs
    {'L', C(255,215,170,255)},
    {'E', C(255,115, 20,255)},  // orange glow eye
};

// ── Fireball ─ 12×12 ─────────────────────────────────────────────────────────
//  C=white-core  Y=yellow  O=orange  R=red-orange
static const char FIREBALL_MAP[] =
    "...ROOOOR..."   // row 0
    "..ROYYYORY.."   // row 1
    ".ROYCCCCYOR."   // row 2
    "ROYCCCCCCYOR"   // row 3
    "OYCCCCCCCYO."   // row 4
    "OYCCCCCCCYO."   // row 5
    "OYCCCCCCCYO."   // row 6
    "ROYCCCCCCYOR"   // row 7
    ".ROYCCCCYOR."   // row 8
    "..ROYYYORY.."   // row 9
    "...ROOOOR..."   // row 10
    "............";  // row 11

static const std::unordered_map<char,Uint32> FIREBALL_PAL = {
    {'C', C(255,255,224,255)},  // core
    {'Y', C(255,222, 60,255)},  // yellow
    {'O', C(255,138, 28,255)},  // orange
    {'R', C(224, 56,  0,210)},  // red-orange semi-transparent
};

// ── Tower (16×32) ─────────────────────────────────────────────────────────────
// Towers are taller (height > width) so we use a 16×32 procedural approach
// that is easier to maintain than a 512-character string.
static SDL_Texture* makeTowerTexture(SDL_Renderer* rend, bool isRed) {
    constexpr int W = 16, H = 32;

    const Uint32 KK = C( 20, 20, 30,255);  // outline
    const Uint32 ST = C(118,132,148,255);  // stone
    const Uint32 SL = C(150,165,182,255);  // stone light
    const Uint32 SD = C( 76, 86, 98,255);  // stone dark
    const Uint32 TM = C( 55, 62, 70,255);  // mortar joint
    const Uint32 WN = C( 22, 22, 38,160);  // window interior
    const Uint32 WT = C(140,158,200, 80);  // window top lit
    const Uint32 FL = isRed ? C(192, 48, 48,255) : C( 46, 90,188,255); // flag
    const Uint32 FD = isRed ? C(140, 28, 28,255) : C( 28, 56,140,255); // flag dark
    const Uint32 PP = C(180,160,120,255);  // pole
    const Uint32 TR = C(  0,  0,  0,  0); // transparent

    Uint32 px[W * H];

    // Fill pixel row `rowIdx` from an initialiser-list; excess columns stay transparent.
    auto fillRow = [&](int rowIdx, std::initializer_list<Uint32> cols) {
        int c = 0;
        for (Uint32 v : cols) { if (c < W) px[rowIdx * W + (c++)] = v; }
        while (c < W) px[rowIdx * W + (c++)] = TR;
    };

    //            0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
    fillRow( 0, {KK, TR, KK, TR, KK, TR, KK, TR, KK, TR, KK, TR, KK, TR, PP, PP});
    fillRow( 1, {KK, SL, KK, SL, KK, SL, KK, SL, KK, SL, KK, SL, KK, TR, FL, FL});
    fillRow( 2, {KK, SL, SL, SL, SL, SL, SL, SL, SL, SL, SL, SL, KK, TR, FL, FD});
    fillRow( 3, {KK, SL, ST, ST, ST, ST, ST, ST, ST, ST, ST, SL, KK, TR, FD, FL});
    fillRow( 4, {KK, SL, ST, SD, WN, WN, WN, WN, SD, ST, ST, SL, KK, TR, TR, TR});
    fillRow( 5, {KK, SL, ST, SD, WT, WN, WN, WT, SD, ST, ST, SL, KK, TR, TR, TR});
    fillRow( 6, {KK, SL, ST, SD, WN, WN, WN, WN, SD, ST, ST, SL, KK, TR, TR, TR});
    fillRow( 7, {KK, SL, ST, SD, SD, SD, SD, SD, SD, ST, ST, SL, KK, TR, TR, TR});
    fillRow( 8, {TM, SL, ST, ST, ST, ST, ST, ST, ST, ST, ST, SL, TM, TR, TR, TR});
    fillRow( 9, {KK, SL, ST, ST, ST, ST, ST, ST, ST, ST, ST, SL, KK, TR, TR, TR});
    fillRow(10, {KK, SL, ST, ST, ST, ST, ST, ST, ST, ST, ST, SL, KK, TR, TR, TR});
    fillRow(11, {TM, SL, ST, ST, ST, ST, ST, ST, ST, ST, ST, SL, TM, TR, TR, TR});
    fillRow(12, {KK, SL, ST, SD, WN, WN, WN, WN, SD, ST, ST, SL, KK, TR, TR, TR});
    fillRow(13, {KK, SL, ST, SD, WT, WN, WN, WT, SD, ST, ST, SL, KK, TR, TR, TR});
    fillRow(14, {KK, SL, ST, SD, WN, WN, WN, WN, SD, ST, ST, SL, KK, TR, TR, TR});
    fillRow(15, {KK, SL, ST, SD, SD, SD, SD, SD, SD, ST, ST, SL, KK, TR, TR, TR});
    fillRow(16, {TM, SL, ST, ST, ST, ST, ST, ST, ST, ST, ST, SL, TM, TR, TR, TR});
    fillRow(17, {KK, SL, ST, ST, ST, ST, ST, ST, ST, ST, ST, SL, KK, TR, TR, TR});
    fillRow(18, {KK, SL, ST, ST, ST, ST, ST, ST, ST, ST, ST, SL, KK, TR, TR, TR});
    fillRow(19, {TM, SL, ST, ST, ST, ST, ST, ST, ST, ST, ST, SL, TM, TR, TR, TR});
    fillRow(20, {KK, SL, ST, ST, ST, ST, ST, ST, ST, ST, ST, SL, KK, TR, TR, TR});
    fillRow(21, {KK, SL, ST, ST, ST, ST, ST, ST, ST, ST, ST, SL, KK, TR, TR, TR});
    fillRow(22, {TM, SL, ST, ST, ST, ST, ST, ST, ST, ST, ST, SL, TM, TR, TR, TR});
    fillRow(23, {KK, SL, ST, ST, ST, ST, ST, ST, ST, ST, ST, SL, KK, TR, TR, TR});
    fillRow(24, {KK, KK, SD, SD, SD, SD, SD, SD, SD, SD, SD, KK, KK, TR, TR, TR});
    fillRow(25, {SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, TR, TR, TR});
    fillRow(26, {SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, TR, TR, TR});
    fillRow(27, {SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, TR, TR, TR});
    fillRow(28, {KK, SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, SD, KK, TR, TR, TR});
    fillRow(29, {KK, KK, KK, KK, KK, KK, KK, KK, KK, KK, KK, KK, KK, TR, TR, TR});
    fillRow(30, {TR, TR, TR, TR, TR, TR, TR, TR, TR, TR, TR, TR, TR, TR, TR, TR});
    fillRow(31, {TR, TR, TR, TR, TR, TR, TR, TR, TR, TR, TR, TR, TR, TR, TR, TR});

    SDL_Surface* surf = SDL_CreateSurface(W, H, SDL_PIXELFORMAT_RGBA8888);
    if (!surf) return nullptr;
    const SDL_PixelFormatDetails* fmt =
        SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888);
    auto* dst = static_cast<Uint32*>(surf->pixels);
    for (int i = 0; i < W * H; ++i) {
        dst[i] = SDL_MapRGBA(fmt, nullptr,
                             (px[i]>>24)&0xFF, (px[i]>>16)&0xFF,
                             (px[i]>> 8)&0xFF, (px[i]    )&0xFF);
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);
    SDL_DestroySurface(surf);
    if (tex) SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    return tex;
}

#undef C

// ── Public API ────────────────────────────────────────────────────────────────

void PixelArtSprites::init(SDL_Renderer* renderer) {
    m_textures[static_cast<int>(SpriteType::PlayerChampion)] =
        makeTexture(renderer, PLAYER_MAP, 16, 16, PLAYER_PAL);

    m_textures[static_cast<int>(SpriteType::BotChampion)] =
        makeTexture(renderer, BOT_MAP, 16, 16, BOT_PAL);

    m_textures[static_cast<int>(SpriteType::BlueMinion)] =
        makeTexture(renderer, BLUE_MINION_MAP, 16, 16, BLUE_MINION_PAL);

    m_textures[static_cast<int>(SpriteType::RedMinion)] =
        makeTexture(renderer, RED_MINION_MAP, 16, 16, RED_MINION_PAL);

    m_textures[static_cast<int>(SpriteType::BlueTower)] =
        makeTowerTexture(renderer, false);

    m_textures[static_cast<int>(SpriteType::RedTower)] =
        makeTowerTexture(renderer, true);

    m_textures[static_cast<int>(SpriteType::Fireball)] =
        makeTexture(renderer, FIREBALL_MAP, 12, 12, FIREBALL_PAL);
}

void PixelArtSprites::shutdown() {
    for (auto& [key, tex] : m_textures) {
        if (tex) SDL_DestroyTexture(tex);
    }
    m_textures.clear();
}

SDL_Texture* PixelArtSprites::get(SpriteType type) const {
    if (type == SpriteType::None) return nullptr;
    auto it = m_textures.find(static_cast<int>(type));
    return (it != m_textures.end()) ? it->second : nullptr;
}
