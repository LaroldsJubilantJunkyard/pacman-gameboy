#include <gb/gb.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "graphics/Map.h"
#include "states/Gameplay/hud.h"
#include "graphics/Map.h"
#include "graphics/Numbers.h"
#include "graphics/Pellets.h"

void UpdateScore(){
    
    DrawNumber(1,1,highScore,4,2);
    DrawNumber(1,4,score,4,0);
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