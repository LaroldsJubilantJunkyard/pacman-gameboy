#ifndef CHARACTER_HEADER
#define CHARACTER_HEADER set
#include <gb/metasprites.h>

typedef struct Character{

    uint16_t move;
    uint8_t direction;
    int16_t column,row;
    
    // Pacman: Normal vs dead
    // Ghosts: Normal vs Eaten vs Scared
    uint8_t state;

    // Ghosts Only
    int16_t targetColumn,targetRow;
} Character;

extern uint8_t directionsCharacterCanMoveIn[4], numberOfDirectionsCharacterCanMoveIn;


uint8_t CanCharacterMoveInThisDirection(Character *character, uint8_t direction);
uint8_t MoveForward(Character *character, uint8_t speed);
void TryChangeDirection(Character *character, uint8_t nextDirection);
void GetDirectionsCharacterCanMoveIn(Character *character,uint8_t allowReverse);
void DrawCharacter(Character *character, metasprite_t *metasprites, uint8_t baseSprite, uint8_t baseTile);
#endif