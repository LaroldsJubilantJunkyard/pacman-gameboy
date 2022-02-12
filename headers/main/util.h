#ifndef UTIL_HEADER
#define UTIL_HEADER set

#include <gb/gb.h>
#include "util.h"
#include "common.h"

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define CHASE 0
#define SCATTER 1
#define FRIGHTENED 2
#define EATEN 3
#define SCATTERCHASE 4

#define INKY 0 // blue
#define BLINKY 1 // red
#define PINKY 2
#define CLYDE 3 // orange

#define BIG_DOT_TILE DOTS_TILES_START+4
#define FRIGHTEND_ENEMY_TILE 21
#define EATEN_ENEMY_EYES 22
#define GHOSTS_DEFAULT_TILE 2
#define SMALL_DOT_TILE DOTS_TILES_START
#define FRIGHTEND_GHOST_PALETTE1 6
#define FRIGHTEND_GHOST_PALETTE2 0

#define GHOST_PIT_TOP 12
#define GHOST_PIT_LEFT 9
#define GHOST_PIT_BOTTOM 13
#define GHOST_PIT_RIGHT 11

#define GAMEBOY_TILE_SIZE 8

uint8_t RandomNumber(uint8_t min, uint8_t max);
uint16_t getSquareDistance(uint8_t columnA, uint8_t rowA, uint8_t columnB, uint8_t rowB);

#endif
