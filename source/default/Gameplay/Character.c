#include <gb/gb.h>
#include <gb/metasprites.h>
#include "common.h"
#include "Gameplay/Character.h"
#include "util.h"
#include "graphics/Map.h"
#include "graphics/Dots.h"

uint8_t directionsCharacterCanMoveIn[4] = {0,0,0,0},numberOfDirectionsCharacterCanMoveIn=0;

void GetDirectionsCharacterCanMoveIn(Character *character,uint8_t allowReverse){
    
    numberOfDirectionsCharacterCanMoveIn=0;

    for (uint8_t i = 0; i < 4; i++)
    {
        // Are we NOT reversing? OR can we reverse?
        uint8_t notReversingOrIsAllowed=(character->direction!=reverseDirections[i]||allowReverse);

        // If this tile is walkable, increase how many and save what direction it is
        if(CanCharacterMoveInThisDirection(character,i)&&notReversingOrIsAllowed) directionsCharacterCanMoveIn[numberOfDirectionsCharacterCanMoveIn++]=i;
    }
}

uint8_t CanCharacterMoveInThisDirection(Character *character, uint8_t direction){
        
    int8_t nextColumn=character->column+Directions[direction].x;
    int8_t nextRow=character->row+Directions[direction].y;

    // To allow looping around, consider horizontally out of bound tiles walkable
    if(nextColumn>=Map_WIDTH/8||nextColumn<0){
        
        // Don't allow vertical movement
        if(direction==UP||direction==DOWN)return FALSE;
        return TRUE;
    }

    return TileSideWalkability[character->column][character->row][direction];
}


void TryChangeDirection(Character *character, uint8_t nextDirection){

    // If this is the same direction, do nothing
    if(character->direction==nextDirection)return;

    // This could be adjusted to make things more user-friendly
    // but, some 'snapping' may occur
    // The move variable's value is shifted to the left 4 bits for higher-precision
    // 128 = 8<<4
    // We're checking if the player has moved more than 8 pixels
    uint8_t aligned = character->move==0||character->move>=128;
    
    uint8_t changingAxis=
        (character->direction==LEFT&&(nextDirection==UP||nextDirection==DOWN))||
        (character->direction==RIGHT&&(nextDirection==UP||nextDirection==DOWN))||
        (character->direction==UP&&(nextDirection==LEFT||nextDirection==RIGHT))||
        (character->direction==DOWN&&(nextDirection==LEFT||nextDirection==RIGHT));

    uint8_t canMoveThisWay=CanCharacterMoveInThisDirection(character,nextDirection);

    // If the tile in this direction is walkable
    if(canMoveThisWay){

        // If we are changing axis and aligned 
        if(changingAxis && aligned){

            character->direction=nextDirection;

            // Reset how far we've moved
            character->move=0;

        // If we are NOT changing axis, ok
        }else if(!changingAxis){

            // Move in the given direction
            character->column+=Directions[character->direction].x;
            character->row+=Directions[character->direction].y;

            character->direction=nextDirection;

            // Use the inverse value for move
            character->move=(128-character->move);

        }
    }
}

uint8_t MoveForward(Character *character, uint8_t speed){

    if(CanCharacterMoveInThisDirection(character,character->direction)){

        // Move forward some
        character->move+=speed;

        // If this character has moved one tile away
        // The move variable's value is shifted to the left 4 bits for higher-precision
        // 128 = 8<<4
        // We're checking if the player has moved more than 8 pixels
        if(character->move>=128){

            // Reset our move
            character->move=0;

            // For looping around
            if(character->column==0&&character->direction==LEFT){
                character->column=Map_WIDTH/8+1;
            }else if(character->column==Map_WIDTH/8&&character->direction==RIGHT){
                character->column=-1;
            }else{

                // Move in the given direction
                character->column+=Directions[character->direction].x;
                character->row+=Directions[character->direction].y;
            }

            return 1;
        }

    }

    return 0;
}

void DrawCharacter(Character *character, metasprite_t const *metasprites, uint8_t baseSprite, uint8_t baseTile){

    // Get our screenX and screenY
    // Take our world position and subtract the camera's position to get our screen position
    uint16_t screenX=(character->column*8+Directions[character->direction].x*(character->move>>4))-SCX_REG;
    uint16_t screenY=(character->row*8+Directions[character->direction].y*(character->move>>4))-SCY_REG;


    // Sprites are draw at the specified x,y position, MINUS 8 and 16
    // So a sprite draw at 0,0 will not be shown
    // To draw a sprite at 0,0, it's x,y should be 8,16
    // We use +4 and +12 so the 8x8 sprite is centered at the x,y position
    move_metasprite(metasprites,baseTile,baseSprite,screenX+12,screenY+20);
}