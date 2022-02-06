#ifndef COMMON_HEADER
#define COMMON_HEADER set

#include <gb/gb.h>
#include "util.h"
#include "Character.h"

// For organizatio nand structer
typedef struct Vector2D{ int16_t x,y; } Vector2D;

extern uint8_t twoFrameAnimator,threeFrameAnimator,cameraX,cameraY,blank,level,joypadCurrent,joypadPrevious,enableDebug;
extern uint16_t score,highScore,ballsRemaining;
extern Character ghosts[4];
extern Character pacman;
extern const Vector2D Directions[];
extern const uint8_t reverseDirections[4];

#define PELLETS_TILES_START Map_TILE_COUNT
#define NUMBERS_TILES_START PELLETS_TILES_START+Pellets_TILE_COUNT
#define READYTEXT_TILES_START NUMBERS_TILES_START+Numbers_TILE_COUNT
#define ONEUPTEXT_TILES_START READYTEXT_TILES_START+ReadyText_TILE_COUNT
#define HIGHTEXT_TILES_START ONEUPTEXT_TILES_START+OneUpText_TILE_COUNT

#define GHOSTS_SPRITES_START Pacman_TILE_COUNT
#define GHOSTS_SCARED_SPRITES_START Pacman_TILE_COUNT+Ghosts_TILE_COUNT
#define GHOSTS_EATEN_SPRITES_START GHOSTS_SCARED_SPRITES_START+GhostsScared_TILE_COUNT
#define TARGET_TILES_SPRITES_START GHOSTS_EATEN_SPRITES_START+GhostsEaten_TILE_COUNT

#define GAMEPLAY 0
#define GAMEFIRSTLOAD 1
#define START_SCREEN 2
#define RELOAD_GAMESTATE 255


void GetSidesCanCheck(Character *character,uint8_t allowReverse);

#endif