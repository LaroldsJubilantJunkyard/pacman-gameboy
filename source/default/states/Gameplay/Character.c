#include <gb/gb.h>
#include <gb/metasprites.h>
#include "common.h"
#include "states/Gameplay/Character.h"
#include "util.h"
#include "graphics/Map.h"
#include "graphics/Pellets.h"



uint8_t sidesCanCheck[4] = {0,0,0,0},maxPossibleSides=0;
void GetSidesCanCheck(Character *character,uint8_t allowReverse){
    
    maxPossibleSides=0;

    for (uint8_t i = 0; i < 4; i++)
    {
        uint8_t notReversingOrIsAllowed=(character->direction!=reverseDirections[i]||allowReverse);
        if(IsTileWalkable(character,i)&&notReversingOrIsAllowed) sidesCanCheck[maxPossibleSides++]=i;
    }
}

uint8_t IsAligned(Character *character){
    
    // Give some extra space for user-friendliness
    return (character->move>>4)==0||(character->move>>4)>=8;
}

uint8_t IsTileWalkable(Character *character, uint8_t direction){
        
    int8_t nextColumn=character->column+Directions[direction].x;
    int8_t nextRow=character->row+Directions[direction].y;

    // To allow looping around, consider horizontally out of bound tiles walkable
    if(nextColumn>=Map_WIDTH/8||nextColumn<0){
        
        // Don't allow vertical movement
        if(direction==UP||direction==DOWN)return FALSE;
        return TRUE;
    }

    return get_bkg_tile_xy(nextColumn,nextRow)==blank ||
        get_bkg_tile_xy(nextColumn,nextRow)==PELLETS_TILES_START||
        get_bkg_tile_xy(nextColumn,nextRow)==PELLETS_TILES_START+1;
}

uint8_t IsNextTileWalkable(Character *character){

    return IsTileWalkable(character,character->direction);
}

void TryChangeDirection(Character *character, uint8_t nextDirection){

    // If this is the same direction, do nothing
    if(character->direction==nextDirection)return;

    uint8_t aligned = IsAligned(character);
    
    uint8_t changingAxis=
        (character->direction==LEFT&&(nextDirection==UP||nextDirection==DOWN))||
        (character->direction==RIGHT&&(nextDirection==UP||nextDirection==DOWN))||
        (character->direction==UP&&(nextDirection==LEFT||nextDirection==RIGHT))||
        (character->direction==DOWN&&(nextDirection==LEFT||nextDirection==RIGHT));

    uint8_t canMoveThisWay=IsTileWalkable(character,nextDirection);

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
            character->move=((8-(character->move>>4))<<4);

        }
    }
}

uint8_t MoveForward(Character *character, uint8_t speed){

    uint8_t canMoveForward=IsNextTileWalkable(character);

    if((Directions[character->direction].x!=0||Directions[character->direction].y!=0)&&canMoveForward){

        // Move forward some
        character->move+=speed;

        // If this character has moved one tile away
        if((character->move>>4)>=8){

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
    uint16_t screenX=(character->column*8+Directions[character->direction].x*(character->move>>4))-cameraX;
    uint16_t screenY=(character->row*8+Directions[character->direction].y*(character->move>>4))-cameraY;


    // Sprites are draw at the specified x,y position, MINUS 8 and 16
    // So a sprite draw at 0,0 will not be shown
    // To draw a sprite at 0,0, it's x,y should be 8,16
    // We use +4 and +12 so the 8x8 sprite is centered at the x,y position
   // move_sprite(0,screenX+4,screenY+12);
    //move_sprite(1,screenX+12,screenY+12);
    move_metasprite(metasprites,baseTile,baseSprite,screenX+12,screenY+20);
}