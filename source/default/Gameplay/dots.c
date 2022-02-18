#include <gb/gb.h>
#include "util.h"
#include "Gameplay/dots.h"
#include "Gameplay/ghosts.h"
#include "Gameplay/pacman.h"
#include "Gameplay/hud.h"
#include "Gameplay/Character.h"
#include "graphics/GhostsEaten.h"
#include "graphics/GhostsScared.h"
#include "graphics/Dots.h"
#include "graphics/Ghosts.h"
#include "graphics/Pacman.h"
#include "graphics/Numbers.h"
#include "graphics/Map.h"
#include "graphics/HighText.h"
#include "graphics/ReadyText.h"
#include "graphics/OneUpText.h"
#include "common.h"


const Vector2D BigPelletPositions[4]={{1,3},{19,3},{1,20},{19,20}};

uint8_t dotsShown=FALSE;

void SetupDots(){

    // Default at 0
    // The following for-loops will increment
    dotsRemaining=0;

    // For each big pellet
    for(uint8_t i=0;i<4;i++){
            
        // Draw the pellet
        VBK_REG=1; set_bkg_tile_xy(BigPelletPositions[i].x,BigPelletPositions[i].y,0);
        VBK_REG=0; set_bkg_tile_xy(BigPelletPositions[i].x,BigPelletPositions[i].y,DOTS_TILES_START+1);
    }

    for(uint8_t i=0;i<Map_WIDTH/8;i++){
        for(uint8_t j=0;j<Map_HEIGHT/8;j++){

            // These are areas where we do not want to spawn dots
            uint8_t mid = i>=6&&j>=6&&i<15&&j<=17;
            uint8_t left = i<=4&&j>=9&&j<=17;
            uint8_t right = i>=17&&j>=9&&j<=17;

            // We do not want to generate dots/dots
            // In these locations
            if(j==3&&i==3)continue;
            if(j==3&&i==7)continue;
            if(j==3&&i==13)continue;
            if(j==3&&i==17)continue;

            // We do not want to generate dots/dots
            //in these sections
            if(mid)continue;
            if(left)continue;
            if(right)continue;

            uint8_t current =get_bkg_tile_xy(i,j);

            if(current==blank){
                VBK_REG=1; set_bkg_tile_xy(i,j,0);
                VBK_REG=0; set_bkg_tile_xy(i,j,DOTS_TILES_START);
                dotsRemaining++;
            }
        }
    }
}


void HandleDotConsumption(){
    
    // Get the tile pacman is on
    uint8_t tile = get_bkg_tile_xy(pacman.column,pacman.row);

    // Every cycle of the two frame animator 
    // We'll hide/show the big dots
    if(threeFrameAnimator==0){

        dotsShown=!dotsShown;

        // Switch the tile in VRAM between the 2nd and third tile in the Dots graphic
        // The third tile is blank, the second is the power pellet
        if(dotsShown)set_bkg_data(DOTS_TILES_START+1,1,Dots_tiles+16);
        else set_bkg_data(DOTS_TILES_START+1,1,Dots_tiles+32);
    }

    // If this is a big or small pellet
    if(tile==DOTS_TILES_START||tile==DOTS_TILES_START+1){

        
        NR21_REG=0x87;
        NR22_REG=0x65;
        NR23_REG=0xf8;
        NR24_REG=0x86;

        if(tile==DOTS_TILES_START+1){
            frightenedTimer=500;
            for(uint8_t i=0;i<4;i++){

                // BOO!!!!!!
                // But only if we are not already eaten
                if(ghosts[i].state==SCATTERCHASE){
                    ghosts[i].state=FRIGHTENED;

                    // Try to change directions
                    TryChangeDirection(&ghosts[i],reverseDirections[ghosts[i].direction]);
                }
            }
        }

        score++;

        // Possibly update our high score
        if(score>highScore)highScore=score;

        UpdateScore();

        dotsRemaining--;

        // Set the current tile as blank
        set_bkg_tile_xy(pacman.column,pacman.row,blank);

    }
}