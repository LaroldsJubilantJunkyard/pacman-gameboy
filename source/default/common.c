#include <gb/gb.h>
#include "states/Gameplay/Character.h"
#include "util.h"

uint8_t twoFrameAnimator=0,joypadCurrent,joypadPrevious;
uint8_t threeFrameAnimator=0;
uint8_t cameraX,cameraY,level;

uint16_t score=0,highScore=0,ballsRemaining=0;

Character pacman;

uint8_t enableDebug=0;

Character ghosts[4] = {
    {0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0}
};



uint8_t blank;

// An eay way to get a directional vector using one of our direction values
// Up, Right, Down, Left respectively
const Vector2D Directions[]={{0,-1}, {1,0}, {0,1}, {-1,0}};
const uint8_t reverseDirections[4]={DOWN,LEFT,UP,RIGHT};