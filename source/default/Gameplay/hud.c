#include <gb/gb.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "graphics/Map.h"
#include "Gameplay/hud.h"
#include "graphics/Map.h"
#include "graphics/Numbers.h"
#include "graphics/Dots.h"
#include "graphics/HighText.h"
#include "graphics/OneUpText.h"
#include "graphics/BlackSquare.h"
#include "graphics/ReadyText.h"


void UpdateScore(){
    
    DrawNumber(1,1,highScore,4,2);
    DrawNumber(1,4,score,4,1);
}

void SetupHUD(){

    // Move the window to the right side
    move_win(120,0);  

    // Fill the window with blank tiles
    VBK_REG=1; fill_win_rect(0,0,20,18,0x80|3);
    VBK_REG=0; fill_win_rect(0,0,20,18,BLACKSQUARE_TILES_START);

    // Show the high (score) text
    VBK_REG=1; fill_win_rect(1,0,4,1,0x80|4);
    VBK_REG=0; set_win_based_tiles(1,0,4,1,HighText_map,HIGHTEXT_TILES_START);
    
    // Show the one up text
    VBK_REG=1; fill_win_rect(2,3,3,1,0x80|3);
    VBK_REG=0; set_win_based_tiles(2,3,3,1,OneUpText_map,ONEUPTEXT_TILES_START);

    UpdateScore();
}

void UpdateHUD(){

    if(threeFrameAnimator==0){

        // at 2,3 on the window we will draw the one up text
        // or blank, when flashed off
        // If this tile is blank we want to draw the text
        // otherwise we want to hide the text
        ShowOrHideOneUpText(get_win_tile_xy(2,3)==blank);
        
    }
}

void ShowOrHideOneUpText(uint8_t show){

    if(show){
        
        // Show the 1up text
        VBK_REG=1; set_win_based_tiles(2,3,3,1,OneUpText_map_attributes,128);
        VBK_REG=0; set_win_based_tiles(2,3,3,1,OneUpText_map,ONEUPTEXT_TILES_START);
    }else{

        // Hide the 1up text
        VBK_REG=1; fill_win_rect(2,3,3,1,1|128);
        VBK_REG=0; fill_win_rect(2,3,3,1,blank);
    }
}

void DrawNumber(uint8_t x,uint8_t y, uint16_t number,uint8_t digits,uint8_t palette){
	
    unsigned char buffer[]="00000000";

    // Convert the number to a decimal string (stored in the buffer char array)
    uitoa(number, buffer, 10);

    // Get the length of the number so we can add leading zeroes
    uint8_t len =strlen(buffer);

    // Add some leading zeroes
    // uitoa will not do this for us
    // Increase the VRAM address each iteration to move to the next tile
    for(uint8_t i=0;i<digits-len;i++){
        VBK_REG=1;
        set_win_tile_xy(x,y,palette|128);
        VBK_REG=0;
        set_win_tile_xy(x++,y,NUMBERS_TILES_START);
    }
        
    // Draw our number
    // Increase the VRAM address each iteration to move to the next tile
    for(uint8_t i=0;i<len;i++){
        VBK_REG=1;
        set_win_tile_xy(x,y,palette|128);
        VBK_REG=0;
        set_win_tile_xy(x++,y,(buffer[i]-'0')+NUMBERS_TILES_START);
    }
}