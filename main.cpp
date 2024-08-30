#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <iostream>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BULLET_WIDTH 9
#define BULLET_HEIGHT 36
#define PLAYER_WIDTH 30
#define PLAYER_HEIGHT 64
#define COMBO_TIME 4
#define NAME_LENGTH 70
#define PLAYER_BULLET_AMOUNT 100
#define BULLET_AMOUNT 100
#define ENEMY_AMOUNT 15
#define POTIONS_AMOUNT 8
#define FIRE_BULLETS_AMOUNT 8
#define EXPLOSIVES_AMOUNT 50
#define TRAPS_AMOUNT 12
#define ENEMY_MAX_HP 3
#define LEFT_BORDER 775
#define RIGHT_BORDER -135
#define TOP_BORDER -1713
#define BOTTOM_BORDER 2361
#define BG_LEFT_BORDER 789
#define BG_RIGHT_BORDER -156
#define BG_TOP_BORDER 2027
#define BG_BOTTOM_BORDER -1510

struct SDL_LIB {
    SDL_Event event;
    SDL_Surface* screen, * charset;
    SDL_Surface* bullet;
    SDL_Texture* scrtex;
    SDL_Window* window;
    SDL_Renderer* renderer;
};

struct gamePhysics {
    int t1, t2, t3, quit, frames, rc, level, playerPoints = 0, enemyPoints = 0, playerScore = 0, shotsRow = 0, it = 0, damageTaken = 0;
    bool ifStarted = false, menu = true, gameOver = false, invincibility = false, nextLevel = true, canShoot = false, newGameStarted = false, display = false, explosionDisplay[2][EXPLOSIVES_AMOUNT];
    double delta, worldTime, fpsTimer, fps, distance, speed, bulletSpeed, t4, moveTimeStamp = 0.0, displayTime = 0.0, explosionTime = 0.0, comboTime = 0.0;
    int enemyHP[ENEMY_AMOUNT];
    bool getTime[2] = { true, true };
    bool hit[ENEMY_AMOUNT];
    const double bonus = 0.2;
};

struct Potion {
    SDL_Rect hitbox;
    bool isPotion = false;
    double appearingTime = 0.0;
};

struct Objects {
    SDL_Rect hitbox;
    bool isExplosive = false, isTrap = false, onTrap[2] = { false, false };
    double appearingTime = 0.0;
    double timeOnTrap = 0.0;
    bool isHit = false;
    double hitTime = 0.0;
};

struct Bullet {
    SDL_Rect hitbox;
    bool shoot = false;
    bool isVertical = true;
    int shootDirection = 0;
    bool isBig = false;
    int bulletType;
    double distance = 0;
};

struct Player {
    int x = SCREEN_WIDTH / 2, y = SCREEN_HEIGHT - 70;
    double bulletSpeed = 600;
    SDL_Surface* eti[4][4];
    SDL_Surface* blueLaser;
    Bullet bullets[BULLET_AMOUNT];
    int walkDirection = 0;
    double walkStartTime = 0;
    double shotTime = 0;
};

struct Enemy {
    SDL_Rect hitbox;
    Bullet bullets[BULLET_AMOUNT];
    bool alive = true;
    double shotingTime = 0.0;
    double distanceToTravel = 0.0;
    bool toTheLeft = true;
};

struct Background {
    SDL_Surface* background;
    int x = SCREEN_WIDTH / 2, y = SCREEN_HEIGHT / 2;
};

struct Surfaces {
    SDL_Surface* greenLaser;
    SDL_Surface* blueLaser[2];
    SDL_Surface* purpleLaser;
    SDL_Surface* redLaser;
    SDL_Surface* opponent;
    SDL_Surface* opponent2;
    SDL_Surface* opponent3[2][6];
    SDL_Surface* healthBar;
    SDL_Surface* deathBar;
    SDL_Surface* healthPot;
    SDL_Surface* Explosive;
    SDL_Surface* fireBullet;
    SDL_Surface* trap[2];
    SDL_Surface* fire;
    SDL_Surface* bigBullet;
    SDL_Surface* bigBullet2;
    SDL_Surface* bigBullet3;
    SDL_Surface* bigBullet4;
    SDL_Surface* gradeAnimationAsc[6];
    SDL_Surface* gradeAnimationDesc[18];
    SDL_Surface* explosionAnimation[7];
};

struct bullets {
    int x = SCREEN_WIDTH / 2;
    int y = 100;
    int none = 0;
    double bulletSpeed = 300;
};

struct Score {
    int score;
    char nickname[NAME_LENGTH];
    int gradeNumber = 0;
    int scoreGrade = 0;
    char grade[10];
};

void DrawString(SDL_Surface* screen, int x, int y, const char* text,
    SDL_Surface* charset) {
    int px, py, c;
    SDL_Rect s, d;
    s.w = 8;
    s.h = 8;
    d.w = 8;
    d.h = 8;
    while (*text) {
        c = *text & 255;
        px = (c % 16) * 8;
        py = (c / 16) * 8;
        s.x = px;
        s.y = py;
        d.x = x;
        d.y = y;
        SDL_BlitSurface(charset, &s, screen, &d);
        x += 8;
        text++;
    };
};


void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
    if (sprite == nullptr) {
        printf("Failed to load sprite");
        return;
    }
    SDL_Rect dest;
    dest.x = x - sprite->w / 2;
    dest.y = y - sprite->h / 2;
    dest.w = sprite->w;
    dest.h = sprite->h;
    SDL_BlitSurface(sprite, NULL, screen, &dest);
}


void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
    *(Uint32*)p = color;
};

void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
    for (int i = 0; i < l; i++) {
        DrawPixel(screen, x, y, color);
        x += dx;
        y += dy;
    };
};

void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
    Uint32 outlineColor, Uint32 fillColor) {
    int i;
    DrawLine(screen, x, y, k, 0, 1, outlineColor);
    DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
    DrawLine(screen, x, y, l, 1, 0, outlineColor);
    DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
    for (i = y + 1; i < y + k - 1; i++)
        DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

int ErrorFree(SDL_LIB* lib, SDL_Surface* surface) {
    if (surface == NULL) {
        SDL_FreeSurface(lib->charset);
        SDL_FreeSurface(lib->screen);
        SDL_DestroyTexture(lib->scrtex);
        SDL_DestroyWindow(lib->window);
        SDL_DestroyRenderer(lib->renderer);
        SDL_Quit();
        return 1;
    };
}

void FPS_Check(gamePhysics* game) {
    game->fpsTimer += game->delta;
    if (game->fpsTimer > 0.5) {
        game->fps = static_cast<int>(static_cast<long long>(game->frames) * 2);
        game->frames = 0;
        game->fpsTimer -= 0.5;
    };
}

bool CanPlaceObject(Enemy enemy[], Potion  potion[], Objects traps1[], Objects traps2[], Objects explosives[], int i, bool checkEnemy, bool checkPotions, bool checkTraps1, bool checkTraps2, bool checkExplosives) {
    if (checkEnemy) {
        for (int j = 0; j < i; j++) {
            if (SDL_HasIntersection(&enemy[i].hitbox, &enemy[j].hitbox))
                return false;
        }
    }
    else if (checkPotions) {
        for (int j = 0; j < ENEMY_AMOUNT; j++) {
            if (SDL_HasIntersection(&potion[i].hitbox, &enemy[j].hitbox))
                return false;
        }
        for (int j = 0; j < i; j++) {
            if (SDL_HasIntersection(&potion[i].hitbox, &potion[j].hitbox))
                return false;
        }
    }
    else if (checkTraps1)
    {
        for (int j = 0; j < ENEMY_AMOUNT; j++) {
            if (SDL_HasIntersection(&traps1[i].hitbox, &enemy[j].hitbox))
                return false;
        }
        for (int j = 0; j < POTIONS_AMOUNT; j++) {
            if (SDL_HasIntersection(&traps1[i].hitbox, &potion[j].hitbox))
                return false;
        }
        for (int j = 0; j < i; j++) {
            if (SDL_HasIntersection(&traps1[i].hitbox, &traps1[j].hitbox))
                return false;
        }
    }
    else if (checkTraps2) {
        for (int j = 0; j < ENEMY_AMOUNT; j++) {
            if (SDL_HasIntersection(&traps2[i].hitbox, &enemy[j].hitbox))
                return false;
        }
        for (int j = 0; j < POTIONS_AMOUNT; j++) {
            if (SDL_HasIntersection(&traps2[i].hitbox, &potion[j].hitbox))
                return false;
        }
        for (int j = 0; j < TRAPS_AMOUNT; j++) {
            if (SDL_HasIntersection(&traps2[i].hitbox, &traps1[j].hitbox))
                return false;
        }
        for (int j = 0; j < i; j++) {
            if (SDL_HasIntersection(&traps2[i].hitbox, &traps2[j].hitbox))
                return false;
        }
    }
    else {
        for (int j = 0; j < ENEMY_AMOUNT; j++) {
            if (SDL_HasIntersection(&explosives[i].hitbox, &enemy[j].hitbox))
                return false;
        }
        for (int j = 0; j < POTIONS_AMOUNT; j++) {
            if (SDL_HasIntersection(&explosives[i].hitbox, &potion[j].hitbox))
                return false;
        }
        for (int j = 0; j < TRAPS_AMOUNT; j++) {
            if (SDL_HasIntersection(&explosives[i].hitbox, &traps1[j].hitbox))
                return false;
        }
        for (int j = 0; j < TRAPS_AMOUNT; j++) {
            if (SDL_HasIntersection(&explosives[i].hitbox, &traps2[j].hitbox))
                return false;
        }
        for (int j = 0; j < i; j++) {
            if (SDL_HasIntersection(&explosives[i].hitbox, &explosives[j].hitbox))
                return false;
        }
    }

    return true;
}

void SetEnemy(Enemy enemy[], gamePhysics& Game, Potion potion[], Objects trap[2][TRAPS_AMOUNT], Objects explosives[]) {
    for (int i = 0; i < ENEMY_AMOUNT; i++) {
        bool canPlace = false;
        enemy[i].alive = true;
        enemy[i].hitbox.w = 60;
        enemy[i].hitbox.h = 60;
        if (Game.level == 3) {
            enemy[i].hitbox.w = 54;
            enemy[i].hitbox.h = 64;
        }
        while (canPlace == false) {
            enemy[i].hitbox.x = rand() % 710 - 75;
            enemy[i].hitbox.y = (rand() % 3400 - 1500);
            canPlace = CanPlaceObject(enemy, potion, trap[0], trap[1], explosives, i, true, false, false, false, false);
        }
        enemy[i].shotingTime = ((rand() % 300) / 100.0) + 1.0;

        if (Game.level == 3) {
            enemy[i].distanceToTravel = ((rand() % 400) / 100.0 + 1.0);
            enemy[i].toTheLeft = rand() % 2;
        }

        for (int j = 0; j < BULLET_AMOUNT; j++) {
            if (Game.level != 3) {
                enemy[i].bullets[j].hitbox.x = enemy[i].hitbox.x + enemy[i].hitbox.w / 2 - 10;
                enemy[i].bullets[j].hitbox.y = enemy[i].hitbox.y + enemy[i].hitbox.h / 2 - 10;
            }
            else {
                enemy[i].bullets[j].hitbox.x = enemy[i].hitbox.x + enemy[i].hitbox.w / 2;
                enemy[i].bullets[j].hitbox.y = enemy[i].hitbox.y + enemy[i].hitbox.h / 2;
            }
            enemy[i].bullets[j].hitbox.w = 9;
            enemy[i].bullets[j].hitbox.h = 36;
            enemy[i].bullets[j].bulletType = rand() % 2;
            enemy[i].bullets[j].distance = 0;
        }
    }
}

void SetPlayer(Player& player) {
    player.shotTime = 0;
    for (int j = 0; j < PLAYER_BULLET_AMOUNT; j++) {
        player.bullets[j].hitbox.x = SCREEN_WIDTH / 2;
        player.bullets[j].hitbox.y = SCREEN_HEIGHT - 70;
        player.bullets[j].hitbox.w = 9;
        player.bullets[j].hitbox.h = 36;
        player.bullets[j].shoot = false;
    }
}

void SetPotions(Potion  potion[], Enemy  enemy[], Objects  trap[2][TRAPS_AMOUNT], Objects explosives[]) {
    for (int i = 0; i < POTIONS_AMOUNT; i++) {
        potion[i].isPotion = false;
        bool canPlace = false;
        potion[i].hitbox.w = 30;
        potion[i].hitbox.h = 44;
        while (canPlace == false) {
            potion[i].hitbox.x = rand() % 710 - 75;
            potion[i].hitbox.y = (rand() % 3400 - 1500);
            canPlace = CanPlaceObject(enemy, potion, trap[0], trap[1], explosives, i, false, true, false, false, false);
        }
        potion[i].appearingTime = ((rand() % 4000) / 100.0) + 1.0;
    }
}

void SetExplosives(Objects explosives[], Enemy enemy[], Potion potion[], Objects trap[2][TRAPS_AMOUNT], Objects fireBullets[EXPLOSIVES_AMOUNT][FIRE_BULLETS_AMOUNT]) {
    for (int i = 0; i < EXPLOSIVES_AMOUNT; i++) {
        explosives[i].hitTime = 0;
        explosives[i].isHit = false;
        explosives[i].isExplosive = false;
        bool canPlace = false;
        explosives[i].hitbox.w = 60;
        explosives[i].hitbox.h = 60;
        while (canPlace == false) {
            explosives[i].hitbox.x = rand() % 710 - 75;
            explosives[i].hitbox.y = (rand() % 3400 - 1500);
            canPlace = CanPlaceObject(enemy, potion, trap[0], trap[1], explosives, i, false, false, false, false, true);
        }
        explosives[i].appearingTime = ((rand() % 4000) / 100.0) + 1.0;
        for (int j = 0; j < FIRE_BULLETS_AMOUNT; j++) {
            fireBullets[i][j].isExplosive = false;
            fireBullets[i][j].hitbox.w = 16;
            fireBullets[i][j].hitbox.h = 16;
            fireBullets[i][j].hitbox.x = explosives[i].hitbox.x + explosives[i].hitbox.w / 2;
            fireBullets[i][j].hitbox.y = explosives[i].hitbox.y + explosives[i].hitbox.h / 2;
        }
    }
}

void SetTraps(Objects trap[2][TRAPS_AMOUNT], Enemy enemy[], Potion potion[], Objects explosives[], int trapType, int w, int h) {

    for (int i = 0; i < TRAPS_AMOUNT; i++) {
        trap[trapType][i].isTrap = false;
        bool canPlace = false;
        trap[trapType][i].hitbox.w = w;
        trap[trapType][i].hitbox.h = h;
        while (canPlace == false) {
            trap[trapType][i].hitbox.x = rand() % 710 - 75;
            trap[trapType][i].hitbox.y = (rand() % 3400 - 1500);
            if (trapType == 0)
                canPlace = CanPlaceObject(enemy, potion, trap[0], trap[1], explosives, i, false, false, true, false, false);
            else
                canPlace = CanPlaceObject(enemy, potion, trap[0], trap[1], explosives, i, false, false, false, true, false);

        }
        trap[trapType][i].appearingTime = ((rand() % 4000) / 100.0) + 1.0;
    }
}

void PlayerShoot(Bullet bullet, SDL_Surface* laser[2], SDL_Rect* pBullet, SDL_LIB* lib) {
    DrawSurface(lib->screen, laser[bullet.isVertical], pBullet->x, pBullet->y);

    if (bullet.shootDirection == 0)
        pBullet->y -= 7;
    else if (bullet.shootDirection == 1)
        pBullet->x -= 7;
    else if (bullet.shootDirection == 2)
        pBullet->x += 7;
    else
        pBullet->y += 7;
}

void EnemyShoot(gamePhysics game, bullets* bullet, Bullet& enemyBullet, double& speed, SDL_Surface* screen, Surfaces BS, SDL_Rect* eBullet) {
    int x = eBullet->x;
    int y = eBullet->y;

    if (enemyBullet.bulletType == 1 && game.level == 1) {
        x = eBullet->x + sin(enemyBullet.distance) * SCREEN_WIDTH / 15;
        y = eBullet->y + cos(enemyBullet.distance) * SCREEN_HEIGHT / 15;
        enemyBullet.distance += 3 * game.delta;
    }
    else if (enemyBullet.bulletType == 1 && game.level == 2) {
        x = eBullet->x + sin(enemyBullet.distance) * SCREEN_WIDTH / 13 + enemyBullet.distance;
        y = eBullet->y + sin(enemyBullet.distance) * SCREEN_HEIGHT / 13 - enemyBullet.distance;
        enemyBullet.distance += 3 * game.delta;
    }
    else if (enemyBullet.bulletType == 1 && game.level == 3) {
        x = eBullet->x + cos(enemyBullet.distance) * SCREEN_WIDTH / 35 - enemyBullet.distance * 2;
        y = eBullet->y + cos(enemyBullet.distance) * SCREEN_HEIGHT / 35 - enemyBullet.distance;
        enemyBullet.distance += 3 * game.delta;
    }

    if (enemyBullet.isBig && game.level == 1)
        DrawSurface(screen, BS.bigBullet4, x, y);
    if (enemyBullet.isBig && game.level == 2)
        DrawSurface(screen, BS.bigBullet, x, y);
    if (enemyBullet.isBig && game.level == 3)
        DrawSurface(screen, BS.bigBullet3, x, y);
    if (!enemyBullet.isBig && game.level == 1)
        DrawSurface(screen, BS.purpleLaser, x, y);
    if (!enemyBullet.isBig && game.level == 2)
        DrawSurface(screen, BS.greenLaser, x, y);
    if (!enemyBullet.isBig && game.level == 3)
        DrawSurface(screen, BS.redLaser, x, y);

    eBullet->y += speed * game.delta;
}

void MoveEnemy(gamePhysics game, Enemy& enemy) {

    if (game.level != 3)
        return;

    if (enemy.distanceToTravel < game.worldTime) {
        enemy.distanceToTravel = ((rand() % 400) / 100.0 + 1.0) + game.worldTime;
        enemy.toTheLeft = rand() % 2;
    }
    if (enemy.toTheLeft) {
        if (enemy.hitbox.x - 7 <= RIGHT_BORDER) {
            enemy.distanceToTravel = ((rand() % 400) / 100.0 + 1.0) + game.worldTime;
            enemy.toTheLeft = false;
        }
        else
            enemy.hitbox.x -= 7;
    }
    else {
        if (enemy.hitbox.x + 7 >= LEFT_BORDER) {
            enemy.distanceToTravel = ((rand() % 400) / 100.0 + 1.0) + game.worldTime;
            enemy.toTheLeft = true;
        }
        else
            enemy.hitbox.x += 7;
    }
}

void LoadSurfaces(Background& bg, Surfaces& Surface, Player& player, SDL_LIB& lib) {
    char text[128];
    bg.background = SDL_LoadBMP("./assets/game/background1.bmp");
    Surface.opponent = SDL_LoadBMP("./assets/game/spaceEnemy.bmp");
    Surface.opponent2 = SDL_LoadBMP("./assets/game/spaceEnemy2.bmp");
    Surface.greenLaser = SDL_LoadBMP("./assets/game/bullet.bmp");
    Surface.blueLaser[0] = SDL_LoadBMP("./assets/game/bullet2_horizontal.bmp");
    Surface.blueLaser[1] = SDL_LoadBMP("./assets/game/bullet2.bmp");
    Surface.redLaser = SDL_LoadBMP("./assets/game/bullet3.bmp");
    Surface.purpleLaser = SDL_LoadBMP("./assets/game/bullet4.bmp");
    Surface.healthBar = SDL_LoadBMP("./assets/game/health_bar.bmp");
    Surface.deathBar = SDL_LoadBMP("./assets/game/dead.bmp");
    Surface.healthPot = SDL_LoadBMP("./assets/game/Health-potion.bmp");
    Surface.Explosive = SDL_LoadBMP("./assets/game/TNT.bmp");
    Surface.fireBullet = SDL_LoadBMP("./assets/game/fire_bullet.bmp");
    Surface.trap[0] = SDL_LoadBMP("./assets/game/trap.bmp");
    Surface.trap[1] = SDL_LoadBMP("./assets/game/wall.bmp");
    Surface.fire = SDL_LoadBMP("./assets/game/fire.bmp");
    Surface.bigBullet = SDL_LoadBMP("./assets/game/big_bullet.bmp");
    Surface.bigBullet2 = SDL_LoadBMP("./assets/game/big_bullet2.bmp");
    Surface.bigBullet3 = SDL_LoadBMP("./assets/game/big_bullet3.bmp");
    Surface.bigBullet4 = SDL_LoadBMP("./assets/game/big_bullet4.bmp");

    for (int i = 0; i < 4; i++) {
        sprintf(text, "./animations/player/up%d.bmp", i + 1);
        player.eti[0][i] = SDL_LoadBMP(text);
        ErrorFree(&lib, player.eti[0][i]);

        sprintf(text, "./animations/player/left%d.bmp", i + 1);
        player.eti[1][i] = SDL_LoadBMP(text);
        ErrorFree(&lib, player.eti[1][i]);

        sprintf(text, "./animations/player/right%d.bmp", i + 1);
        player.eti[2][i] = SDL_LoadBMP(text);
        ErrorFree(&lib, player.eti[2][i]);

        sprintf(text, "./animations/player/down%d.bmp", i + 1);
        player.eti[3][i] = SDL_LoadBMP(text);
        ErrorFree(&lib, player.eti[3][i]);
    }

    for (int i = 0; i < 6; i++) {
        sprintf(text, "./animations/grade_animation/frame_%d.bmp", i + 1);
        Surface.gradeAnimationAsc[i] = SDL_LoadBMP(text);
        ErrorFree(&lib, Surface.gradeAnimationAsc[i]);
        sprintf(text, "./animations/enemy_animation/left_%d.bmp", i + 1);
        Surface.opponent3[1][i] = SDL_LoadBMP(text);
        ErrorFree(&lib, Surface.opponent3[1][i]);
        sprintf(text, "./animations/enemy_animation/right_%d.bmp", i + 1);
        Surface.opponent3[0][i] = SDL_LoadBMP(text);
        ErrorFree(&lib, Surface.opponent3[0][i]);
    }

    for (int i = 0; i < 7; i++) {
        sprintf(text, "./animations/explosions/explosion_%d.bmp", i + 1);
        Surface.explosionAnimation[i] = SDL_LoadBMP(text);
        ErrorFree(&lib, Surface.explosionAnimation[i]);
    }
}

int LoadSDL(gamePhysics& game, SDL_LIB& lib) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    game.rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
        &lib.window, &lib.renderer);
    if (game.rc != 0) {
        SDL_Quit();
        printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
        return 1;
    };

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(lib.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawColor(lib.renderer, 0, 0, 0, 255);

    SDL_SetWindowTitle(lib.window, "BULLET HELL 2021");

    SDL_DestroyRenderer(lib.renderer);
    lib.renderer = SDL_CreateRenderer(lib.window, 0, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);


    lib.screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    lib.scrtex = SDL_CreateTexture(lib.renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_ShowCursor(SDL_DISABLE);

    lib.charset = SDL_LoadBMP("./assets/game/cs8x8.bmp");
    ErrorFree(&lib, lib.charset);

    SDL_SetColorKey(lib.charset, true, 0x000000);

    return 0;
}

void DrawHealthBar(SDL_LIB lib, Surfaces Surface, gamePhysics game) {
    for (int i = 0; i < 3; i++) {
        DrawSurface(lib.screen, Surface.healthBar, 40 + (40 * i), 68);
    }
    for (int i = 0; i < game.enemyPoints; i++) {
        DrawSurface(lib.screen, Surface.deathBar, 120 - (40 * i), 68);
    }
}

char* GetGradeText(int gradeNumber) {
    if (gradeNumber == 0)
        return "none";
    if (gradeNumber == 1)
        return "shooter";
    if (gradeNumber == 2)
        return "soldier";
    if (gradeNumber == 3)
        return "master";
    if (gradeNumber == 4)
        return "general";
    return "pro";
}

void CheckGrade(gamePhysics game, Score& score, SDL_LIB lib, int gradeNumber, int min, int max) {
    char text[128];
    if (game.playerScore > min && game.playerScore <= max) {
        score.gradeNumber = gradeNumber;
        sprintf(text, "Grade: %s", GetGradeText(gradeNumber));
        DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2 + 250, 80, text, lib.charset);
    }
}

void DrawGradeChange(gamePhysics& game, SDL_LIB lib, SDL_Surface* gradeSurface[], char* message) {
    int Aframe = game.worldTime / 0.1;
    Aframe %= 6;

    game.display = true;
    if (game.displayTime < 3) {
        game.displayTime += game.delta;
        DrawString(lib.screen, lib.screen->w / 2 - strlen(message) * 8 / 2, 120, message, lib.charset);
        DrawSurface(lib.screen, gradeSurface[Aframe], SCREEN_WIDTH - 100, 160);
    }
    else {
        game.display = false;
        game.displayTime = 0;
    }
}

void DrawGrade(gamePhysics& game, Score& score, SDL_LIB lib, Surfaces Surface) {
    if (!game.display) score.scoreGrade = score.gradeNumber;

    CheckGrade(game, score, lib, 0, -1000, 1);
    CheckGrade(game, score, lib, 1, 0, 50);
    CheckGrade(game, score, lib, 2, 50, 125);
    CheckGrade(game, score, lib, 3, 125, 250);
    CheckGrade(game, score, lib, 4, 250, 500);
    CheckGrade(game, score, lib, 5, 500, 50000);

    if (!game.newGameStarted) {
        if (score.gradeNumber > score.scoreGrade) {
            DrawGradeChange(game, lib, Surface.gradeAnimationAsc, "Your grade has increased!");
        }
    }
    game.newGameStarted = false;
}

void PotionCollision(gamePhysics& game, SDL_Rect player, Potion potion[]) {
    for (int i = 0; i < POTIONS_AMOUNT; i++) {
        if (potion[i].isPotion && SDL_HasIntersection(&player, &potion[i].hitbox)) {
            game.enemyPoints = 0;
            potion[i].isPotion = false;
        }
    }
}

void PlayerBulletCollision(gamePhysics& game, Player& player, Enemy enemy[], Objects explosives[]) {
    for (int j = 0; j < BULLET_AMOUNT; j++) {

        if (!player.bullets[j].shoot)
            continue;

        for (int i = 0; i < ENEMY_AMOUNT; i++) {

            if (!enemy[i].alive || !SDL_HasIntersection(&enemy[i].hitbox, &player.bullets[j].hitbox))
                continue;

            game.shotsRow++;
            game.playerPoints++;
            game.damageTaken++;
            game.enemyHP[i]--;
            game.comboTime = game.worldTime;

            if (game.enemyHP[i] <= 0)
                enemy[i].alive = false;

            player.bullets[j].shoot = false;
            game.hit[i] = true;

            if (!game.hit)
                continue;

            if (game.getTime[1]) {
                game.t4 = game.worldTime;
                game.enemyHP[game.level - 1]--;
                game.getTime[1] = false;
            }
            if (game.worldTime - game.t4 >= 0.3) {
                game.hit[i] = false;
                game.getTime[1] = true;
            }
        }

        for (int i = 0; i < EXPLOSIVES_AMOUNT; i++) {
            if (explosives[i].isExplosive && !explosives[i].isHit && SDL_HasIntersection(&explosives[i].hitbox, &player.bullets[j].hitbox)) {
                player.bullets[j].shoot = false;
                explosives[i].isHit = true;
            }
        }
    }
}

void WalkIntoEnemyCollision(gamePhysics& game, SDL_Rect Player, Enemy enemy[]) {
    if (game.invincibility)
        return;

    for (int i = 0; i < ENEMY_AMOUNT; i++) {
        if (SDL_HasIntersection(&Player, &enemy[i].hitbox) && enemy[i].alive) {
            game.enemyPoints++;
            game.playerPoints--;
            game.invincibility = true;
        }
    }
}

void ExplosiveCollision(gamePhysics& game, SDL_Rect Player, Objects explosives[], Objects fireBullets[EXPLOSIVES_AMOUNT][FIRE_BULLETS_AMOUNT]) {

    if (game.invincibility)
        return;

    for (int i = 0; i < EXPLOSIVES_AMOUNT; i++) {
        if (explosives[i].isExplosive && !explosives[i].isHit && SDL_HasIntersection(&explosives[i].hitbox, &Player)) {
            game.enemyPoints++;
            game.playerPoints--;
            game.invincibility = true;
            explosives[i].isHit = true;
        }
        if (!explosives[i].isExplosive || !explosives[i].isHit)
            continue;

        for (int j = 0; j < FIRE_BULLETS_AMOUNT; j++) {
            if (!fireBullets[i][j].isHit && SDL_HasIntersection(&fireBullets[i][j].hitbox, &Player)) {
                game.enemyPoints++;
                game.playerPoints--;
                game.invincibility = true;
                fireBullets[i][j].isHit = true;
            }
        }
    }
}

void EnemyBulletCollision(gamePhysics& game, SDL_Rect Player, Enemy enemy[])
{
    if (game.invincibility)
        return;

    for (int i = 0; i < ENEMY_AMOUNT; i++) {
        for (int j = 0; j < BULLET_AMOUNT; j++) {
            SDL_Rect bulletHitbox;
            bulletHitbox.x = enemy[i].bullets[j].hitbox.x;
            bulletHitbox.y = enemy[i].bullets[j].hitbox.y;
            bulletHitbox.w = enemy[i].bullets[j].hitbox.w;
            bulletHitbox.h = enemy[i].bullets[j].hitbox.h;

            if (enemy[i].bullets[j].bulletType == 1 && game.level != 2) {
                bulletHitbox.x = enemy[i].bullets[j].hitbox.x + sin(enemy[i].bullets[j].distance) * SCREEN_WIDTH / 15;
                bulletHitbox.y = enemy[i].bullets[j].hitbox.y + cos(enemy[i].bullets[j].distance) * SCREEN_HEIGHT / 15;
            }

            else if (enemy[i].bullets->bulletType == 1 && game.level == 2) {
                bulletHitbox.x = enemy[i].bullets[j].hitbox.x + sin(enemy[i].bullets[j].distance) * SCREEN_WIDTH / 15;
                bulletHitbox.y = enemy[i].bullets[j].hitbox.y + cos(enemy[i].bullets[j].distance) * SCREEN_HEIGHT / 15;
            }

            if (enemy[i].bullets[j].shoot && SDL_HasIntersection(&bulletHitbox, &Player)) {
                game.enemyPoints++;
                game.playerPoints--;
                game.invincibility = true;
                enemy[i].bullets[j].shoot = false;
                return;
            }
        }
    }
}

void TrapCollision(gamePhysics& game, SDL_Rect Player, Objects trap[TRAPS_AMOUNT]) {
    for (int i = 0; i < TRAPS_AMOUNT; i++) {
        if (SDL_HasIntersection(&Player, &trap[i].hitbox) && trap[i].isTrap)
            trap[i].timeOnTrap += game.delta;
        else
            trap[i].timeOnTrap = 0;


        if (trap[i].timeOnTrap > 2) {
            trap[i].timeOnTrap = 0;
            if (!game.invincibility) {
                game.enemyPoints++;
                game.playerPoints--;
                game.invincibility = true;
            }
        }
    }
}

void DrawDestroyAnimation(gamePhysics& game, Surfaces Surface, SDL_LIB lib, int x, int y, int explosionIt, int i) {
    int Eframe = game.worldTime / 0.1;
    Eframe %= 7;

    if (game.explosionTime < 1.6) {
        DrawSurface(lib.screen, Surface.explosionAnimation[Eframe], x, y);
        game.explosionTime += game.delta;
    }
    else {
        game.explosionDisplay[explosionIt][i] = false;
        game.explosionTime = 0.0;
    }
}

void DrawExplosives(gamePhysics& game, Surfaces Surface, SDL_LIB lib, Objects explosives[]) {
    for (int i = 0; i < EXPLOSIVES_AMOUNT; i++) {
        if (!explosives[i].isHit || !game.explosionDisplay[0][i])
            continue;

        DrawDestroyAnimation(game, Surface, lib, explosives[i].hitbox.x + explosives[i].hitbox.w / 2,
            explosives[i].hitbox.y + explosives[i].hitbox.h / 2, 0, i);
    }
}

void DrawEnemyDestruction(gamePhysics& game, Surfaces Surface, SDL_LIB lib, Enemy enemy[]) {
    for (int i = 0; i < ENEMY_AMOUNT; i++) {
        int Eframe = game.worldTime / 0.1;
        Eframe %= 7;
        if (enemy[i].alive || !game.explosionDisplay[1][i])
            continue;

        DrawDestroyAnimation(game, Surface, lib, enemy[i].hitbox.x + enemy[i].hitbox.w / 2, enemy[i].hitbox.y + enemy[i].hitbox.h / 2, 1, i);
    }
}

void DrawInvincibility(gamePhysics& game, SDL_LIB lib) {
    char text[128];
    if (!game.invincibility)
        return;

    if (game.getTime[0]) {
        game.t3 = game.worldTime;
        game.getTime[0] = false;
    }
    if (game.worldTime - game.t3 >= 4) {
        game.invincibility = false;
        game.getTime[0] = true;
    }
    sprintf(text, "Invincibility");
    DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 250, text, lib.charset);
}

void DrawActions(gamePhysics& game, SDL_LIB lib) {
    char text[128];
    sprintf(text, "Punkty gracza, Punkty = %i", game.playerScore);
    DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 460, text, lib.charset);

    if (game.shotsRow > 2) {
        sprintf(text, "Combo x %i", game.shotsRow);
        DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2 + 250, 60, text, lib.charset);
    }
}

void DrawPotions(Surfaces Surface, SDL_LIB lib, Potion potion[]) {
    for (int i = 0; i < POTIONS_AMOUNT; i++) {
        if (potion[i].isPotion)
            DrawSurface(lib.screen, Surface.healthPot, potion[i].hitbox.x + potion[i].hitbox.w / 2, potion[i].hitbox.y + potion[i].hitbox.h / 2);
    }
}

void DrawTraps(Surfaces Surface, SDL_LIB lib, Objects trap[TRAPS_AMOUNT], int trapType) {
    char text[128];
    for (int i = 0; i < TRAPS_AMOUNT; i++) {
        if (!trap[i].isTrap)
            continue;

        DrawSurface(lib.screen, Surface.trap[trapType], trap[i].hitbox.x + trap[i].hitbox.w / 2, trap[i].hitbox.y + trap[i].hitbox.h / 2);
        if (trap[i].timeOnTrap > 0) {
            sprintf(text, "%.1lf", 2.0 - trap[i].timeOnTrap);
            DrawString(lib.screen, trap[i].hitbox.x + trap[i].hitbox.w / 2, trap[i].hitbox.y + trap[i].hitbox.h / 2, text, lib.charset);
        }
    }
}

void MoveFireBullet(Objects& fireBullet, int direction) {
    if (direction == 0)
        fireBullet.hitbox.y -= 1;
    if (direction == 1)
        fireBullet.hitbox.x -= 1;
    if (direction == 2)
        fireBullet.hitbox.x += 1;
    if (direction == 3)
        fireBullet.hitbox.y += 1;
    if (direction == 4) {
        fireBullet.hitbox.y -= 1;
        fireBullet.hitbox.x -= 1;
    }
    if (direction == 5) {
        fireBullet.hitbox.y -= 1;
        fireBullet.hitbox.x += 1;
    }
    if (direction == 6) {
        fireBullet.hitbox.y += 1;
        fireBullet.hitbox.x -= 1;
    }
    if (direction == 7) {
        fireBullet.hitbox.y += 1;
        fireBullet.hitbox.x += 1;
    }
}

void DrawObjects(Surfaces Surface, SDL_LIB lib, Objects explosives[], Objects fireBullets[EXPLOSIVES_AMOUNT][FIRE_BULLETS_AMOUNT])
{
    for (int i = 0; i < EXPLOSIVES_AMOUNT; i++) {
        if (!explosives[i].isExplosive)
            continue;
        if (!explosives[i].isHit) {
            DrawSurface(lib.screen, Surface.Explosive, explosives[i].hitbox.x + explosives[i].hitbox.w / 2, explosives[i].hitbox.y + explosives[i].hitbox.h / 2);
            continue;
        }
        for (int j = 0; j < FIRE_BULLETS_AMOUNT; j++) {
            if (fireBullets[i][j].isHit)
                continue;
            DrawSurface(lib.screen, Surface.fireBullet, fireBullets[i][j].hitbox.x + fireBullets[i][j].hitbox.w / 2, fireBullets[i][j].hitbox.y + fireBullets[i][j].hitbox.h / 2);
            MoveFireBullet(fireBullets[i][j], j);
        }
    }
}

void ActivatePotions(Potion potion[], gamePhysics& game)
{
    for (int i = 0; i < POTIONS_AMOUNT; i++) {
        if (potion[i].appearingTime < game.worldTime) {
            potion[i].appearingTime = ((rand() % 1200) / 100.0) + 1.0 + game.worldTime;
            potion[i].isPotion = true;
        }
    }
}

void ActivateObjects(gamePhysics& game, Objects object[], int objectsAmount) {
    for (int i = 0; i < objectsAmount; i++) {
        if (object[i].appearingTime < game.worldTime) {
            object[i].appearingTime = ((rand() % 1200) / 100.0) + 1.0 + game.worldTime;
            if (objectsAmount == EXPLOSIVES_AMOUNT)
                object[i].isExplosive = true;
            else object[i].isTrap = true;
        }
    }
}

void EnemySetBullets(Enemy enemy[], int i, gamePhysics& game)
{
    for (int j = 0; j < BULLET_AMOUNT; j++) {
        if (!enemy[i].bullets[j].shoot) {
            enemy[i].bullets[j].shoot = true;
            if (game.level != 3) {
                enemy[i].bullets[j].hitbox.x = enemy[i].hitbox.x + enemy[i].hitbox.w / 2 - 10;
                enemy[i].bullets[j].hitbox.y = enemy[i].hitbox.y + enemy[i].hitbox.h / 2 - 10;
            }
            else {
                enemy[i].bullets[j].hitbox.x = enemy[i].hitbox.x + enemy[i].hitbox.w / 2;
                enemy[i].bullets[j].hitbox.y = enemy[i].hitbox.y + enemy[i].hitbox.h / 2;
            }
            int chance = rand() % 100;
            if (chance < 10) {
                enemy[i].bullets[j].isBig = true;
                enemy[i].bullets[j].hitbox.w = 27;
                enemy[i].bullets[j].hitbox.h = 108;
            }
            enemy[i].bullets[j].bulletType = rand() % 2;
            enemy[i].bullets[j].distance = 0;
            break;
        }
    }
}

void DrawEnemy(Enemy enemy[], gamePhysics& game, SDL_LIB& lib, Surfaces& Surface, bullets& enemyBullet, Player& player, bullets& playerBullet, Objects explosives[]) {
    char text[128];
    for (int i = 0; i < ENEMY_AMOUNT; i++) {
        if (enemy[i].alive) {
            if (game.moveTimeStamp == 0 || game.t2 - game.moveTimeStamp > 33) {
                MoveEnemy(game, enemy[i]);
            }
            if (game.level == 3) {
                int Mframe = (int)(game.worldTime / 0.1) % 6;
                DrawSurface(lib.screen, Surface.opponent3[enemy[i].toTheLeft][Mframe], enemy[i].hitbox.x + enemy[i].hitbox.w / 2, enemy[i].hitbox.y + enemy[i].hitbox.h / 2);
            }

            else if (game.level == 2) {
                DrawSurface(lib.screen, Surface.opponent2, enemy[i].hitbox.x + enemy[i].hitbox.w / 2 - 10, enemy[i].hitbox.y + enemy[i].hitbox.h / 2 - 10);
            }
            else
                DrawSurface(lib.screen, Surface.opponent, enemy[i].hitbox.x + enemy[i].hitbox.w / 2 - 10, enemy[i].hitbox.y + enemy[i].hitbox.h / 2 - 10);
            if (enemy[i].shotingTime < game.worldTime) {
                enemy[i].shotingTime = ((rand() % 300) / 100.0) + 1.0 + game.worldTime;
                EnemySetBullets(enemy, i, game);
            }
        }
        for (int j = 0; j < BULLET_AMOUNT; j++) {
            if (enemy[i].bullets[j].shoot)
                EnemyShoot(game, &enemyBullet, enemy[i].bullets[j], enemyBullet.bulletSpeed, lib.screen, Surface, &enemy[i].bullets[j].hitbox);
        }
    }
    if (game.moveTimeStamp == 0 || game.t2 - game.moveTimeStamp > 33)
        game.moveTimeStamp = game.t2;

    for (int i = 0; i < PLAYER_BULLET_AMOUNT; i++) {
        if (player.bullets[i].shoot)
            PlayerShoot(player.bullets[i], Surface.blueLaser, &player.bullets[i].hitbox, &lib);
    }

    for (int i = game.it; i < PLAYER_BULLET_AMOUNT; i++) {
        if (player.bullets[i].hitbox.y < -500 || player.bullets[i].hitbox.x < -500 || player.bullets[i].hitbox.x > 500 || player.bullets[i].hitbox.y > 500 || SDL_HasIntersection(&explosives[i].hitbox, &player.bullets[i].hitbox)) {
            game.shotsRow = 0;
            game.it++;
        }
    }

    if (game.comboTime + COMBO_TIME < game.worldTime) {
        game.shotsRow = 0;
    }
}

void Walk(Player& player, Background& bg, Enemy enemy[], Potion potion[], Objects explosives[], Objects fireBullets[EXPLOSIVES_AMOUNT][FIRE_BULLETS_AMOUNT],
    Objects trap[2][TRAPS_AMOUNT], int walkDirection, int x, int y) {
    player.walkDirection = walkDirection;
    bg.x += x;
    bg.y += y;
    for (int i = 0; i < ENEMY_AMOUNT; i++) {
        enemy[i].hitbox.x += x;
        enemy[i].hitbox.y += y;
        for (int j = 0; j < BULLET_AMOUNT; j++) {
            if (enemy[i].bullets[j].shoot) {
                enemy[i].bullets[j].hitbox.x += x;
                enemy[i].bullets[j].hitbox.y += y;
            }
        }
    }
    for (int i = 0; i < POTIONS_AMOUNT; i++) {
        potion[i].hitbox.x += x;
        potion[i].hitbox.y += y;
    }
    for (int i = 0; i < EXPLOSIVES_AMOUNT; i++) {
        explosives[i].hitbox.x += x;
        explosives[i].hitbox.y += y;
        for (int j = 0; j < FIRE_BULLETS_AMOUNT; j++) {
            fireBullets[i][j].hitbox.x += x;
            fireBullets[i][j].hitbox.y += y;
        }
    }
    for (int i = 0; i < TRAPS_AMOUNT; i++) {
        trap[0][i].hitbox.x += x;
        trap[1][i].hitbox.x += x;
        trap[0][i].hitbox.y += y;
        trap[1][i].hitbox.y += y;
    }
}

void SetPlayerBullets(Player& player, gamePhysics& game)
{
    for (int i = 0; i < PLAYER_BULLET_AMOUNT; i++) {
        if (player.bullets[i].shoot == false) {
            player.bullets[i].shoot = true;
            player.bullets[i].hitbox.x = SCREEN_WIDTH / 2;
            player.bullets[i].hitbox.y = SCREEN_HEIGHT / 2;
            player.bullets[i].shootDirection = player.walkDirection;
            if (player.walkDirection == 0 || player.walkDirection == 3) {
                player.bullets[i].isVertical = true;
                player.bullets[i].hitbox.w = 9;
                player.bullets[i].hitbox.h = 36;
            }
            else {
                player.bullets[i].isVertical = false;
                player.bullets[i].hitbox.h = 9;
                player.bullets[i].hitbox.w = 36;
            }
            player.shotTime = game.worldTime;
            break;
        }
    }
}

void NewGame(gamePhysics& Game, Background& bg, SDL_Rect& Player)
{
    Game.newGameStarted = true;
    Game.playerPoints = 0;
    Game.enemyPoints = 0;
    Game.invincibility = true;
    Game.shotsRow = 0;
    Game.it = 0;
    Game.moveTimeStamp = 0.0;
    Game.displayTime = 0.0;
    Game.explosionTime = 0.0;
    Game.ifStarted = false;
    Game.gameOver = false;
    Game.canShoot = false,
        Game.display = false;
    Game.damageTaken = 0;

    for (int i = 0; i < ENEMY_AMOUNT; i++)
        Game.enemyHP[i] = 3;
    for (int i = 0; i < 2; i++)
        Game.getTime[i] = true;
    for (int i = 0; i < EXPLOSIVES_AMOUNT; i++)
        Game.explosionDisplay[0][i] = true;
    for (int i = 0; i < ENEMY_AMOUNT; i++) {
        Game.explosionDisplay[1][i] = true;
        Game.hit[i] = false;
    }

    bg.x = SCREEN_WIDTH / 2;
    bg.y = SCREEN_HEIGHT / 2;
    Player.x = SCREEN_WIDTH / 2 - (PLAYER_WIDTH / 2);
    Player.y = SCREEN_HEIGHT / 2 - (PLAYER_HEIGHT / 2);
    Player.w = 30;
    Player.h = 55;

    Game.t2 = SDL_GetTicks();
    Game.t1 = SDL_GetTicks();
    Game.worldTime = 0;
}

void initGame(Enemy enemy[], Potion  potion[], Player& player, gamePhysics& Game, Objects explosives[EXPLOSIVES_AMOUNT], Objects fireBullets[EXPLOSIVES_AMOUNT][FIRE_BULLETS_AMOUNT],
    Objects trap[2][TRAPS_AMOUNT], SDL_Rect& Player, Background& bg) {
    SetEnemy(enemy, Game, potion, trap, explosives);
    SetPlayer(player);
    SetPotions(potion, enemy, trap, explosives);
    SetTraps(trap, enemy, potion, explosives, 0, 60, 60);
    SetTraps(trap, enemy, potion, explosives, 1, 50, 120);
    SetExplosives(explosives, enemy, potion, trap, fireBullets);
    NewGame(Game, bg, Player);
}

int CountFileLines() {
    FILE* file = fopen("./Highscore.txt", "r");
    int result = 0;
    while (!feof(file)) {
        char sign = fgetc(file);
        if (sign == '\n') {
            result++;
        }
    }
    fclose(file);

    return result / 2;
}

void PlayerMove(SDL_LIB& lib, Player& player, gamePhysics& game, Background& bg, Enemy enemy[], Potion potion[], Objects explosives[], Objects fireBullets[EXPLOSIVES_AMOUNT][FIRE_BULLETS_AMOUNT], Objects trap[2][TRAPS_AMOUNT], SDL_Rect& Player)
{
    while (SDL_PollEvent(&lib.event)) {
        switch (lib.event.type) {
        case SDL_KEYDOWN:
            if (lib.event.key.keysym.sym == SDLK_RIGHT || lib.event.key.keysym.sym == SDLK_LEFT || lib.event.key.keysym.sym == SDLK_UP || lib.event.key.keysym.sym == SDLK_DOWN)
                player.walkStartTime = game.worldTime;
            if (lib.event.key.keysym.sym == SDLK_ESCAPE) game.quit = 1;
            else if (lib.event.key.keysym.sym == SDLK_RIGHT && bg.x > BG_RIGHT_BORDER)
                Walk(player, bg, enemy, potion, explosives, fireBullets, trap, 2, -7, 0);

            else if (lib.event.key.keysym.sym == SDLK_LEFT && bg.x < BG_LEFT_BORDER)
                Walk(player, bg, enemy, potion, explosives, fireBullets, trap, 1, 7, 0);

            else if (lib.event.key.keysym.sym == SDLK_UP && bg.y < BG_TOP_BORDER)
                Walk(player, bg, enemy, potion, explosives, fireBullets, trap, 0, 0, 7);

            else if (lib.event.key.keysym.sym == SDLK_DOWN && bg.y > BG_BOTTOM_BORDER)
                Walk(player, bg, enemy, potion, explosives, fireBullets, trap, 3, 0, -7);

            else if (lib.event.key.keysym.sym == SDLK_SPACE && player.shotTime + 0.3 < game.worldTime)
                SetPlayerBullets(player, game);

            else if (lib.event.key.keysym.sym == SDLK_n)
                initGame(enemy, potion, player, game, explosives, fireBullets, trap, Player, bg);

            break;

        case SDL_KEYUP:
            player.walkStartTime = 0;
            break;
        case SDL_QUIT:
            game.quit = 1;
            break;
        };
    };
}

void ReadScores(Score scores[], int amountOfScores) {
    for (int i = 0; i < amountOfScores; i++) {
        scores[i].score = 0;
    }
    FILE* file = fopen("./Highscore.txt", "r");

    bool readName = true;

    int i = 0;
    int iSign = 0;
    while (!feof(file)) {
        char sign = fgetc(file);
        bool negativeScore = false;
        if (sign == '\n') {
            if (!readName) {
                if (negativeScore)
                    scores[i].score *= -1;

                iSign = 0;
                i++;
            }
            readName = !readName;
            continue;
        }

        if (readName) {
            scores[i].nickname[iSign++] = sign;
            scores[i].nickname[iSign] = '\0';
        }
        else {
            if (sign == '-') {
                negativeScore = true;
                continue;
            }
            scores[i].score *= 10;
            scores[i].score += (sign - '0');
        }
    }
    fclose(file);
}

void BubbleSort(Score scores[], int scoreAmount) {
    bool sort = false;

    while (!sort) {
        sort = true;
        for (int i = 0; i < scoreAmount - 1; i++) {
            if (scores[i].score < scores[i + 1].score) {
                Score temp = scores[i];
                scores[i] = scores[i + 1];
                scores[i + 1] = temp;
                sort = false;
            }
        }
    }
}

void SaveScores(char player[NAME_LENGTH], int score) {
    int savedScoresAmount = CountFileLines() + 1;
    Score* scores = new Score[savedScoresAmount];

    ReadScores(scores, savedScoresAmount);

    for (int i = 0; i < NAME_LENGTH; i++)
        scores[savedScoresAmount - 1].nickname[i] = player[i];

    scores[savedScoresAmount - 1].score = score;

    BubbleSort(scores, savedScoresAmount);

    char text[128];

    FILE* file = fopen("./Highscore.txt", "w");
    for (int i = 0; i < savedScoresAmount; i++) {
        sprintf(text, "%s\n", scores[i].nickname);
        fputs(text, file);
        sprintf(text, "%d\n", scores[i].score);
        fputs(text, file);
    }

    fclose(file);
    delete[] scores;
}

void DrawHighScores(SDL_LIB lib) {
    char text[128];
    bool display = true;

    int blue = SDL_MapRGB(lib.screen->format, 0x11, 0x11, 0xCC);

    int numberOfSavedScores = CountFileLines();
    Score* scores = new Score[numberOfSavedScores];

    ReadScores(scores, numberOfSavedScores);

    int pages = numberOfSavedScores / 20;
    int currentPage = 1;

    while (display) {
        SDL_FillRect(lib.screen, NULL, blue);

        sprintf(text, "HIGHSCORES");
        DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 10, text, lib.charset);

        int line = 0;

        for (int i = (currentPage - 1) * 20; i < ((currentPage - 1) * 20) + 20 && i < numberOfSavedScores; i++) {
            sprintf(text, "%d. %s -------- %d", i + 1, scores[i].nickname, scores[i].score);
            DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 50 + (20 * line++), text, lib.charset);
        }

        SDL_UpdateTexture(lib.scrtex, NULL, lib.screen->pixels, lib.screen->pitch);
        SDL_RenderCopy(lib.renderer, lib.scrtex, NULL, NULL);
        SDL_RenderPresent(lib.renderer);
        while (SDL_PollEvent(&lib.event)) {
            switch (lib.event.type) {
            case SDL_KEYDOWN:
                if (lib.event.key.keysym.sym == SDLK_ESCAPE) { return; }
                else if (lib.event.key.keysym.sym == SDLK_UP && currentPage > 1) { currentPage--; }
                else if (lib.event.key.keysym.sym == SDLK_DOWN && currentPage <= pages) { currentPage++; }
            };
        };
    }
}

void Set_Game(gamePhysics& game, SDL_LIB& lib, Surfaces& Surface, SDL_Rect& playerRect, Enemy Enemy[ENEMY_AMOUNT], Background& bg,
    Potion potion[], Player& player, Objects explosives[], Objects fireBullets[EXPLOSIVES_AMOUNT][FIRE_BULLETS_AMOUNT], Objects trap[2][TRAPS_AMOUNT]) {
    int blue = SDL_MapRGB(lib.screen->format, 0x11, 0x11, 0xCC);
    char text[128];
    while (game.menu) {
        SDL_FillRect(lib.screen, NULL, blue);
        sprintf(text, "MAIN MENU");
        DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 10, text, lib.charset);
        sprintf(text, "PRESS '1' TO START LEVEL 1");
        DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 50, text, lib.charset);
        sprintf(text, "PRESS '2' TO START LEVEL 2");
        DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 70, text, lib.charset);
        sprintf(text, "PRESS '3' TO START LEVEL 3");
        DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 90, text, lib.charset);
        sprintf(text, "PRESS 'H' TO SEE HIGHSCORES");
        DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 110, text, lib.charset);
        sprintf(text, "PRESS 'ESC' TO EXIT");
        DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 130, text, lib.charset);
        SDL_UpdateTexture(lib.scrtex, NULL, lib.screen->pixels, lib.screen->pitch);
        SDL_RenderCopy(lib.renderer, lib.scrtex, NULL, NULL);
        SDL_RenderPresent(lib.renderer);

        while (SDL_PollEvent(&lib.event)) {
            switch (lib.event.type) {
            case SDL_KEYDOWN:
                if (lib.event.key.keysym.sym == SDLK_ESCAPE) { game.quit = 1; game.menu = false; }
                else if (lib.event.key.keysym.sym == SDLK_1) {
                    game.level = 1; initGame(Enemy, potion, player, game, explosives, fireBullets, trap, playerRect, bg); game.menu = false;
                }
                else if (lib.event.key.keysym.sym == SDLK_2) { game.level = 2; initGame(Enemy, potion, player, game, explosives, fireBullets, trap, playerRect, bg); game.menu = false; }
                else if (lib.event.key.keysym.sym == SDLK_3) { game.level = 3; initGame(Enemy, potion, player, game, explosives, fireBullets, trap, playerRect, bg); game.menu = false; }
                else if (lib.event.key.keysym.sym == SDLK_h) { DrawHighScores(lib); }
            };
        };
    }
};

void WinCondition(gamePhysics& game, SDL_LIB& lib, SDL_Rect& playerRect, Enemy Enemy[ENEMY_AMOUNT], Background& bg,
    Potion potion[], Player& player, Objects explosives[], Objects fireBullets[EXPLOSIVES_AMOUNT][FIRE_BULLETS_AMOUNT], Objects trap[2][TRAPS_AMOUNT], Surfaces Surface) {
    char text[128];
    bool win = true;
    int black = SDL_MapRGB(lib.screen->format, 0x00, 0x00, 0x00);
    int blue = SDL_MapRGB(lib.screen->format, 0x11, 0x11, 0xCC);

    for (int i = 0; i < ENEMY_AMOUNT; i++) {
        if (Enemy[i].alive)
            win = false;
    }

    if (game.level == 0 || !win)
        return;

    char playerNick[NAME_LENGTH];
    for (int i = 0; i < NAME_LENGTH; i++)
        playerNick[i] = '\0';
    int i = 0;
    bool savingScore = false;
    bool repeatScore = false;
    game.nextLevel = true;
    while (game.nextLevel) {
        SDL_FillRect(lib.screen, NULL, black);
        sprintf(text, "CONGRATULATIONS");
        DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 10, text, lib.charset);
        sprintf(text, "PRESS 'N' TO START NEXT LEVEL");
        DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 30, text, lib.charset);
        sprintf(text, "PRESS 'M' TO GO BACK TO MENU");
        DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 50, text, lib.charset);
        sprintf(text, "PRESS 'S' TO SAVE YOUR SCORE");
        DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 70, text, lib.charset);

        if (savingScore) {
            sprintf(text, "Enter your name:");
            DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 90, text, lib.charset);
            DrawString(lib.screen, lib.screen->w / 2 - strlen(playerNick) * 8 / 2, 110, playerNick, lib.charset);
        }

        SDL_UpdateTexture(lib.scrtex, NULL, lib.screen->pixels, lib.screen->pitch);
        SDL_RenderCopy(lib.renderer, lib.scrtex, NULL, NULL);
        SDL_RenderPresent(lib.renderer);

        while (SDL_PollEvent(&lib.event)) {
            switch (lib.event.type) {
            case SDL_KEYDOWN:
                if (savingScore && lib.event.key.keysym.sym == SDLK_RETURN) {
                    savingScore = false;
                    SaveScores(playerNick, game.playerScore);

                }
                else if (savingScore && i > 0 && lib.event.key.keysym.sym == SDLK_BACKSPACE) {
                    playerNick[--i] = '\0';
                }
                else if (savingScore && ((lib.event.key.keysym.sym >= 'a' && lib.event.key.keysym.sym <= 'z') || lib.event.key.keysym.sym == SDLK_SPACE || (lib.event.key.keysym.sym >= '0' && lib.event.key.keysym.sym <= '9'))) {
                    playerNick[i++] = lib.event.key.keysym.sym;
                }
                else if (lib.event.key.keysym.sym == SDLK_ESCAPE) {
                    game.gameOver = false;
                }
                else if (!savingScore && lib.event.key.keysym.sym == SDLK_n) {
                    game.level++;
                    game.nextLevel = false;
                    initGame(Enemy, potion, player, game, explosives, fireBullets, trap, playerRect, bg);
                }
                else if (!savingScore && lib.event.key.keysym.sym == SDLK_m) {
                    game.menu = true;
                    game.nextLevel = false;
                    Set_Game(game, lib, Surface, playerRect, Enemy, bg, potion, player, explosives, fireBullets, trap);
                }
                else if (!savingScore && lib.event.key.keysym.sym == SDLK_s) {
                    savingScore = true;
                }
            };
        };
    }
}

void DeathCondition(gamePhysics& game, SDL_LIB& lib, Surfaces& Surface, SDL_Rect& playerRect, Enemy Enemy[ENEMY_AMOUNT], Background& bg,
    Potion potion[], Player& player, Objects explosives[], Objects fireBullets[EXPLOSIVES_AMOUNT][FIRE_BULLETS_AMOUNT], Objects trap[2][TRAPS_AMOUNT]) {
    char text[128];
    int black = SDL_MapRGB(lib.screen->format, 0x00, 0x00, 0x00);
    int blue = SDL_MapRGB(lib.screen->format, 0x11, 0x11, 0xCC);
    if (game.enemyPoints >= 3) {
        game.gameOver = true;
        while (game.gameOver) {
            SDL_FillRect(lib.screen, NULL, black);
            sprintf(text, "GAME OVER");
            DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 10, text, lib.charset);
            sprintf(text, "PRESS 'N' TO TRY AGAIN");
            DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 30, text, lib.charset);
            sprintf(text, "PRESS 'M' TO GO BACK TO MENU");
            DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 50, text, lib.charset);
            SDL_UpdateTexture(lib.scrtex, NULL, lib.screen->pixels, lib.screen->pitch);
            SDL_RenderCopy(lib.renderer, lib.scrtex, NULL, NULL);
            SDL_RenderPresent(lib.renderer);

            while (SDL_PollEvent(&lib.event)) {
                switch (lib.event.type) {
                case SDL_KEYDOWN:
                    if (lib.event.key.keysym.sym == SDLK_ESCAPE) {
                        game.gameOver = false;
                        initGame(Enemy, potion, player, game, explosives, fireBullets, trap, playerRect, bg);
                    }
                    else if (lib.event.key.keysym.sym == SDLK_n) {
                        game.gameOver = false;
                        initGame(Enemy, potion, player, game, explosives, fireBullets, trap, playerRect, bg);
                    }
                    else if (lib.event.key.keysym.sym == SDLK_m) {
                        game.menu = true; game.gameOver = false;
                        Set_Game(game, lib, Surface, playerRect, Enemy, bg, potion, player, explosives, fireBullets, trap);
                    }
                };
            };
        }
    }
}

void DrawInfo(SDL_LIB& lib, gamePhysics& game)
{
    char text[128];
    int red = SDL_MapRGB(lib.screen->format, 0xFF, 0x00, 0x00);
    int blue = SDL_MapRGB(lib.screen->format, 0x11, 0x11, 0xCC);
    DrawRectangle(lib.screen, 4, 4, SCREEN_WIDTH - 8, 36, red, blue);
    sprintf(text, "Bullet Hell Game, duration = %.1lf s  %.0lf frames / s", game.worldTime, game.fps);
    DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 10, text, lib.charset);
    sprintf(text, "Esc - exit, \030 - up, \031 - down, \032 - left, \033 - right, SPACE - shoot");
    DrawString(lib.screen, lib.screen->w / 2 - strlen(text) * 8 / 2, 26, text, lib.charset);
    SDL_UpdateTexture(lib.scrtex, NULL, lib.screen->pixels, lib.screen->pitch);
    SDL_RenderCopy(lib.renderer, lib.scrtex, NULL, NULL);
    SDL_RenderPresent(lib.renderer);
}

#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char** argv) {
    char text[128];
    SDL_LIB lib;
    Background bg;
    Surfaces Surface;
    bullets enemyBullet;
    bullets playerBullet;
    gamePhysics game;
    Player player;
    Score score;
    SDL_Rect Player;
    Enemy enemy[ENEMY_AMOUNT];
    Potion potion[POTIONS_AMOUNT];
    Objects explosives[EXPLOSIVES_AMOUNT];
    Objects fireBullets[EXPLOSIVES_AMOUNT][FIRE_BULLETS_AMOUNT];
    Objects trap[2][TRAPS_AMOUNT];

    srand(time(NULL));

    if (LoadSDL(game, lib))
        return 0;

    LoadSurfaces(bg, Surface, player, lib);

    int black = SDL_MapRGB(lib.screen->format, 0x00, 0x00, 0x00);
    int red = SDL_MapRGB(lib.screen->format, 0xFF, 0x00, 0x00);
    int blue = SDL_MapRGB(lib.screen->format, 0x11, 0x11, 0xCC);

    game.frames = 0;
    game.fpsTimer = 0;
    game.fps = 0;
    game.quit = 0;
    game.worldTime = 0;
    game.distance = 0;
    game.speed = 0;

    Set_Game(game, lib, Surface, Player, enemy, bg, potion, player, explosives, fireBullets, trap);
    initGame(enemy, potion, player, game, explosives, fireBullets, trap, Player, bg);

    game.t1 = SDL_GetTicks();

    while (!game.quit) {
        game.t2 = SDL_GetTicks();
        game.delta = (static_cast<int64_t>(game.t2) - static_cast<int64_t>(game.t1)) * 0.001;
        game.t1 = game.t2;

        game.worldTime += game.delta;

        game.distance += game.speed * game.delta;

        FPS_Check(&game);

        SDL_FillRect(lib.screen, NULL, black);

        DrawSurface(lib.screen, bg.background, bg.x, bg.y);

        DeathCondition(game, lib, Surface, Player, enemy, bg, potion, player, explosives, fireBullets, trap);
        WinCondition(game, lib, Player, enemy, bg, potion, player, explosives, fireBullets, trap, Surface);

        PotionCollision(game, Player, potion);
        PlayerBulletCollision(game, player, enemy, explosives);
        WalkIntoEnemyCollision(game, Player, enemy);
        EnemyBulletCollision(game, Player, enemy);
        ExplosiveCollision(game, Player, explosives, fireBullets);
        TrapCollision(game, Player, trap[0]);
        TrapCollision(game, Player, trap[1]);

        game.playerScore = game.playerPoints * 20 + (game.playerPoints + game.shotsRow * game.bonus);

        DrawExplosives(game, Surface, lib, explosives);
        DrawEnemyDestruction(game, Surface, lib, enemy);
        DrawInvincibility(game, lib);
        DrawActions(game, lib);
        DrawPotions(Surface, lib, potion);
        DrawTraps(Surface, lib, trap[0], 0);
        DrawTraps(Surface, lib, trap[1], 1);
        DrawObjects(Surface, lib, explosives, fireBullets);

        ActivatePotions(potion, game);
        ActivateObjects(game, explosives, EXPLOSIVES_AMOUNT);
        ActivateObjects(game, trap[0], TRAPS_AMOUNT);
        ActivateObjects(game, trap[1], TRAPS_AMOUNT);

        DrawEnemy(enemy, game, lib, Surface, enemyBullet, player, playerBullet, explosives);

        int step = player.walkStartTime / 0.1;
        step %= 4;

        DrawSurface(lib.screen, player.eti[player.walkDirection][step], Player.x + Player.w / 2, Player.y + Player.h / 2);

        DrawHealthBar(lib, Surface, game);
        DrawGrade(game, score, lib, Surface);

        DrawInfo(lib, game);

        PlayerMove(lib, player, game, bg, enemy, potion, explosives, fireBullets, trap, Player);
        game.frames++;
    };

    SDL_FreeSurface(lib.charset);
    SDL_FreeSurface(lib.screen);
    SDL_DestroyTexture(lib.scrtex);
    SDL_DestroyRenderer(lib.renderer);
    SDL_DestroyWindow(lib.window);

    SDL_Quit();
    return 0;
}