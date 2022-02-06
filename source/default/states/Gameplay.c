
#include <gb/gb.h>
#include <gb/metasprites.h>
#include "common.h"
#include "hud.h"
#include "ghosts.h"
#include "pacman.h"
#include "dots.h"
#include "util.h"
#include "graphics/GhostsEaten.h"
#include "graphics/GhostsScared.h"
#include "graphics/Pellets.h"
#include "graphics/Ghosts.h"
#include "graphics/Pacman.h"
#include "graphics/PacmanEatenLeft.h"
#include "graphics/PacmanEatenRight.h"
#include "graphics/PacmanEatenUp.h"
#include "graphics/PacmanEatenDown.h"
#include "graphics/Numbers.h"
#include "graphics/Map.h"
#include "graphics/HighText.h"
#include "graphics/ReadyText.h"
#include "graphics/OneUpText.h"
#include "graphics/TargetTiles.h"

uint8_t counter;


void UpdateCameraPosition(uint8_t dividor){
    uint8_t playerX=pacman.column*8+Directions[pacman.direction].x*(pacman.move>>4);;
    uint8_t playerY=pacman.row*8+Directions[pacman.direction].y*(pacman.move>>4);;

    uint8_t sidebarWidth=56;
    uint8_t heightRemainder=Map_HEIGHT-DEVICE_SCREEN_HEIGHT*8;

    uint16_t targetCameraX=cameraX;
    uint16_t targetCameraY=cameraY;

    
    if(playerX<Map_WIDTH/3)targetCameraX=0;
    else if(playerX<(Map_WIDTH/3)*2)targetCameraX=((playerX-Map_WIDTH/3)*sidebarWidth)/(Map_WIDTH/3);
    else targetCameraX=sidebarWidth;

    
    if(playerY<Map_HEIGHT/3)targetCameraY=0;
    else if(playerY<(Map_HEIGHT/3)*2)targetCameraY=((playerY-Map_HEIGHT/3)*heightRemainder)/(Map_HEIGHT/3);
    else targetCameraY=heightRemainder;

    int16_t cameraXDiff=targetCameraX-cameraX;
    int16_t cameraYDiff=targetCameraY-cameraY;
    cameraX+=cameraXDiff/dividor;
    cameraY+=cameraYDiff/dividor;

    move_bkg(cameraX,cameraY);
}



void SetupGameplay(){

    cameraX=0;
    cameraY=0;
    score=0;

    // Move the window to the right side
    move_win(120,0);    

    // Use the palettes from these two graphics
    // For simplicity, all sprite PNGs use the same palette set
    // For simplicity, all background/map PNGs use the same palette set
    set_sprite_palette(0,5,Pacman_palettes);
    set_bkg_palette(0,3,Map_palettes);

    // Pur our backround tiles in VRAM
    set_bkg_data(0,Map_TILE_COUNT,Map_tiles);
    set_bkg_data(PELLETS_TILES_START,Pellets_TILE_COUNT,Pellets_tiles);
    set_bkg_data(NUMBERS_TILES_START,Numbers_TILE_COUNT,Numbers_tiles);
    set_bkg_data(READYTEXT_TILES_START,ReadyText_TILE_COUNT,ReadyText_tiles);
    set_bkg_data(ONEUPTEXT_TILES_START,OneUpText_TILE_COUNT,OneUpText_tiles);
    set_bkg_data(HIGHTEXT_TILES_START,HighText_TILE_COUNT,HighText_tiles);

    // Pur our sprite tiles in VRAM
    set_sprite_data(GHOSTS_SPRITES_START,Pacman_TILE_COUNT,Ghosts_tiles);
    set_sprite_data(GHOSTS_SCARED_SPRITES_START,GhostsScared_TILE_COUNT,GhostsScared_tiles);
    set_sprite_data(GHOSTS_EATEN_SPRITES_START,GhostsEaten_TILE_COUNT,GhostsEaten_tiles);
    set_sprite_data(TARGET_TILES_SPRITES_START,TargetTiles_TILE_COUNT,TargetTiles_tiles);

    // Draw the map
    VBK_REG=1; set_bkg_tiles(0,0,21,27,Map_map_attributes);
    VBK_REG=0; set_bkg_tiles(0,0,21,27,Map_map);

    // Get the tile we'll use at the blank tile 
    blank=get_bkg_tile_xy(10,13);

    // Fill the window with blank tiles
    VBK_REG=1; fill_win_rect(0,0,20,18,1|128);
    VBK_REG=0; fill_win_rect(0,0,20,18,blank);

    // Show the high (score) text
    VBK_REG=1; set_win_based_tiles(1,0,4,1,HighText_map_attributes,128);
    VBK_REG=0; set_win_based_tiles(1,0,4,1,HighText_map,HIGHTEXT_TILES_START);
    
    // Show the one up text
    VBK_REG=1; set_win_based_tiles(2,3,3,1,OneUpText_map_attributes,128);
    VBK_REG=0; set_win_based_tiles(2,3,3,1,OneUpText_map,ONEUPTEXT_TILES_START);

    UpdateScore();
    SetupDots();
    SetupPacman();    
    SetupGhosts();
    
    // We want all the ghosts, the camera, and pacman in position before the upcoming "ready" delay
    UpdateCameraPosition(1);
    DrawGhost(0);
    DrawGhost(1);
    DrawGhost(2);
    DrawGhost(3);
    DrawPacman();

    // Show the ready text (above pacman, below the ghost pit)
    VBK_REG=1; set_bkg_tiles(8,15,6,1,ReadyText_map_attributes);
    VBK_REG=0; set_bkg_based_tiles(8,15,6,1,ReadyText_map,READYTEXT_TILES_START);

    delay(1000);

    // Open up the gap forthe ghosts
    VBK_REG=1; set_bkg_tile_xy(10,11,0);
    VBK_REG=0; set_bkg_tile_xy(10,11,blank);

    // Fully hide the blocker's edges
    set_bkg_tile_xy(9,11,get_bkg_tile_xy(8,9));
    set_bkg_tile_xy(11,11,get_bkg_tile_xy(12,9));

    // cover up our ready text

    VBK_REG=1; fill_bkg_rect(8,15,6,1,1);
    VBK_REG=0; fill_bkg_rect(8,15,6,1,blank);

    counter=0;
    twoFrameAnimator=0;
    threeFrameAnimator=0;

}

void UpdateGlobalFrameCounters(){
    

    counter++;
    if(counter>=5){
        counter=0;
        twoFrameAnimator++;
        threeFrameAnimator++;
        if(threeFrameAnimator>2){
            threeFrameAnimator=0;
        }
        if(twoFrameAnimator>1){
            twoFrameAnimator=0;
            
            // Show the 1up text
            VBK_REG=1; set_win_based_tiles(2,3,3,1,OneUpText_map_attributes,128);
            VBK_REG=0; set_win_based_tiles(2,3,3,1,OneUpText_map,ONEUPTEXT_TILES_START);

            // For each big pellet
            for(uint8_t i=0;i<4;i++){

                // If it is active
                if(BigPelletsActive[i]){
                
                    // Draw the pellet
                    VBK_REG=1; set_bkg_tile_xy(BigPelletPositions[i].x,BigPelletPositions[i].y,0);
                    VBK_REG=0; set_bkg_tile_xy(BigPelletPositions[i].x,BigPelletPositions[i].y,PELLETS_TILES_START+1);
                }
            }

        }else{
            
            // Hide the 1up text
            VBK_REG=1; fill_win_rect(2,3,3,1,1|128);
            VBK_REG=0; fill_win_rect(2,3,3,1,blank);

            // For each big pellet
            for(uint8_t i=0;i<4;i++){
                
                // Hide the pellet using our blank tile
                VBK_REG=1; set_bkg_tile_xy(BigPelletPositions[i].x,BigPelletPositions[i].y,1);
                VBK_REG=0; set_bkg_tile_xy(BigPelletPositions[i].x,BigPelletPositions[i].y,blank);
            }
        }
    }
}

void BlinkLevelBlueAndWhite_Halting((){
    for(uint8_t i=0;i<10;i++){

        // The palette item for blue (in the first, default, palette) is white in the third color palette
        // Swap between these two to create the level complete blinking effect
        if(i%2==0)set_bkg_palette(0,1,Map_palettes+8);
        else set_bkg_palette(0,1,Map_palettes);

        // Wait a little bit
        // Could be some wait_vbl_done's, but i like the control
        // Also, at this point, the level is done.
        delay(250);
    }

    // Make sure we restore the original color palettes
    set_bkg_palette(0,1,Map_palettes);
}

uint8_t UpdateGameplay(){

    // Allow enabling a debug mode
    if((joypadCurrent & J_SELECT) && !(joypadPrevious & J_SELECT)){
        enableDebug = !enableDebug;
    }

    UpdateGlobalFrameCounters();
    UpdateAllGhosts();
    UpdatePacman();
    HandleDotConsumption();
    UpdateCameraPosition(5);

    // If pacman is dead, pereform his death animation
    // NOTE: this function contains a while loop and some wait_vbl_done's
    // Thus why we added _halting on it's name for clarity
    if(pacman.state==0){
        PacmanDeathAnimation_Halting();
        return RELOAD_GAMESTATE;
    }

    // If there are no more balls remaining
    // We want to blink the screen blue and white in celebration
    // NOTE: this function contains a while loop and some wait_vbl_done's
    // Thus why we added _halting on it's name for clarity
    else if(ballsRemaining==0){

        BlinkLevelBlueAndWhite_Halting(();

        return RELOAD_GAMESTATE;
    }
    
   
    return GAMEPLAY;
}
