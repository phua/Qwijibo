#pragma once
#ifndef _FISH_H_
#define _FISH_H_

#include "ksdl.hpp"

enum Difficulty {
    EASY, MEDIUM, HARD
};

enum FishType {
    YELLOW, PURPLE, BLUE, GRAY, GREEN, BROWN,
    RED, SHARK,
    OYSTER, STAR
};

class BadFish : public KSprite
{
public:
    BadFish(std::string k, KSpriteSheet spriteSheet, FishType t, int x = 0, int y = 0) :
        KSprite(k, spriteSheet, x, y), color(t), dead(false) { }
    ~BadFish() = default;

    virtual void update();
    virtual void render(SDL_Renderer *);
// private:
    FishType color;
    bool dead;
};

class GoodFish : public KSprite
{
public:
    GoodFish(std::string k, KSpriteSheet spriteSheet, int x = 0, int y = 0) :
        KSprite(k, spriteSheet, x, y), lives(3) {}

    virtual void handleEvent(SDL_Event &);
    virtual void update();
    virtual void render(SDL_Renderer *);
// private:
    int lives, score;
};

class FishLevel : public KLevel
{
public:
    FishLevel(KNavigator *n, Difficulty d = EASY);
    virtual ~FishLevel();

    virtual void handleEvent(SDL_Event &);
    virtual void update();
    virtual void render(SDL_Renderer *);

    bool handleCollision();
private:
    // struct LevelState: nFishes, nBadFishesOrSharks, fishVelocity, nEatGoal, playerSize
    bool paused;
    Difficulty difficulty;
    FishType badFish;
    int numtoeat;

    // invulnerable, time

    // LevelObjects:
    GoodFish *player;
    GoodFish *deadFish;
    std::vector<BadFish *> fishes;

    bool dying;
    KTexture *complete;
    Uint32 timeout = 0;         // complete msg timeout
};

class FishGameOver : public KLevel, KButtonHandler
{
public:
    FishGameOver(KNavigator *n);
    ~FishGameOver() = default;

    virtual void handleEvent(SDL_Event &);
    virtual void onMouseButtonClick(std::string);
};

class FishSplash : public KLevel, KButtonHandler
{
public:
    FishSplash(KNavigator *n);
    ~FishSplash() = default;

    virtual void handleEvent(SDL_Event &);
    virtual void render(SDL_Renderer *);
    virtual void onMouseButtonClick(std::string);
private:
    KTexture *buttons[HARD + 1][2];

    Difficulty difficulty;
};

class FishDish : public KGame
{
public:
    FishDish(const char *title = "Fish Dish", int w = 640, int h = 480,
             KRGBA rgba = { 0x66, 0xCC, 0xFF, 0xFF });
    ~FishDish();
};

#endif
