#include "../include/fish.hpp"

#define SCREEN_W    640
#define SCREEN_H    480
#define SCREEN_FPS  60
#define SCREEN_MSPF (1000.f / SCREEN_FPS)

const char *splashFile = "resources/images/fishtitle_parts_fixed.png";
const char *backgroundFile = "resources/images/fishbackground_parts_fixed.png";
const char *badFishFile = "resources/images/fishbaddie_parts_fixed.png";
const char *goodFishFile = "resources/images/fishsalmon_parts_fixed.png";

const char *fontKey = "lazyFont";
const char *fontFile = "resources/fonts/lazy.ttf";
const char *levelComplete = "levelComplete"; // TODO

const char *splashTexture = "splashTexture";
const char *backgroundTexture = "backgroundTexture";
const char *badFishTexture = "badFishTexture";
const char *goodFishTexture = "goodFishTexture";

const KSpriteSheet ssTitle = { 2, { 6, 183, 335, 240 }, 0, 0, 0, 19, 0, 0, 335, 68, 3, 1 };
const SDL_Rect rImage = { 289,  12, 336, 140 };
const SDL_Rect rStart = { 405, 179, 122,  37 };
const SDL_Rect rDifficulty[HARD + 1][2] = { // CHECKED, UNCHECKED
    { { 135,   8, 120, 40 }, { 7,   8, 120, 40 } },
    { { 135,  58, 120, 40 }, { 7,  58, 120, 40 } },
    { { 135, 108, 120, 40 }, { 7, 108, 120, 40 } },
};

const SDL_Rect ssBackground[] = {
    { 17,   7, 501, 81 },
    { 17,  96, 501, 81 },
    { 17, 185, 501, 81 },
};
const SDL_Rect ssBadFish[BROWN + 1] = {
    {  17, 273, 95, 62 },
    { 122, 273, 95, 62 },
    { 227, 273, 95, 62 },
    { 334, 273, 95, 62 },
    {  17, 341, 95, 62 },
    { 122, 341, 95, 62 },
};
const SDL_Rect ssScore = { 524, 113, 110, 53 };
const SDL_Rect ssGameOver = { 225, 344, 205, 117 };
// const KSpriteSheet ssStopLight     = { { 525, 34, 106, 25 }, 0, 0, 2, 0, 1, 1, 23, 23, 1, 4 };
// const KSpriteSheet ssShield        = { { 525,  7, 106, 25 }, 0, 0, 2, 0, 1, 1, 23, 23, 1, 4 };
// const KSpriteSheet ssEmptyBubble   = { { 525, 61,  79, 25 }, 0, 0, 2, 0, 1, 1, 23, 23, 1, 3 };
// const KSpriteSheet ssEmptyBUbbleSm = { { 525, 88,  55, 17 }, 0, 0, 2, 0, 1, 1, 15, 15, 1, 3 };

const KSpriteSheet ssBaddies[] = {
    { 1, {   5,   5, 215, 127 }, 0, 0, 3, 2, 1, 1,  39, 39, 3, 5 },
    { 1, {   5, 135, 189, 114 }, 0, 0, 1, 1, 1, 1,  35, 35, 3, 5 },
    { 1, {   5, 252, 173, 103 }, 0, 0, 2, 2, 1, 1,  31, 31, 3, 5 },
    { 1, {   5, 361, 161,  93 }, 0, 0, 4, 3, 1, 1,  27, 27, 3, 5 },
    { 1, { 236,   3, 137,  78 }, 0, 0, 3, 1, 1, 1,  23, 23, 3, 5 },
    { 1, { 387,   3, 121,  67 }, 0, 0, 4, 2, 1, 1,  19, 19, 3, 5 },
    { 1, { 215, 340,  55,  17 }, 0, 0, 2, 1, 1, 1,  15, 15, 2, 3 }, // RED
    { 1, { 216, 174, 303, 228 }, 0, 0, 5, 3, 1, 1, 147, 72, 3, 2 }, // SHARK
    { 1, { 236,  90, 277,  31 }, 0, 0, 3, 0, 1, 1,  65, 29, 1, 4 }, // OYSTER
    { 1, { 236, 127, 163,  37 }, 0, 0, 5, 0, 1, 1,  35, 35, 1, 4 }, // STAR
};

const KSpriteSheet ssGoodFish[] = {
    { 1, {  37,  28,  93,  57 }, 0, 0, 2, 3, 1, 1, 15, 15, 3, 5 },
    { 1, { 141,  27, 113,  67 }, 0, 0, 2, 2, 1, 1, 19, 19, 3, 5 },
    { 1, { 277,  23, 133,  81 }, 0, 0, 2, 3, 1, 1, 23, 23, 3, 5 },
    { 1, { 432,  20, 157,  93 }, 0, 0, 3, 3, 1, 1, 27, 27, 3, 5 },
    { 1, {  37, 115, 185, 105 }, 0, 0, 5, 3, 1, 1, 31, 31, 3, 5 },
    { 1, { 264, 116, 201, 119 }, 0, 0, 4, 4, 1, 1, 35, 35, 3, 5 },
    { 1, {  37, 250, 221, 127 }, 0, 0, 4, 2, 1, 1, 39, 39, 3, 5 },
    { 1, { 282, 253,  96,  45 }, 0, 0, 4, 3, 1, 1, 19, 19, 2, 4 }, // 7. DEATH
};

void checkBounds(KVector &position, KVector &velocity, KSpriteSheet &spriteSheet, SDL_RendererFlip &flip)
{
    if (position.getX() < 0) {
        position.setX(0);
        velocity.setX(velocity.getX() * -1);
    } else if (position.getX() + spriteSheet.spriteW > SCREEN_W) {
        position.setX(SCREEN_W - spriteSheet.spriteW);
        velocity.setX(velocity.getX() * -1);
    }
    if (position.getY() < 0) {
        position.setY(0);
        velocity.setY(velocity.getY() * -1);
    } else if (position.getY() + spriteSheet.spriteH > SCREEN_H) {
        position.setY(SCREEN_H - spriteSheet.spriteH);
        velocity.setY(velocity.getY() * -1);
    }

    flip = velocity.getX() > 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
}

// class BadFish;

void BadFish::update()
{
    position += velocity;
    checkBounds(position, velocity, spriteSheet, flip);
}

void BadFish::render(SDL_Renderer *r)
{
    if (!dead) {                // TODO remove from objects
        KSprite::render(r);
    }
}

// class GoodFish;

void GoodFish::handleEvent(SDL_Event &e)
{
    int magnitude = 2;
    if (e.key.repeat) { magnitude += 3; } // accelerate

    // handleKeyboardEvent(SDL_GetKeyboardState)
    const Uint8 *keyStates = SDL_GetKeyboardState(NULL);
    if (keyStates[SDL_SCANCODE_LEFT]) {
        velocity.setX(-magnitude);
    } else if (keyStates[SDL_SCANCODE_DOWN]) {
        velocity.setY(+magnitude);
    } else if (keyStates[SDL_SCANCODE_UP]) {
        velocity.setY(-magnitude);
    } else if (keyStates[SDL_SCANCODE_RIGHT]) {
        velocity.setX(+magnitude);
    } else if (keyStates[SDL_SCANCODE_RETURN]) {
        velocity.setXY(0, 0);
    }
}

void GoodFish::update()
{
    // velocity += acceleration;
    position += velocity;
    checkBounds(position, velocity, spriteSheet, flip);
}

void GoodFish::render(SDL_Renderer *r)
{
    if (lives > 0) {
        KSprite::render(r);
    }
}

// class FishLevel;

FishLevel::FishLevel(KNavigator *n, Difficulty d) : KLevel(n), difficulty(d)
{                               // pass in external player?
    dying = false;
    complete = new KTexture(levelComplete);
    deadFish = new GoodFish(goodFishTexture, ssGoodFish[7], 320, 240);
    deadFish->setVelocity(0, -3);

    paused = false;
    // totalFishes = badFishes + eatFishes + bgFishes?
    int nFishes = 0, nBadFishes = 0, kBadFishes = 0, pSize = 0;
    KVector velocity;

    switch (difficulty) {       // plus levelNumber
    case EASY:                  // ratio fishes :: (badFish + shark)
        nFishes = 10;
        nBadFishes = 1;         // % of nFishes?
        velocity.setXY(rand() % 2 ? 1 : -1, 0);

        pSize = 5;
        break;
    case MEDIUM:
        nFishes = 15;
        nBadFishes = 2;
        velocity.setXY(rand() % 2 ? 2 : -2, 1.0f / (rand() % 25 + 1) * (rand() % 2 ? 1 : -1));

        pSize = 2;
        break;
    case HARD:
        nFishes = 20;
        nBadFishes = 5;
        velocity.setXY(rand() % 2 ? 2 : -2, rand() % 2 ? 1 : -1);

        pSize = 0;
        break;
    }

    numtoeat = 0;
    badFish = (FishType) (rand() % (BROWN + 1));
    int x, y, c;

    objects.push_back(new KTexture(backgroundTexture, ssBackground[2], true));
    for (int i = 0; i < 3; i++) { // OYSTERS and STARS
        x = rand() % SCREEN_W, y = rand() % SCREEN_H, c = OYSTER + rand() % 2;
        objects.push_back(new KSprite(badFishTexture, ssBaddies[c], x, SCREEN_H - 50));
    }
    for (int i = 0; i < nFishes; i++) {
        x = rand() % SCREEN_W, y = rand() % SCREEN_H, c = (rand() % (STAR + 1));
        if (c == badFish) { kBadFishes++; }
        if (c >= SHARK) { kBadFishes++; }
        if (c > SHARK) { c = badFish; }
        BadFish *b = new BadFish(badFishTexture, ssBaddies[c], (FishType) c, x, y);
        b->setVelocity(velocity.getX(), velocity.getY());
        objects.push_back(b);   // render
        fishes.push_back(b);    // collision
    }
    numtoeat = nFishes - kBadFishes;

    player = new GoodFish(goodFishTexture, ssGoodFish[pSize], 320, 240);
    objects.push_back(player);
    objects.push_back(new KTexture(backgroundTexture, ssScore, false, 0, 8));
    objects.push_back(new KTexture(backgroundTexture, ssBadFish[badFish], false, SCREEN_W - ssBadFish[badFish].w, 8));
}

FishLevel::~FishLevel()
{
    delete deadFish;
    delete complete;
}

void FishLevel::handleEvent(SDL_Event &e)
{
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
        paused = !paused;
    }
    KLevel::handleEvent(e);
}

void FishLevel::update()
{
    if (paused) {
        if (dying) {
            deadFish->update();
        }
        return;
    }

    KLevel::update();

    handleCollision();

    if (player->lives <= 0) {
        printf("Game Over\n");
        paused = dying = true;
        deadFish->setPosition(player->getX(), player->getY());
    }
    if (numtoeat == 0) {
        paused = true;
        printf("Level complete\n");
    }
}

bool FishLevel::handleCollision()
{
    SDL_Rect playerRect = player->getRect();
    for (auto iter = fishes.begin(); iter != fishes.end(); /* iter++ */) {
        BadFish *fish = *iter;
        SDL_Rect fishRect = fish->getRect();
        if (SDL_HasIntersection(&playerRect, &fishRect)) { // TODO shrink rect
            switch (fish->color) {
            case YELLOW: printf("YELLOW"); break;
            case PURPLE: printf("PURPLE"); break;
            case BLUE:   printf("BLUE");   break;
            case GRAY:   printf("GRAY");   break;
            case GREEN:  printf("GREEN");  break;
            case BROWN:  printf("BROWN");  break;
            case RED:    printf("RED");  break;
            case SHARK:  printf("SHARK");  break;
            default: break;
            } printf(" fish got eaten!\n");

            if (fish->color == badFish) {
                player->lives--;
                printf("Ate bad fish, you die! Lives left = %d\n", player->lives);
            } else if (fish->color == SHARK) {
                player->lives--;
                printf("Cant eat a shark, you die! Lives left = %d\n\n", player->lives);
            } else {
                numtoeat--;
                printf("numtoeat = %d\n", numtoeat);
            }
            // if (fish->color != SHARK) { // TODO stuck behind shark...
            fish->dead = true;
            iter = fishes.erase(iter);
            // delete fish;
            // }
        } else {
            iter++;
        }
    }
    return false;
}

void FishLevel::render(SDL_Renderer *r)
{
    // if (!paused) {
    KLevel::render(r);
    // }

    if (paused && dying) {
        // onTimeout, delete death
        deadFish->render(r);
        if (deadFish->getY() == 0) {
            navigator->changeLevel(new FishGameOver(navigator));
        }
    }

    if (paused && numtoeat == 0) {
        int x = (SCREEN_W - complete->getW()) / 2, y = (SCREEN_H - complete->getH()) / 2;
        KTextureCache::getInstance()->renderTexture(r, levelComplete, x, y);
        if (timeout == 0) {
            timeout = SDL_GetTicks() + 2000;
        } else if (timeout < SDL_GetTicks()) {
            timeout = 0;
            navigator->changeLevel(new FishLevel(navigator, difficulty));
            // TODO player state needs to persist across levels
            // TODO pass next level state
        }
    }
}

// class FishGameOver;

FishGameOver::FishGameOver(KNavigator *n) : KLevel(n)
{
    objects.push_back(new KTexture(backgroundTexture, ssBackground[1], true));
    objects.push_back(new KTexture(backgroundTexture, ssGameOver, false,
                                   (SCREEN_W - ssGameOver.w) / 2,
                                   (SCREEN_H - ssGameOver.h) / 2));
    int x = (SCREEN_W - ssGameOver.w) / 2 + 41;
    int y = (SCREEN_H - ssGameOver.h) / 2 + 62;
    objects.push_back(new KButton("newGameButton", this, backgroundTexture, { 266, 405, 124, 41 }, x, y));
}

void FishGameOver::handleEvent(SDL_Event &e)
{
    KLevel::handleEvent(e);
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
        navigator->changeLevel(new FishLevel(navigator/* , difficulty */));
    }
}

void FishGameOver::onMouseButtonClick(std::string source)
{
    if (source.compare("newGameButton") == 0) {
        navigator->changeLevel(new FishLevel(navigator/* , difficulty */));
    }
}

// class FishSplash;

FishSplash::FishSplash(KNavigator *n) : KLevel(n)
{
    difficulty = EASY;

    int xpad = 5, ypad = 20;

    int height = ssTitle.spriteH + rImage.h + rDifficulty[EASY][0].h + rStart.h + 3 * ypad;
    height = (SCREEN_H - height) / 2;

    int x = (SCREEN_W - ssTitle.region.w) / 2, y = height;
    objects.push_back(new KSprite(splashTexture, ssTitle, x, y));

    x = (SCREEN_W - rImage.w) / 2, y += ssTitle.spriteH + ypad;
    objects.push_back(new KTexture(splashTexture, rImage, false, x, y));

    x = (SCREEN_W - (3 * rDifficulty[EASY][0].w + 2 * xpad)) / 2, y += rImage.h + ypad;
    buttons[EASY][0] = new KButton("easyButton", this, splashTexture, rDifficulty[EASY][0], x, y);
    buttons[EASY][1] = new KTexture(splashTexture, rDifficulty[EASY][1], false, x, y);
    objects.push_back(buttons[EASY][0]);
    objects.push_back(buttons[EASY][1]);

    x += xpad + rDifficulty[EASY][0].w;
    buttons[MEDIUM][0] = new KButton("mediumButton", this, splashTexture, rDifficulty[MEDIUM][0], x, y);
    buttons[MEDIUM][1] = new KTexture(splashTexture, rDifficulty[MEDIUM][1], false, x, y);
    objects.push_back(buttons[MEDIUM][0]);
    objects.push_back(buttons[MEDIUM][1]);

    x += xpad + rDifficulty[MEDIUM][0].w;
    buttons[HARD][0] = new KButton("hardButton", this, splashTexture, rDifficulty[HARD][0], x, y);
    buttons[HARD][1] = new KTexture(splashTexture, rDifficulty[HARD][1], false, x, y);
    objects.push_back(buttons[HARD][0]);
    objects.push_back(buttons[HARD][1]);

    x = (SCREEN_W - rStart.w) / 2, y += rDifficulty[EASY][0].h + ypad;
    objects.push_back(new KButton("startButton", this, splashTexture, rStart, x, y));
}

void FishSplash::handleEvent(SDL_Event &e)
{
    KLevel::handleEvent(e);
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
        navigator->changeLevel(new FishLevel(navigator, difficulty));
    }
}

void FishSplash::onMouseButtonClick(std::string source)
{
    if (source.compare("startButton") == 0) {
        navigator->changeLevel(new FishLevel(navigator, difficulty));
    } else if (source.compare("easyButton") == 0) {
        difficulty = EASY;
    } else if (source.compare("mediumButton") == 0) {
        difficulty = MEDIUM;
    } else if (source.compare("hardButton") == 0) {
        difficulty = HARD;
    }
}

void FishSplash::render(SDL_Renderer *r)
{
    KLevel::render(r);
    for (int i = 0; i < HARD + 1; i++){
        buttons[i][i == difficulty]->render(r);
    }
}

// class FishDish;

static void initResources(SDL_Renderer *r)
{
    KTextureCache *textures = KTextureCache::getInstance();
    KRGBA blackKey = { 0, 0, 0, 0, SDL_TRUE };
    KRGBA waterKey = { 0x66, 0xCC, 0xFF, 0xFF, SDL_TRUE};

    textures->loadTexture(r, splashTexture, splashFile, blackKey);
    textures->loadTexture(r, backgroundTexture, backgroundFile, waterKey);
    textures->loadTexture(r, badFishTexture, badFishFile, blackKey);
    textures->loadTexture(r, goodFishTexture, goodFishFile, blackKey);

    KFontCache::getInstance()->loadFont(fontKey, fontFile);
    textures->loadFontTexture(r, levelComplete, fontKey, "Level Complete", { 0xFF, 0, 0, 0xFF});
}

FishDish::FishDish(const char *title, int w, int h, KRGBA rgba)
    : KGame(title, w, h, rgba)
{
    initResources(renderer);
    pushLevel(new FishSplash(this));
}

FishDish::~FishDish()
{
    popLevel();
}
