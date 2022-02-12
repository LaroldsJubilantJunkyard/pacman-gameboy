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

uint8_t BigDotsActive[4]={1,1,1,1};
uint8_t dotsShown=FALSE;

void SetupDots(){

    // Default at 0
    // The following for-loops will increment
    dotsRemaining=0;

    // All dots set to active
    BigDotsActive[0]=1;
    BigDotsActive[1]=1;
    BigDotsActive[2]=1;
    BigDotsActive[3]=1;

    for(uint8_t i=0;i<21;i++){
        for(uint8_t j=0;j<27;j++){

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


void ShowOrHideActiveBigDots(uint8_t show){

    // For each big pellet
    for(uint8_t i=0;i<4;i++){

        
        if(show){

            // If it is active
            if(BigDotsActive[i]){
            
                // Draw the pellet
                VBK_REG=1; set_bkg_tile_xy(BigPelletPositions[i].x,BigPelletPositions[i].y,0);
                VBK_REG=0; set_bkg_tile_xy(BigPelletPositions[i].x,BigPelletPositions[i].y,DOTS_TILES_START+1);
            }

        }else{
            

            // If it is active
            if(BigDotsActive[i]){
            
                // Draw the pellet
                VBK_REG=1; set_bkg_tile_xy(BigPelletPositions[i].x,BigPelletPositions[i].y,0);
                VBK_REG=0; set_bkg_tile_xy(BigPelletPositions[i].x,BigPelletPositions[i].y,DOTS_TILES_START+1);
            }
        }
    }
}

void HandleDotConsumption(){
    
    // Get the tile pacman is on
    uint8_t tile = get_bkg_tile_xy(pacman.column,pacman.row);

    // Every cycle of the two frame animator 
    // We'll hide/show the big dots
    if(twoFrameAnimator==0){

        dotsShown=!dotsShown;
     
        ShowOrHideActiveBigDots(dotsShown);
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
            
            for(uint8_t i=0;i<4;i++){
                if(pacman.column==BigPelletPositions[i].x&&pacman.row==BigPelletPositions[i].y){
                    BigDotsActive[i]=0;
                    break;
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