#include <gb/gb.h>
#include <gb/metasprites.h>
#include "util.h"
#include "states/Gameplay/ghosts.h"
#include "states/Gameplay/Pacman.h"
#include "states/Gameplay/Character.h"
#include "graphics/Pacman.h"
#include "graphics/Ghosts.h"
#include "graphics/GhostsScared.h"
#include "graphics/GhostsEaten.h"
#include "graphics/Map.h"
#include "graphics/Pellets.h"
#include "graphics/TargetTiles.h"
#include "cOMMON.h"

#define INKY 0 // blue
#define BLINKY 1 // red
#define PINKY 2
#define CLYDE 3 // orange

uint16_t scatterChaseCounter;
uint8_t scatterOrChaseMode=CHASE;
uint8_t frigtenedCounter,frigtenedOffset;

// Where each ghost starts, and where they scatter to
const Vector2D GhostsStartPositions[]={ {9,12}, {10,10},  {10,12}, {11,12}};
const Vector2D GhostsScatterTargets[]={ {20,27}, {18,-4},  {2,-4}, {0,27}};

// An eay way to get a directional priority using one of our direction values
// None, Up, Right, Down, Left respectively
const uint8_t DirectionalPriorities[]={ 3, 0, 1, 2 };


void GetGhostNextDirection(uint8_t ghostIndex){

    // Override directional logic if ghosts are in the ghost pit
    // To Make sure they can get out
    if(ghosts[ghostIndex].column>=GHOST_PIT_LEFT&&ghosts[ghostIndex].column<=GHOST_PIT_RIGHT&&ghosts[ghostIndex].row>=GHOST_PIT_TOP&&ghosts[ghostIndex].row<=GHOST_PIT_BOTTOM){

        // If we are on the left side
        if(ghosts[ghostIndex].column==GHOST_PIT_LEFT){

            // Move down and then to the right
            if(ghosts[ghostIndex].row==GHOST_PIT_TOP)ghosts[ghostIndex].direction=DOWN;
            else ghosts[ghostIndex].direction=RIGHT;

        // IF we are in the middle
        }else if(ghosts[ghostIndex].column>GHOST_PIT_LEFT&&ghosts[ghostIndex].column<GHOST_PIT_RIGHT){

            // Move up or right
            if(ghosts[ghostIndex].row==GHOST_PIT_TOP)ghosts[ghostIndex].direction=UP;
            else ghosts[ghostIndex].direction=RIGHT;

        // If weare on the right
        }else if(ghosts[ghostIndex].column==GHOST_PIT_RIGHT){

            // Move left or up
            if(ghosts[ghostIndex].row==GHOST_PIT_TOP)ghosts[ghostIndex].direction=LEFT;
            else ghosts[ghostIndex].direction=UP;
        }
    
        return;
    }

    // Get which directions we can move in
    // NOTE: WE cannot reverse directions
    GetSidesCanCheck(&ghosts[ghostIndex],FALSE);

    if(maxPossibleSides==0)return;

    uint16_t bestDistance=getSquareDistance(
        ghosts[ghostIndex].column+Directions[sidesCanCheck[0]].x,
        ghosts[ghostIndex].row+Directions[sidesCanCheck[0]].y,
        ghosts[ghostIndex].targetColumn,
        ghosts[ghostIndex].targetRow
    );

    uint16_t bestDirection=sidesCanCheck[0];

    for(uint8_t i=1;i<maxPossibleSides;i++){

        uint8_t nextPossibleNestDirection = sidesCanCheck[i];
        uint16_t nextPossibleBestDistance = getSquareDistance(
            ghosts[ghostIndex].column+Directions[nextPossibleNestDirection].x,
            ghosts[ghostIndex].row+Directions[nextPossibleNestDirection].y,
            ghosts[ghostIndex].targetColumn,
            ghosts[ghostIndex].targetRow
        );

        if((nextPossibleBestDistance<bestDistance)||(nextPossibleBestDistance==bestDistance&&DirectionalPriorities[nextPossibleNestDirection]>DirectionalPriorities[bestDirection])){
            bestDistance=nextPossibleBestDistance;
            bestDirection=nextPossibleNestDirection;
        }
    }

    ghosts[ghostIndex].direction=bestDirection;


}

void GetGhostTargetTile(uint8_t ghostIndex){

    int16_t intermediateColumn, intermediateRow;
    int16_t blinkyXDiff, blinkyYDiff;
    uint16_t squareDistance;
    uint8_t random;

    uint8_t state = ghosts[ghostIndex].state;

    // The ghost's 'state' variable will never have a value of SCATTER or CHASE
    // They are combined so we can use our 'scatterOrChase' variabble which alternates
    if(state==SCATTERCHASE)state=scatterOrChaseMode;

    switch(state){
        case CHASE:

            switch(ghostIndex){

                // From: https://www.youtube.com/watch?v=ataGotQ7ir8
                // Inky works with blinky to flank pacman

                case INKY:
                    intermediateColumn = pacman.column+Directions[pacman.direction].x*2;
                    intermediateRow = pacman.row+Directions[pacman.direction].y*2;

                    // When facing up, some targets offset horizontally too
                    if(pacman.direction==UP)intermediateColumn-=2;

                    blinkyXDiff = ghosts[BLINKY].column-intermediateColumn;
                    blinkyYDiff = ghosts[BLINKY].row-intermediateRow;
                    
                    ghosts[ghostIndex].targetColumn=intermediateColumn-blinkyXDiff;
                    ghosts[ghostIndex].targetRow=intermediateRow-blinkyYDiff;

                    break;

                // From: https://www.youtube.com/watch?v=ataGotQ7ir8
                // Pinky tries targets the tile 4 ahead of pacman

                case PINKY:
                    ghosts[ghostIndex].targetColumn=pacman.column+Directions[pacman.direction].x*4;
                    ghosts[ghostIndex].targetRow=pacman.row+Directions[pacman.direction].y*4;

                    // When facing up, some targets offset horizontally too
                    if(pacman.direction==UP)ghosts[ghostIndex].targetColumn-=4;
                    break;

                // From: https://www.youtube.com/watch?v=ataGotQ7ir8
                // Pinky tries targets pacman directly

                case BLINKY:                
                    ghosts[ghostIndex].targetColumn=pacman.column;
                    ghosts[ghostIndex].targetRow=pacman.row;
                    break;

                // From: https://www.youtube.com/watch?v=ataGotQ7ir8
                // Clyde targets pacman if far, and his scatter point if far
                case CLYDE:


                    squareDistance = getSquareDistance(pacman.column,pacman.row,ghosts[ghostIndex].column,ghosts[ghostIndex].row);

                    // if we are 8 tiles away or more
                    // 8*8 is 64
                    if(squareDistance>64){

                        // go toward the scatter target
                        ghosts[ghostIndex].targetColumn=GhostsScatterTargets[ghostIndex].x;
                        ghosts[ghostIndex].targetRow=GhostsScatterTargets[ghostIndex].y;

                    }else{

                        // Go toward pacman
                        ghosts[ghostIndex].targetColumn=pacman.column;
                        ghosts[ghostIndex].targetRow=pacman.row;

                    }

                    break;
            }

        break;
        
        // Go to the scatter targets
        case SCATTER:

            ghosts[ghostIndex].targetColumn=GhostsScatterTargets[ghostIndex].x;
            ghosts[ghostIndex].targetRow=GhostsScatterTargets[ghostIndex].y;

        break;
        case FRIGHTENED:

            // Get which directions we can move in
            // Do not pick opposite directions
            GetSidesCanCheck(&ghosts[ghostIndex],FALSE);

            random = RandomNumber(0,maxPossibleSides);

            ghosts[ghostIndex].targetColumn=ghosts[ghostIndex].column+Directions[sidesCanCheck[random]].x;
            ghosts[ghostIndex].targetRow=ghosts[ghostIndex].row+Directions[sidesCanCheck[random]].y;


        break;

        // Go to the middle tile
        case EATEN:
            ghosts[ghostIndex].targetColumn=10;
            ghosts[ghostIndex].targetRow=10;
        break;
    }

}

void SetupGhosts(){
    for(uint8_t i=0;i<4;i++){
        ghosts[i].state=SCATTERCHASE;
        ghosts[i].move=0;
        ghosts[i].direction=DOWN;
        ghosts[i].column=GhostsStartPositions[i].x;
        ghosts[i].row=GhostsStartPositions[i].y;
        GetGhostTargetTile(i);
        GetGhostNextDirection(i);
        DrawGhost(i);
    }
    frigtenedCounter=0;
    frigtenedOffset=0;
}

void UpdateSingleGhost(uint8_t i){


    if(ghosts[i].state!=EATEN){

        int16_t pacmanX, pacmanY, ghostX, ghostY;

        pacmanX = pacman.column*8+Directions[pacman.direction].x*(pacman.move>>4);
        pacmanY = pacman.row*8+Directions[pacman.direction].y*(pacman.move>>4);
        ghostX = ghosts[i].column*8+Directions[ghosts[i].direction].x*(ghosts[i].move>>4);
        ghostY = ghosts[i].row*8+Directions[ghosts[i].direction].y*(ghosts[i].move>>4);

        int16_t xd = pacmanX-ghostX;
        int16_t yd = pacmanY-ghostY;

        if(xd<0)xd=-xd;
        if(yd<0)yd=-yd;

        if(xd<=8&&yd<=8){
            
            if(ghosts[i].state==FRIGHTENED){

                ghosts[i].state=EATEN;
                delay(400);
            }else{

                pacman.state=0;

            }
        }

    }else if(ghosts[i].column>=GHOST_PIT_LEFT&&ghosts[i].column<=GHOST_PIT_RIGHT&&ghosts[i].row>=GHOST_PIT_TOP&&ghosts[i].row<=GHOST_PIT_BOTTOM){

        ghosts[i].state=SCATTERCHASE;
        
    }

    uint8_t speed = 10;

    // Move faster when eaten
    if(ghosts[i].state==EATEN)speed=40;

    if(MoveForward(&ghosts[i],speed)){

        GetGhostTargetTile(i);
        GetGhostNextDirection(i);
    }
}
void DrawGhost(uint8_t i){

    uint8_t baseTile=GHOSTS_SPRITES_START;
    metasprite_t const *ghostCurrentMetasprite = Ghosts_metasprites[ghosts[i].direction*2+twoFrameAnimator+i*8];
    
    if(ghosts[i].state==FRIGHTENED){

        baseTile=GHOSTS_SCARED_SPRITES_START;
        ghostCurrentMetasprite=GhostsScared_metasprites[frigtenedOffset*2+twoFrameAnimator];
        
    }else if(ghosts[i].state==EATEN){

        baseTile=GHOSTS_EATEN_SPRITES_START;
        ghostCurrentMetasprite=GhostsScared_metasprites[ghosts[i].direction];
    }

    
    DrawCharacter(&ghosts[i],ghostCurrentMetasprite,2+i*2,baseTile);

    if(enableDebug){

        uint8_t screenX = ghosts[i].targetColumn*8-cameraX;
        uint8_t screenY = ghosts[i].targetRow*8-cameraY;

        move_metasprite(TargetTiles_metasprites[i],TARGET_TILES_SPRITES_START,36+i,12+screenX,24+screenY);
        
    }
}


void UpdateScatterOrChaseMode(){

    // Per: youtube.com/watch?v=ataGotQ7ir8
    // The ghosts alternate between scatter and chase mode

    scatterChaseCounter++;
    uint16_t scatterChaseTime;

    if(scatterOrChaseMode==SCATTER)scatterChaseTime=7;
    else scatterChaseTime=20;

    // Wait until enough time has passed
    // 60 frames (roughly with wait_vbl_done), multiply the time by 60 to the amount in seconds
    // TODO Add the  5 chase/scatter cycles
    if(scatterChaseCounter>scatterChaseTime*60){

        // Reset our counter
        scatterChaseCounter=0;

        // Swap between the two
        scatterOrChaseMode=!CHASE;

        // For each of the 4 ghosts
        for(uint8_t i=0;i<4;i++){

            // if this ghost is in scatter or chase mode
            if(ghosts[i].state==SCATTERCHASE){

                // Per: youtube.com/watch?v=ataGotQ7ir8
                // The ghosts swap directions when changing between chase/scatter mode
                switch(ghosts[i].direction){
                    case LEFT:TryChangeDirection(&ghosts[i],RIGHT);break;
                    case RIGHT:TryChangeDirection(&ghosts[i],LEFT);break;
                    case UP:TryChangeDirection(&ghosts[i],DOWN);break;
                    case DOWN:TryChangeDirection(&ghosts[i],UP);break;
                }
            }
        }

    }
}

void UpdateAllGhosts(){

    frigtenedCounter++;
    if(frigtenedCounter>=20){
        frigtenedCounter=0;
        if(frigtenedOffset==0)frigtenedOffset=1;
        else frigtenedOffset=0;
    }

    UpdateScatterOrChaseMode();

    // For each of the 4 ghosts
    for(uint8_t i=0;i<4;i++){


        UpdateSingleGhost(i);
        DrawGhost(i);

    }
}