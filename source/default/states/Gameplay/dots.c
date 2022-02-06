#include <gb/gb.h>
#include "util.h"
#include "states/Gameplay/dots.h"
#include "states/Gameplay/ghosts.h"
#include "states/Gameplay/pacman.h"
#include "states/Gameplay/hud.h"
#include "states/Gameplay/Character.h"
#include "graphics/GhostsEaten.h"
#include "graphics/GhostsScared.h"
#include "graphics/Pellets.h"
#include "graphics/Ghosts.h"
#include "graphics/Pacman.h"
#include "graphics/Numbers.h"
#include "graphics/Map.h"
#include "graphics/HighText.h"
#include "graphics/ReadyText.h"
#include "graphics/OneUpText.h"
#include "common.h"


const Vector2D BigPelletPositions[4]={{1,3},{19,3},{1,20},{19,20}};

uint8_t BigPelletsActive[4]={1,1,1,1};

void SetupDots(){

    ballsRemaining=0;

    BigPelletsActive[0]=1;
    BigPelletsActive[1]=1;
    BigPelletsActive[2]=1;
    BigPelletsActive[3]=1;

    for(uint8_t i=0;i<21;i++){
        for(uint8_t j=0;j<27;j++){


            uint8_t mid = i>=6&&j>=6&&i<15&&j<=17;
            uint8_t left = i<=4&&j>=9&&j<=17;
            uint8_t right = i>=17&&j>=9&&j<=17;

            // We do not want to generate dots/pellets
            // in these sections/locations.
            if(j==3&&i==3)continue;
            if(j==3&&i==7)continue;
            if(j==3&&i==13)continue;
            if(j==3&&i==17)continue;
            if(mid)continue;
            if(left)continue;
            if(right)continue;

            uint8_t current =get_bkg_tile_xy(i,j);

            if(current==blank){
                VBK_REG=1; set_bkg_tile_xy(i,j,0);
                VBK_REG=0; set_bkg_tile_xy(i,j,PELLETS_TILES_START);
                ballsRemaining++;
            }
        }
    }
}

void HandleDotConsumption(){
    
    uint8_t tile = get_bkg_tile_xy(pacman.column,pacman.row);

    // If this is a big or small pellet
    if(tile==PELLETS_TILES_START||tile==PELLETS_TILES_START+1){

        
        NR21_REG=0x87;
        NR22_REG=0x65;
        NR23_REG=0xf8;
        NR24_REG=0x86;

        if(tile==PELLETS_TILES_START+1){
            for(uint8_t i=0;i<4;i++){

                // BOO!!!!!!
                ghosts[i].state=FRIGHTENED;

                // Try to change directions
                TryChangeDirection(&ghosts[i],reverseDirections[ghosts[i].direction]);
            }
            
            for(uint8_t i=0;i<4;i++){
                if(pacman.column==BigPelletPositions[i].x&&pacman.row==BigPelletPositions[i].y){
                    BigPelletsActive[i]=0;
                    break;
                }
            }
        }

        score++;
        if(score>highScore)highScore=score;
        UpdateScore();
        ballsRemaining--;
        set_bkg_tile_xy(pacman.column,pacman.row,blank);

    }
}