#pragma once
#ifndef _KSDL_H_
#define _KSDL_H_

#include <map>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

// #define KTEXTURE_CENTERED -1
#define KSPRITE_HORIZONTAL 1
#define KSPRITE_VERTICAL   2

typedef struct KRGBA
{
    uint8_t r = 0x00;
    uint8_t g = 0x00;
    uint8_t b = 0x00;
    uint8_t a = 0xFF;

    int colorKeyFlag = SDL_FALSE;
} KRGBA;

typedef struct KSpriteSheet
{
    int axis;
    SDL_Rect region;
    int offsetX, offsetY;
    int marginX, marginY;
    int borderX, borderY;
    int spriteW, spriteH;
    int rows, cols;
} KSpriteSheet;

class KVector
{
public:
    KVector(float x = 0.f, float y = 0.f) : x(x), y(y) {}
    KVector(const KVector &) = default;
    // KVector(KVector &&) = default;
    KVector &operator=(const KVector &) = default;
    // KVector &operator=(KVector &&) = default;
    ~KVector() = default;

    float getX() const              { return x; }
    void  setX(float x)             { this->x = x; }
    float getY() const              { return y; }
    void  setY(float y)             { this->y = y; }
    void  getXY(float &x, float &y) { x = this->x, y = this->y; }
    void  setXY(float x, float y)   { this->x = x, this->y = y; }

    float magnitude() const;
    void normalize();

    KVector operator+(const KVector &) const;
    KVector operator-(const KVector &) const;
    KVector operator*(float) const;
    KVector operator/(float) const;
    // KVector &operator+=(const KVector &);
    // KVector &operator-=(const KVector &);
    KVector &operator*=(float);
    KVector &operator/=(float);

    friend KVector &operator+=(KVector &, const KVector &);
    friend KVector &operator-=(KVector &, const KVector &);
    // friend KVector &operator*=(KVector &, const KVector &);
    // friend KVector &operator/=(KVector &, const KVector &);

    friend std::ostream &operator<<(std::ostream &, KVector const &);
private:
    float x, y;
};

class KFontCache
{
public:
    static KFontCache *getInstance();

    TTF_Font *loadFont(std::string k, const char *file, int ptsize = 28);
    void unloadFont(std::string k);
    TTF_Font *getFont(std::string k);
private:
    KFontCache();
    ~KFontCache();
    // KFontCache(const KFontCache &) = delete;
    // KFontCache &operator=(const KFontCache &) = delete;

    static KFontCache *instance;

    std::map<std::string, TTF_Font *> fonts;
};

class KTextureCache
{
public:
    static KTextureCache *getInstance();

    SDL_Texture *loadTexture(SDL_Renderer *r, std::string k, const char *file, KRGBA rgb = {});
    SDL_Texture *loadFontTexture(SDL_Renderer *r, std::string k,
                                 std::string f, const char *text, SDL_Color fg = { 0, 0, 0, 0xFF});
    void unloadTexture(std::string k);
    SDL_Texture *getTexture(std::string k) const;
    void renderTexture(SDL_Renderer *r, std::string k, int x, int y, SDL_Rect *src = NULL,
                       double angle = 0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void renderSprite(SDL_Renderer *r, std::string k, int x, int y, int m, int N, int w, int h);
private:
    KTextureCache();
    ~KTextureCache();
    // KTextureCache(const KTextureCache &) = delete;
    // KTextureCache &operator=(const KTextureCache &) = delete;

    static KTextureCache *instance;

    // SDL_Renderer *renderer;
    std::map<std::string, SDL_Texture *> textures;
};

class KTexture
{
public:
    KTexture(std::string, SDL_Rect src = { 0, 0, 0, 0 }, bool bg = false, int x = 0, int y = 0);
    KTexture(const KTexture &) = default;
    virtual KTexture &operator=(const KTexture &) = default;
    virtual ~KTexture() = default;

    SDL_Texture *getTexture();
    void setPosition(float x, float y) { position.setXY(x, y); }
    int getX() const { return (int) position.getX(); }
    int getY() const { return (int) position.getY(); }
    int getW() const { return src.w; }
    int getH() const { return src.h; }
    SDL_Rect getRect() const { return { getX(), getY(), getW(), getH() }; }

    virtual void handleEvent(SDL_Event &) {}
    virtual void update() {}
    virtual void render(SDL_Renderer *);

    friend std::ostream &operator<<(std::ostream &, KTexture const &);
protected:
    std::string key;
    // SDL_Texture *texture;
    SDL_Rect src;
    KVector position;
    bool isBgTexture;
};

class KSprite : public KTexture
{
public:
    KSprite(std::string k, KSpriteSheet src, int x = 0, int y = 0)
        : KTexture(k, src.region, false, x, y)
        , spriteSheet(src)
        , velocity(0, 0)
        , acceleration(0, 0)
        {}
    virtual ~KSprite() = default;

    void setVelocity(float x, float y) { velocity.setXY(x, y); }
    void setAcceleration(float x, float y) { acceleration.setXY(x, y); }
    SDL_Rect getRect() const { return { getX(), getY(), spriteSheet.spriteW, spriteSheet.spriteH }; }

    // virtual void handleEvent(SDL_Event &);
    // virtual void update();
    virtual void render(SDL_Renderer *);
protected:
    KSpriteSheet spriteSheet;
    KVector velocity;
    KVector acceleration;

    // double angle = 0.0;
    // SDL_Point *center = NULL;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
};

class KButtonHandler
{
public:
    // virtual void onMouseMotion() = 0;
    // virtual void onMouseButtonDown() = 0;
    // virtual void onMouseButtonUp() = 0;
    virtual void onMouseButtonClick(std::string) = 0;
private:
    // KButtonHandler();
    // ~KButtonHandler();
};

class KButton : public KTexture
{
public:
    KButton(std::string s, KButtonHandler *h, std::string k, SDL_Rect src, int x = 0, int y = 0)
        : KTexture(k, src, false, x, y)
        , source(s)
        , handler(h)
        {}
    virtual ~KButton() = default;

    virtual void handleEvent(SDL_Event &);
    // virtual void update();
    // virtual void render(SDL_Renderer *);
protected:
    std::string source;
    KButtonHandler *handler;

    bool clicked = false;
};

class KSoundCache
{
public:
    static KSoundCache *getInstance();

    Mix_Music *loadMusic(std::string k, const char *file);
    Mix_Chunk *loadChunk(std::string k, const char *file);
    void unloadMusic(std::string k);
    void unloadChunk(std::string k);
    Mix_Music *getMusic(std::string k) const;
    Mix_Chunk *getChunk(std::string k) const;
    void playMusic(std::string k, int loops = -1);
    void playChunk(std::string k, int loops = 0);
private:
    KSoundCache();
    ~KSoundCache();
    // KSoundCache(const KSoundCache &) = delete;
    // KSoundCache &operator=(const KSoundCache &) = delete;

    static KSoundCache *instance;

    std::map<std::string, Mix_Chunk *> chunks;
    std::map<std::string, Mix_Music *> musics;
};

class KNavigator;

class KLevel
{
public:
    KLevel(KNavigator *n) : navigator(n) {}
    virtual ~KLevel();

    virtual void handleEvent(SDL_Event &);
    virtual void update();
    virtual void render(SDL_Renderer *);
protected:
    KNavigator *navigator;

    std::vector<KTexture *> objects;
    // std::vector<KSprite *> colliders;
};

class KNavigator
{
public:
    KNavigator() = default;
    ~KNavigator();

    virtual void pushLevel(KLevel *);
    virtual void popLevel();
    virtual void changeLevel(KLevel *);
protected:
    std::vector<KLevel *> levels;
};

class KGame : public KNavigator
{
public:
    KGame(const char *title = "KGame", int w = 640, int h = 480,
          KRGBA rgb = { 0x00, 0x00, 0x00, 0xFF });
    ~KGame();

    virtual bool handleEvents();
    virtual void update();
    virtual void render();
protected:
    SDL_Window *window;
    SDL_Renderer *renderer;

    KRGBA rgba;
    Uint32 ticks;
};

// class KKeyboardHandler
// {
// public:
//     virtual void onKeyDown() = 0;
//     virtual void onKeyUp() = 0;
// };

// class KCamera {};

#endif
