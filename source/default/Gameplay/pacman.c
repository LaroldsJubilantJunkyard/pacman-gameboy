#include <gb/gb.h>
#include <gb/metasprites.h>
#include "util.h"
#include "Gameplay/Character.h"
#include "Gameplay/pacman.h"
#include "common.h"
#include "graphics/Pacman.h"
#include "graphics/Map.h"
#include "graphics/Dots.h"
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

    if(pacman.state!=0){

        // Move pacman forward
        MoveForward(&pacman,10);
        DrawPacman();
    }

}

void DrawPacman(){
    
    // Draw our pacman metasprite
    // We have this in a function so we can make sure pacman is properly drawn from wherever
    DrawCharacter(&pacman,Pacman_metasprites[pacman.direction*3+threeFrameAnimator],0,0);
}


void PacmanDeathAnimation_Halting(){
    
    NR10_REG=0x7C;
    NR11_REG=0xCB;
    NR12_REG=0x67;
    NR13_REG=0xA6;
    NR14_REG=0x86;

    // We have an eaten animation for each direction
    // Use the tile data from the proprer direction
    switch(pacman.direction){
        case DOWN: set_sprite_data(0,PacmanEatenDown_TILE_COUNT,PacmanEatenDown_tiles); break;
        case UP: set_sprite_data(0,PacmanEatenUp_TILE_COUNT,PacmanEatenUp_tiles); break;
        case LEFT: set_sprite_data(0,PacmanEatenLeft_TILE_COUNT,PacmanEatenLeft_tiles); break;
        case RIGHT: set_sprite_data(0,PacmanEatenRight_TILE_COUNT,PacmanEatenRight_tiles); break;
    }


    // Since some othe metasprites only use one sprites
    // Hide these by default, so they don't accidentally show when unused by the metasprite
    move_sprite(0,0,0);
    move_sprite(1,0,0);

    for(uint8_t i=0;i<8;i++){        

        // Draw the proper metasprites depending on which direction we are facing
        switch(pacman.direction){
            case DOWN: DrawCharacter(&pacman,PacmanEatenDown_metasprites[i],0,0); break;
            case UP: DrawCharacter(&pacman,PacmanEatenUp_metasprites[i],0,0); break;
            case LEFT: DrawCharacter(&pacman,PacmanEatenLeft_metasprites[i],0,0); break;
            case RIGHT: DrawCharacter(&pacman,PacmanEatenRight_metasprites[i],0,0); break;
        }        

        wait_vbl_done();
        wait_vbl_done();
        wait_vbl_done();
    }

    move_sprite(0,0,0);
    move_sprite(1,0,0);

    pacman.column=10;
    pacman.row=20;
    pacman.move=0;
    pacman.direction=RIGHT;
}
