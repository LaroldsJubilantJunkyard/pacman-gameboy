
#include <gb/gb.h>
#include <gb/metasprites.h>
#include "common.h"
#include "Gameplay/hud.h"
#include "Gameplay/ghosts.h"
#include "Gameplay/pacman.h"
#include "Gameplay/dots.h"
#include "util.h"
#include "graphics/GhostsEaten.h"
#include "graphics/GhostsScared.h"
#include "graphics/Dots.h"
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
#include "graphics/BlackSquare.h"
#include "graphics/OneUpText.h"
#include "graphics/TargetTiles.h"

uint8_t counter;


void UpdateCameraPosition(uint8_t dividor){

    // Get pacman's position
    uint8_t playerX=pacman.column*8+Directions[pacman.direction].x*(pacman.move>>4);;
    uint8_t playerY=pacman.row*8+Directions[pacman.direction].y*(pacman.move>>4);;

    uint8_t sidebarWidth=56;
    uint8_t heightRemainder=Map_HEIGHT-DEVICE_SCREEN_HEIGHT*8;

    uint16_t targetCameraX=SCX_REG;
    uint16_t targetCameraY=SCY_REG;

    // Make the map follow pacman when in the middle third
    if(playerX<Map_WIDTH/3)targetCameraX=0;
    else if(playerX<(Map_WIDTH/3)*2)targetCameraX=((playerX-Map_WIDTH/3)*sidebarWidth)/(Map_WIDTH/3);
    else targetCameraX=sidebarWidth;
    
    // Make the map follow pacman when in the middle third
    if(playerY<Map_HEIGHT/3)targetCameraY=0;
    else if(playerY<(Map_HEIGHT/3)*2)targetCameraY=((playerY-Map_HEIGHT/3)*heightRemainder)/(Map_HEIGHT/3);
    else targetCameraY=heightRemainder;

    // The amount of change to get to the target position
    int16_t cameraXDiff=targetCameraX-SCX_REG;
    int16_t cameraYDiff=targetCameraY-SCY_REG;

    // Divide it some, so it's not instant
    SCX_REG+=cameraXDiff/dividor;
    SCY_REG+=cameraYDiff/dividor;
}
void SetupVRAM(){
    

    // Use the palettes from these two graphics
    // For simplicity, all sprite PNGs use the same palette set
    // For simplicity, all background/map PNGs use the same palette set
    set_sprite_palette(0,5,Pacman_palettes);
    set_bkg_palette(0,5,Map_palettes);

    // Pur our backround tiles in VRAM
    set_bkg_data(0,Map_TILE_COUNT,Map_tiles);
    set_bkg_data(DOTS_TILES_START,Dots_TILE_COUNT,Dots_tiles);
    set_bkg_data(NUMBERS_TILES_START,Numbers_TILE_COUNT,Numbers_tiles);
    set_bkg_data(READYTEXT_TILES_START,ReadyText_TILE_COUNT,ReadyText_tiles);
    set_bkg_data(ONEUPTEXT_TILES_START,OneUpText_TILE_COUNT,OneUpText_tiles);
    set_bkg_data(HIGHTEXT_TILES_START,HighText_TILE_COUNT,HighText_tiles);
    set_bkg_data(BLACKSQUARE_TILES_START,BlackSquare_TILE_COUNT,BlackSquare_tiles);

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
}

void StartGameplay(){

    // Show the ready text (above pacman, below the ghost pit)
    VBK_REG=1; set_bkg_tiles(8,15,6,1,ReadyText_map_attributes);
    VBK_REG=0; set_bkg_based_tiles(8,15,6,1,ReadyText_map,READYTEXT_TILES_START);

    delay(1000);

    // Open up the gap forthe ghosts
    // Inky, pinky, and clyde will not be able to escape
    VBK_REG=1; set_bkg_tile_xy(10,11,0);
    VBK_REG=0; set_bkg_tile_xy(10,11,blank);

    // Fully hide the blocker's edges
    // This won't affet their pathfinding, it's just for asthetics
    // Were going to borrow two of the tiles for the same type of edge
    set_bkg_tile_xy(9,11,get_bkg_tile_xy(8,9));
    set_bkg_tile_xy(11,11,get_bkg_tile_xy(12,9));

    // cover up our ready text
    VBK_REG=1; fill_bkg_rect(8,15,6,1,1);
    VBK_REG=0; fill_bkg_rect(8,15,6,1,blank);

    twoFrameAnimator=0;
    threeFrameAnimator=0;
}


uint8_t CheckBackgroundTileIsWalkable(int8_t nextColumn, int8_t nextRow){

    if(nextColumn==10&&nextRow==11)return TRUE;

    // To allow looping around, consider horizontally out of bound tiles walkable
    if(nextColumn>=Map_WIDTH/8||nextColumn<0){
        return TRUE;
    }

    // If this tile is blank, or one of the dots, it is walkabble
    return get_bkg_tile_xy(nextColumn,nextRow)==blank ||
        get_bkg_tile_xy(nextColumn,nextRow)==DOTS_TILES_START||
        get_bkg_tile_xy(nextColumn,nextRow)==DOTS_TILES_START+1;
}


void SetupGameplay(){

    // This enables Sound
    // these registers must be in this specific order!
    NR52_REG = 0x80; // is 1000 0000 in binary and turns on sound
    NR50_REG = 0x77; // sets the volume for both left and right channel just set to max 0x77
    NR51_REG = 0xFF; // is 1111 1111 in binary, select which chanels we want to use in this case all of them. One bit for the L one bit for the R of all four channels

    DISPLAY_ON;
    SHOW_SPRITES;
    SPRITES_8x16;
    SHOW_BKG;
    SHOW_WIN;  

    score=0;

    SetupVRAM();
    SetupHUD();
    SetupDots();
    SetupPacman();    
    SetupGhosts();
    
    // We want all the ghosts, the camera, and pacman in position before the upcoming "ready" delay
    // Otherwise, they'll jump to the correct location after it's completed
    UpdateScore();
    UpdateCameraPosition(1);
    DrawGhost(0);
    DrawGhost(1);
    DrawGhost(2);
    DrawGhost(3);
    DrawPacman();

    for(uint8_t i=0;i<21;i++){
        for(uint8_t j=0;j<27;j++){

            TileSideWalkability[i][j][DOWN]=CheckBackgroundTileIsWalkable(i,j+1);
            TileSideWalkability[i][j][UP]=CheckBackgroundTileIsWalkable(i,j-1);
            TileSideWalkability[i][j][RIGHT]=CheckBackgroundTileIsWalkable(i+1,j);
            TileSideWalkability[i][j][LEFT]=CheckBackgroundTileIsWalkable(i-1,j);
        }
    }
    

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
        }
    }
}

void BlinkLevelBlueAndWhite_Halting(){

    // We'll flash on/off 10 times.
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

void main(){

    SetupGameplay();
    StartGameplay();


    while(TRUE){
        

        joypadPrevious=joypadCurrent;
        joypadCurrent=joypad();

        // Allow enabling a debug mode
        if((joypadCurrent & J_SELECT) && !(joypadPrevious & J_SELECT)){
            enableDebug = !enableDebug;
        }

        UpdateGlobalFrameCounters();
        UpdateHUD();
        UpdateAllGhosts();
        UpdatePacman();
        HandleDotConsumption();
        UpdateCameraPosition(5);

        // If pacman is dead, pereform his death animation
        // NOTE: this function contains a while loop and some wait_vbl_done's
        // Thus why we added _halting on it's name for clarity
        if(pacman.state==0){

            // If we are not yet resetting
            // Show the pacman death animation
            if(!ghostsResetting)PacmanDeathAnimation_Halting();
            ghostsResetting=1;


            // Setup gamelay
            if(ghostsReady){
                SetupGameplay();
                StartGameplay();
            }
        }

        // If there are no more balls remaining
        // We want to blink the screen blue and white in celebration
        // NOTE: this function contains a while loop and some wait_vbl_done's
        // Thus why we added _halting on it's name for clarity
        else if(dotsRemaining==0){
            BlinkLevelBlueAndWhite_Halting();
            SetupGameplay();
            StartGameplay();
        }

        wait_vbl_done();
    
    }
}
