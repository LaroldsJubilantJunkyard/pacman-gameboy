#include <gb/gb.h>
#include <gb/hardware.h>
#include "util.h"

uint8_t lastValue=0;

uint8_t RandomNumber(uint8_t min, uint8_t max){

    // Use the Divider Register, the first sprite for pacman, the ghosts, and the last value
    uint8_t v = (DIV_REG)+shadow_OAM[0].x+shadow_OAM[2].x+shadow_OAM[4].x+shadow_OAM[6].x+shadow_OAM[8].x+lastValue;
    
    // Get the new value
    // Save for next
    lastValue= min+(v % (max-min)); 

    return lastValue;
}

uint16_t getSquareDistance(uint8_t columnA, uint8_t rowA, uint8_t columnB, uint8_t rowB){
    
    return ((uint16_t)columnA-(uint16_t)columnB)*((uint16_t)columnA-(uint16_t)columnB)+ ((uint16_t)rowA-(uint16_t)rowB)*((uint16_t)rowA-(uint16_t)rowB);
}


