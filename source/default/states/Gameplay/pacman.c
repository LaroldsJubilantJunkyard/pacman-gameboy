#include <gb/gb.h>
#include <gb/metasprites.h>
#include "util.h"
#include "states/Gameplay/Character.h"
#include "states/Gameplay/pacman.h"
#include "common.h"
#include "graphics/Pacman.h"
#include "graphics/Map.h"
#include "graphics/Pellets.h"
#include "graphics/PacmanEatenLeft.h"
#include "graphics/PacmanEatenRight.h"
#include "graphics/PacmanEatenUp.h"
#include "graphics/PacmanEatenDown.h"



void SetupPacman(){
    

    // Make sure pacman's normaltiles are set
    // At the end of gameplay, we change these to a death animation
    // Ensure they are in their normal state
    set_sprite_data(0,Pacman_TILE_COUNT,Pacman_tiles);

    // Default pacman's position and motion and state
    pacman.column=10;
    pacman.move=0;
    pacman.row=20;
    pacman.state=1;
    pacman.direction=RIGHT;
}


void UpdatePacman(){

    // if the d-pad is down on the joypad
    if(joypadCurrent & (J_UP | J_DOWN | J_LEFT | J_RIGHT)){

        // Get our direction
        uint8_t dir = UP;
        if(joypadCurrent & J_DOWN)dir=DOWN;
        else if(joypadCurrent & J_LEFT)dir=LEFT;
        else if(joypadCurrent & J_RIGHT)dir=RIGHT;

        // Change direction
        // This will fail if they cannot
        TryChangeDirection(&pacman,dir);
    }

    // Move pacman forward
    MoveForward(&pacman,10);
    DrawPacman();
}

void DrawPacman(){
    
    DrawCharacter(&pacman,Pacman_metasprites[pacman.direction*3+threeFrameAnimator],0,0);
}


void PacmanDeathAnimation_Halting(){
    
    NR10_REG=0x7C;
    NR11_REG=0xCB;
    NR12_REG=0x67;
    NR13_REG=0xA6;
    NR14_REG=0x86;

    metasprite_t const *pacmanEatenMetasprites;

    // Use the tile data from the propre direction
    switch(pacman.direction){
        case DOWN: set_sprite_data(0,PacmanEatenDown_TILE_COUNT,PacmanEatenDown_tiles); break;
        case UP: set_sprite_data(0,PacmanEatenUp_TILE_COUNT,PacmanEatenUp_tiles); break;
        case LEFT: set_sprite_data(0,PacmanEatenLeft_TILE_COUNT,PacmanEatenLeft_tiles); break;
        case RIGHT: set_sprite_data(0,PacmanEatenRight_TILE_COUNT,PacmanEatenRight_tiles); break;
    }


    uint16_t screenX=(pacman.column*8+Directions[pacman.direction].x*(pacman.move>>4))-cameraX;
    uint16_t screenY=(pacman.row*8+Directions[pacman.direction].y*(pacman.move>>4))-cameraY;

    // Since some othe metasprites only use one sprites
    // Hide these by default, so they don't accidentally show when unused by the metasprite
    move_sprite(0,0,0);
    move_sprite(1,0,0);

    for(uint8_t i=0;i<8;i++){        

        // Use the tile data from the propre direction
        switch(pacman.direction){
            case DOWN: move_metasprite(PacmanEatenDown_metasprites[i],0,0,screenX+12,screenY+20); break;
            case UP: move_metasprite(PacmanEatenUp_metasprites[i],0,0,screenX+12,screenY+20); break;
            case LEFT: move_metasprite(PacmanEatenLeft_metasprites[i],0,0,screenX+12,screenY+20); break;
            case RIGHT: move_metasprite(PacmanEatenRight_metasprites[i],0,0,screenX+12,screenY+20); break;
        }        

        wait_vbl_done();
        wait_vbl_done();
        wait_vbl_done();
    }

    move_sprite(0,0,0);
    move_sprite(1,0,0);

    delay(1000);
}
