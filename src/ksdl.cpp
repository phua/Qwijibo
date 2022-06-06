#include "../include/ksdl.hpp"

#include <iostream>
#include <stdexcept>

// class KVector;

float KVector::magnitude() const
{
    return sqrt(x * x + y * y);
}

void KVector::normalize()
{
    float m = magnitude();
    if (m > 0) {
        (*this) *= 1.f / m;
    }
}

KVector KVector::operator+(const KVector &v) const
{
    return KVector(x + v.x, y + v.y);
}

KVector KVector::operator-(const KVector &v) const
{
    return KVector(x - v.x, y - v.y);
}

KVector KVector::operator*(float c) const
{
    return KVector(x * c, y * c);
}

KVector &KVector::operator*=(float c)
{
    x *= c, y *= c;
    return *this;
}

KVector KVector::operator/(float c) const
{
    return KVector(x / c, y / c);
}

KVector &KVector::operator/=(float c)
{
    x /= c, y /= c;
    return *this;
}

KVector &operator+=(KVector &u, const KVector &v)
{
    u.x += v.x, u.y += v.y;
    return u;
}

KVector &operator-=(KVector &u, const KVector &v) {
    u.x -= v.x, u.y -= v.y;
    return u;
}

std::ostream &operator<<(std::ostream &os, KVector const &v)
{
    return os << "KVector(" << v.x << ", " << v.y << ")";
}

// class KFontCache;

KFontCache *KFontCache::instance = NULL;

KFontCache::KFontCache()
{
    if (TTF_Init() == -1) {
        SDL_Log("TTF_Init(): %s\n", TTF_GetError());
    }
}

KFontCache::~KFontCache()
{
    for (auto iter = fonts.begin(); iter != fonts.end(); /* iter++ */) {
        TTF_CloseFont(iter->second);
        iter = fonts.erase(iter);
    }

    TTF_Quit();
}

KFontCache *KFontCache::getInstance()
{
    if (!instance) {
        instance = new KFontCache();
        atexit([]() { delete instance; instance = NULL; });
    }
    return instance;
}

TTF_Font *KFontCache::loadFont(std::string k, const char *file, int ptsize)
{
    unloadFont(k);

    TTF_Font *f = TTF_OpenFont(file, ptsize);
    if (f) {
        fonts[k] = f;
    } else {
        SDL_Log("TTF_OpenFont(%s): %s\n", file, TTF_GetError());
    }
    return f;
}

void KFontCache::unloadFont(std::string k)
{
    try {
        TTF_CloseFont(fonts.at(k));
        fonts.erase(k);
    } catch (std::out_of_range) {}
}

TTF_Font *KFontCache::getFont(std::string k)
{
    try {
        return fonts.at(k);
    } catch (std::out_of_range e) {
        fprintf(stderr, "KFontCache::getFont(%s): %s\n", k.c_str(), e.what());
        return NULL;
    }
}

// class KTextureCache;

KTextureCache *KTextureCache::instance = NULL;

KTextureCache::KTextureCache()
{
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("IMG_Init(IMG_INIT_PNG): %s\n", IMG_GetError());
    }
}

KTextureCache::~KTextureCache()
{
    for (auto iter = textures.begin(); iter != textures.end(); /* iter++ */) {
        SDL_DestroyTexture(iter->second);
        iter = textures.erase(iter);
    }

    IMG_Quit();
}

KTextureCache *KTextureCache::getInstance()
{
    if (!instance) {
        instance = new KTextureCache();
        atexit([]() { delete instance; instance = NULL; });
    }
    return instance;
}

SDL_Texture *KTextureCache::loadTexture(SDL_Renderer *r, std::string k, const char *file, KRGBA rgb)
{
    unloadTexture(k);

    // SDL_Surface *s = SDL_LoadBMP(file);
    SDL_Surface *s = IMG_Load(file);
    SDL_Texture *t = NULL;
    if (s) {
        if (SDL_SetColorKey(s, rgb.colorKeyFlag, SDL_MapRGB(s->format, rgb.r, rgb.g, rgb.b)) < 0) {
            SDL_Log("SDL_SetColorKey(%s): %s\n", file, SDL_GetError());
        }

        // t = SDL_ConvertSurface(s, surface->format, 0);
        t = SDL_CreateTextureFromSurface(r, s);
        if (t) {
            textures[k] = t;
        } else {
            // SDL_Log("SDL_ConvertSurface(%s): %s\n", file, SDL_GetError());
            SDL_Log("SDL_CreateTextureFromSurface(%s): %s\n", file, SDL_GetError());
        }
        SDL_FreeSurface(s);
    } else {
        // SDL_Log("SDL_LoadBMP(%s): %s\n", file, SDL_GetError());
        SDL_Log("IMG_Load(%s): %s\n", file, IMG_GetError());
    }
    return t;
}

SDL_Texture *KTextureCache::loadFontTexture(SDL_Renderer *r, std::string k, std::string f, const char *text, SDL_Color fg)
{
    TTF_Font *font = KFontCache::getInstance()->getFont(f);
    if (!font) {                // TODO
        return NULL;
    }

    unloadTexture(k);

    SDL_Surface *s = TTF_RenderText_Solid(font, text, fg);
    SDL_Texture *t = NULL;
    if (s) {
        t = SDL_CreateTextureFromSurface(r, s);
        if (t) {
            textures[k] = t;
        } else {
            SDL_Log("SDL_CreateTextureFromSurface(%s): %s\n", text, SDL_GetError());
        }
        SDL_FreeSurface(s);
    } else {
        SDL_Log("TTF_RenderText_Solid(%s): %s\n", text, TTF_GetError());
    }
    return t;
}

void KTextureCache::unloadTexture(std::string k)
{
    try {
        SDL_DestroyTexture(textures.at(k));
        textures.erase(k);
    } catch (std::out_of_range) {}
}

SDL_Texture *KTextureCache::getTexture(std::string k) const
{
    try {
        return textures.at(k);
    } catch (std::out_of_range e) {
        fprintf(stderr, "KTextureCache::getTexture(%s): %s\n", k.c_str(), e.what());
        return NULL;
    }
}

void KTextureCache::renderTexture(SDL_Renderer *r, std::string k, int x, int y, SDL_Rect *src,
                                  double angle, SDL_Point *center, SDL_RendererFlip flip)
{
    try {
        SDL_Texture *t = textures.at(k);
        SDL_Rect dst = { x, y, 0, 0 };
        if (src) {
            dst.w = src->w, dst.h = src->h;
        } else {
            SDL_QueryTexture(t, NULL, NULL, &dst.w, &dst.h);
        }
        // SDL_RenderCopy(r, t, src, &dst);
        SDL_RenderCopyEx(r, t, src, &dst, angle, center, flip);
    } catch (std::out_of_range e) {
        fprintf(stderr, "KTextureCache::renderTexture(%s): %s\n", k.c_str(), e.what());
    }
}

void KTextureCache::renderSprite(SDL_Renderer *r, std::string k, int x, int y,
                                 int spriteM, int spriteN, int spriteW, int spriteH)
{
    fprintf(stderr, "KTextureCache::renderSprite(): DEPRECATED\n");
    SDL_Rect src = {
        ((int) SDL_GetTicks() / 100) % spriteN * spriteW, // TODO mspf
        spriteM * spriteH,
        spriteW,
        spriteH
    };
    renderTexture(r, k, x, y, &src);
}

// class KTexture;

KTexture::KTexture(std::string k, SDL_Rect src, bool bg, int x, int y)
    : key(k), src(src), position(x, y), isBgTexture(bg)
{
    if (src.w == 0 || src.h == 0) {
        SDL_Texture *t = getTexture();
        if (t) {
            SDL_QueryTexture(t, NULL, NULL, &src.w, &src.h);
        } else {
            using namespace std::string_literals;
            throw std::runtime_error("KTexture("s + key + ") not found");
        }
    }
}

SDL_Texture *KTexture::getTexture()
{
    return KTextureCache::getInstance()->getTexture(key);
}

void KTexture::render(SDL_Renderer *r)
{
    SDL_Texture *t = getTexture();
    if (t) {
        if (isBgTexture) {
            SDL_RenderCopy(r, t, &this->src, NULL);
        } else {
            SDL_Rect dst = getRect();
            SDL_RenderCopy(r, t, &this->src, &dst);
        }
    }
}

std::ostream &operator<<(std::ostream &os, KTexture const &t)
{
    return os << "KTexture(" << t.key << ")";
}

// class KSprite;

void KSprite::render(SDL_Renderer *r)
{
    SDL_Texture *t = getTexture();
    if (t) {
        KSpriteSheet &s = spriteSheet;
        int m = 0, n = 0;
        if (s.axis == KSPRITE_HORIZONTAL) {
            m = (1 % s.rows), n = ((SDL_GetTicks() / 100) % s.cols); // TODO
        } else {
            m = ((SDL_GetTicks() / 100) % s.rows), n = (1 % s.cols); // TODO
        }
        SDL_Rect src = {
            s.region.x + n * (s.borderX + s.spriteW + s.borderX + s.marginX) + s.borderX,
            s.region.y + m * (s.borderY + s.spriteH + s.borderY + s.marginY) + s.borderY,
            s.spriteW,
            s.spriteH
        };
        SDL_Rect dst = getRect();
        SDL_RenderCopyEx(r, t, &src, &dst, 0, NULL, flip);
    }
}

// class KButton;

void KButton::handleEvent(SDL_Event &e)
{
    // TODO handleMouseEvent(SDL_GetMouseState(&x, &y))
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            SDL_Point p = { e.button.x, e.button.y };
            SDL_Rect r = getRect();
            if (SDL_EnclosePoints(&p, 1, &r, NULL)) {
                clicked = true;
            }
        }
    }
    if (e.type == SDL_MOUSEBUTTONUP) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            SDL_Point p = { e.button.x, e.button.y };
            SDL_Rect r = getRect();
            if (SDL_EnclosePoints(&p, 1, &r, NULL) && clicked) {
                handler->onMouseButtonClick(source);
            }
        }
        clicked = false;
    }
}

// class KSoundCache;

KSoundCache *KSoundCache::instance = NULL;

KSoundCache::KSoundCache()
{
    if (!(SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO)) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
            SDL_Log("SDL_InitSubSystem(SDL_INIT_AUDIO): %s\n", SDL_GetError());
            return;
        }
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("Mix_OpenAudio(): %s\n", Mix_GetError());
    }
}

KSoundCache::~KSoundCache()
{
    for (auto iter = musics.begin(); iter != musics.end(); /* iter++ */) {
        Mix_FreeMusic(iter->second);
        iter = musics.erase(iter);
    }

    for (auto iter = chunks.begin(); iter != chunks.end(); /* iter++ */) {
        Mix_FreeChunk(iter->second);
        iter = chunks.erase(iter);
    }

    Mix_CloseAudio();
    Mix_Quit();
}

KSoundCache *KSoundCache::getInstance()
{
    if (!instance) {
        instance = new KSoundCache();
        atexit([]() { delete instance; instance = NULL; });
    }
    return instance;
}

Mix_Music *KSoundCache::loadMusic(std::string k, const char *file)
{
    unloadMusic(k);

    Mix_Music *m = Mix_LoadMUS(file);
    if (m) {
        musics[k] = m;
    } else {
        SDL_Log("Mix_LoadMUS(%s): %s\n", file, Mix_GetError());
    }
    return m;
}

Mix_Chunk *KSoundCache::loadChunk(std::string k, const char *file)
{
    unloadChunk(k);

    Mix_Chunk *c = Mix_LoadWAV(file);
    if (c) {
        chunks[k] = c;
    } else {
        SDL_Log("Mix_LoadWAV(%s): %s\n", file, Mix_GetError());
    }
    return c;
}

void KSoundCache::unloadMusic(std::string k)
{
    try {
        Mix_FreeMusic(musics.at(k));
        musics.erase(k);
    } catch (std::out_of_range) {}
}

void KSoundCache::unloadChunk(std::string k)
{
    try {
        Mix_FreeChunk(chunks.at(k));
        chunks.erase(k);
    } catch (std::out_of_range) {}
}

Mix_Music *KSoundCache::getMusic(std::string k) const
{
    try {
        return musics.at(k);
    } catch (std::out_of_range e) {
        fprintf(stderr, "KSoundCache::getMusic(%s): %s\n", k.c_str(), e.what());
        return NULL;
    }
}

Mix_Chunk *KSoundCache::getChunk(std::string k) const
{
    try {
        return chunks.at(k);
    } catch (std::out_of_range e) {
        fprintf(stderr, "KSoundCache::getChunk(%s): %s\n", k.c_str(), e.what());
        return NULL;
    }
}

void KSoundCache::playMusic(std::string k, int loops)
{
    try {
        Mix_PlayMusic(musics.at(k), loops);
    } catch (std::out_of_range e) {
        fprintf(stderr, "KSoundCache::playMusic(%s): %s\n", k.c_str(), e.what());
    }
}

void KSoundCache::playChunk(std::string k, int loops)
{
    try {
        Mix_PlayChannel(-1, chunks.at(k), loops);
    } catch (std::out_of_range e) {
        fprintf(stderr, "KSoundCache::playChunk(%s): %s\n", k.c_str(), e.what());
    }
}

// class KLevel;

KLevel::~KLevel()
{
    for (auto iter = objects.begin(); iter != objects.end(); /* iter++ */) {
        delete *iter;
        iter = objects.erase(iter);
    }
}

void KLevel::handleEvent(SDL_Event &e)
{
    for (auto iter : objects) {
        iter->handleEvent(e);
    }
}

void KLevel::update()
{
    for (auto iter : objects) {
        iter->update();
    }
}

void KLevel::render(SDL_Renderer *r)
{
    for (auto iter : objects) {
        iter->render(r);     // TODO camera
    }
}

KNavigator::~KNavigator()
{
    while (!levels.empty()) {
        delete levels.back();
        levels.pop_back();
    }
}

void KNavigator::pushLevel(KLevel *l)
{
    levels.push_back(l);
}

void KNavigator::popLevel()
{
    if (!levels.empty()){
        delete levels.back();
        levels.pop_back();
    }
}

void KNavigator::changeLevel(KLevel *l)
{
    popLevel();
    pushLevel(l);
}

// class KGame;

KGame::KGame(const char *title, int w, int h, KRGBA rgba)
    : rgba(rgba)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL_Init(): %s\n", SDL_GetError());
    }

    // if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
    //     printf("Warning: Linear texture filtering not enabled.\n");
    // }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("SDL_CreateWindow(%s): %s\n", title, SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer(): %s\n", SDL_GetError());
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}

KGame::~KGame()
{
    SDL_DestroyRenderer(renderer); renderer = NULL;
    SDL_DestroyWindow(window);     window   = NULL;
}

bool KGame::handleEvents()
{
    ticks = SDL_GetTicks();     // tick();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
            return false;
        }
        if (!levels.empty()) {
            levels.back()->handleEvent(e);
        }
    }
    return true;
}

void KGame::update()
{
    if (!levels.empty()) {
        levels.back()->update();
    }
}

void KGame::render()
{
    SDL_SetRenderDrawColor(renderer, rgba.r, rgba.g, rgba.b, rgba.a);
    SDL_RenderClear(renderer);

    // SDL_Surface *surface = SDL_GetWindowSurface(window);
    // SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0x00, 0x00));
    // SDL_UpdateWindowSurface(window);
    // SDL_Delay(2000);

    if (!levels.empty()) {
        levels.back()->render(renderer);
    }

    SDL_RenderPresent(renderer);

    // Uint32 delay = SDL_GetTicks() - ticks; // tock()
    // if (delay < SCREEN_MSPF) {
    //     SDL_Delay(SCREEN_MSPF - delay);
    // }
}
