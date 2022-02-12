#include <gb/gb.h>
#include "Gameplay/Character.h"
#include "util.h"

// Used for animating pacman (3 frames), the ghosts (2 frames), blinking the big dots, and blinking the 1 up text
uint8_t twoFrameAnimator=0;
uint8_t threeFrameAnimator=0;

// The current and previous state of the joypad
// multiple calls of the joypad() function should not be made per update
// The previous value can help determine when users JUST press, or JUST release
uint8_t joypadCurrent,joypadPrevious;
uint8_t level;

uint16_t score=0,highScore=0,dotsRemaining=0;

uint8_t TileSideWalkability[21][27][4];

// Good ole' pacman
Character pacman;

// Enabled using select
// Enables the "debug" x's showing a ghosts target tile
uint8_t enableDebug=0;

// Declare our 4 ghosts
Character ghosts[4] = {
    {0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0}
};


// We'll store the tile to use in VRAM in this variable
// When checking if a tile is walkable, this will be checked against
uint8_t blank;

// An eay way to get a directional vector using one of our direction values
// Up, Right, Down, Left respectively
const Vector2D Directions[]={{0,-1}, {1,0}, {0,1}, {-1,0}};

// Any easy way to get opposite directions
// Normal directions go: UP, RIGHT,DOWN, LEFT
const uint8_t reverseDirections[4]={DOWN,LEFT,UP,RIGHT};