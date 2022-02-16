#include <gb/gb.h>
#include <gb/metasprites.h>
#include "util.h"
#include "Gameplay/ghosts.h"
#include "Gameplay/Pacman.h"
#include "Gameplay/Character.h"
#include "graphics/Pacman.h"
#include "graphics/Ghosts.h"
#include "graphics/GhostsScared.h"
#include "graphics/GhostsEaten.h"
#include "graphics/Map.h"
#include "graphics/Dots.h"
#include "graphics/TargetTiles.h"
#include "cOMMON.h"

uint8_t ghostsReady=0;
uint8_t ghostsResetting=0;
uint16_t scatterChaseCounter,frightenedTimer;
uint8_t scatterOrChaseMode=CHASE;
uint8_t frigtenedTwoFrameAnimator,frigtenedOffset;


// Where each ghost starts, and where they scatter to
const Vector2D GhostsStartPositions[]={ {9,12}, {10,10},  {10,12}, {11,12}};
const Vector2D GhostsScatterTargets[]={ {20,27}, {18,-4},  {2,-4}, {0,27}};

// An eay way to get a directional priority using one of our direction values
// None, Up, Right, Down, Left respectively
const uint8_t DirectionalPriorities[]={ 3, 0, 1, 2 };


void GetGhostNextDirection(uint8_t ghostIndex){

    // If the ghosts aren't resetting we want specific logic to get out of the ghost pit
    // If they are resetting let them use the normal logic
    if(!ghostsResetting){

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
    }

    // Get which directions we can move in
    // NOTE: WE cannot reverse directions
    GetDirectionsCharacterCanMoveIn(&ghosts[ghostIndex],FALSE);

    if(numberOfDirectionsCharacterCanMoveIn==0)return;

    // Get the distance to our target tile FROM the first (possibly best) direction 
    uint16_t bestDistance=getSquareDistance(
        ghosts[ghostIndex].column+Directions[directionsCharacterCanMoveIn[0]].x,
        ghosts[ghostIndex].row+Directions[directionsCharacterCanMoveIn[0]].y,
        ghosts[ghostIndex].targetColumn,
        ghosts[ghostIndex].targetRow
    );

    uint16_t bestDirection=directionsCharacterCanMoveIn[0];

    // Check each other possibble side
    // NOTE: This loop starts at 1, because we default at 0
    for(uint8_t i=1;i<numberOfDirectionsCharacterCanMoveIn;i++){

        // Get the distance to our target tile FROM the tile in our next (possibly best) direction
        uint8_t nextPossibleNestDirection = directionsCharacterCanMoveIn[i];
        uint16_t nextPossibleBestDistance = getSquareDistance(
            ghosts[ghostIndex].column+Directions[nextPossibleNestDirection].x,
            ghosts[ghostIndex].row+Directions[nextPossibleNestDirection].y,
            ghosts[ghostIndex].targetColumn,
            ghosts[ghostIndex].targetRow
        );

        // If this distance is lesser OR equal w/ higher priority
        // Then we have a new best direction to move in
        if((nextPossibleBestDistance<bestDistance)||(nextPossibleBestDistance==bestDistance&&DirectionalPriorities[nextPossibleNestDirection]>DirectionalPriorities[bestDirection])){
            bestDistance=nextPossibleBestDistance;
            bestDirection=nextPossibleNestDirection;
        }
    }

    TryChangeDirection(&ghosts[ghostIndex],bestDirection);

}

void GetGhostTargetTile(uint8_t ghostIndex){
    if(pacman.state==0){
        ghosts[ghostIndex].targetColumn=GhostsStartPositions[ghostIndex].x;
        ghosts[ghostIndex].targetRow=GhostsStartPositions[ghostIndex].y;
        return;
    }

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

                    // if we are 8 tiles away or less
                    // 8*8 is 64
                    if(squareDistance<64){

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
            GetDirectionsCharacterCanMoveIn(&ghosts[ghostIndex],FALSE);

            random = RandomNumber(0,numberOfDirectionsCharacterCanMoveIn);

            ghosts[ghostIndex].targetColumn=ghosts[ghostIndex].column+Directions[directionsCharacterCanMoveIn[random]].x;
            ghosts[ghostIndex].targetRow=ghosts[ghostIndex].row+Directions[directionsCharacterCanMoveIn[random]].y;


        break;

        // Go to the middle tile
        case EATEN:
            ghosts[ghostIndex].targetColumn=10;
            ghosts[ghostIndex].targetRow=10;
        break;
    }

}

void SetupGhosts(){
    
    // For each of the 4 ghosts
    for(uint8_t i=0;i<4;i++){

        // Reset their positions and movement
        ghosts[i].state=SCATTERCHASE;
        ghosts[i].move=0;
        ghosts[i].direction=DOWN;
        ghosts[i].column=GhostsStartPositions[i].x;
        ghosts[i].row=GhostsStartPositions[i].y;

        // Decide our next target tile and direction
        GetGhostTargetTile(i);
        GetGhostNextDirection(i);

        // Draw the ghot initially
        DrawGhost(i);
    }
    frigtenedTwoFrameAnimator=0;
    frigtenedOffset=0;
    ghostsResetting=0;
    frightenedTimer=0;
}

void UpdateSingleGhost(uint8_t i){

    // If we are not at our start position
    // The ghosts aren't ready
    if(ghosts[i].column!=GhostsStartPositions[i].x||ghosts[i].row!=GhostsStartPositions[i].y){
        ghostsReady=0;
    }

    if(ghosts[i].state!=EATEN){

        int16_t pacmanX, pacmanY, ghostX, ghostY;

        pacmanX = pacman.column*8+Directions[pacman.direction].x*(pacman.move>>4);
        pacmanY = pacman.row*8+Directions[pacman.direction].y*(pacman.move>>4);
        ghostX = ghosts[i].column*8+Directions[ghosts[i].direction].x*(ghosts[i].move>>4);
        ghostY = ghosts[i].row*8+Directions[ghosts[i].direction].y*(ghosts[i].move>>4);

        int16_t xd = pacmanX-ghostX;
        int16_t yd = pacmanY-ghostY;

        // Get the absolute value
        if(xd<0)xd=-xd;
        if(yd<0)yd=-yd;

        if(xd<=8&&yd<=8){
            
            // If we are frightened
            if(ghosts[i].state==FRIGHTENED){

                // WE are eaten
                ghosts[i].state=EATEN;
                delay(400);
            }else{

                // Pacman is dead
                pacman.state=0;

            }
        }

    }else if(ghosts[i].column>=GHOST_PIT_LEFT&&ghosts[i].column<=GHOST_PIT_RIGHT&&ghosts[i].row>=GHOST_PIT_TOP&&ghosts[i].row<=GHOST_PIT_BOTTOM){

        ghosts[i].state=SCATTERCHASE;
        
    }

    uint8_t speed = 10;

    // Move faster when eaten
    if(ghosts[i].state==EATEN)speed=40;

    // If the ghosts are resetting
    if(ghostsResetting){

        // If we are at our target location
        if(ghosts[i].column==GhostsStartPositions[i].x&&ghosts[i].row==GhostsStartPositions[i].y){
            speed=0;
            ghosts[i].move=0;

        // if we are not there, move faster
        }else{
            speed=40;
        }
    }
    

    // Move forward
    // This function returns true when we reach new tile
    if(MoveForward(&ghosts[i],speed)){

        // Pick a new target tile
        // Pick a new direction
        GetGhostTargetTile(i);
        GetGhostNextDirection(i);
    }
}
void DrawGhost(uint8_t i){

    switch(ghosts[i].state){
        case FRIGHTENED: DrawCharacter(&ghosts[i],GhostsScared_metasprites[frigtenedOffset*2+twoFrameAnimator],2+i*2,GHOSTS_SCARED_SPRITES_START); break;
        case EATEN: DrawCharacter(&ghosts[i],GhostsScared_metasprites[ghosts[i].direction],2+i*2,GHOSTS_EATEN_SPRITES_START); break;
        default: DrawCharacter(&ghosts[i],Ghosts_metasprites[ghosts[i].direction*2+twoFrameAnimator+i*8],2+i*2,GHOSTS_SPRITES_START); break;
    }

    if(enableDebug){

        int16_t screenX = ghosts[i].targetColumn*8-SCX_REG;
        int16_t screenY = ghosts[i].targetRow*8-SCY_REG;

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

    // While our frighted timer is larger than 0
    // Are ghosts are still frightened
    if(frightenedTimer>0){
        frightenedTimer--;

        // When it reaches zero
        // We want to change all ghosts back to scatterchase mode
        if(frightenedTimer==0){
            for(uint8_t i=0;i<4;i++){

                // If this ghosts is frightened still (because they could have been eaten)
                // Change by to scatter-chase mode
                if(ghosts[i].state==FRIGHTENED)ghosts[i].state=SCATTERCHASE;
            }
        }
    }

    frigtenedTwoFrameAnimator++;
    if(frigtenedTwoFrameAnimator>=20){
        frigtenedTwoFrameAnimator=0;
        if(frigtenedOffset==0)frigtenedOffset=1;
        else frigtenedOffset=0;
    }

    UpdateScatterOrChaseMode();

    // By default, all ghosts are ready
    // Each ghost will unset this to 0 if they are not ready.
    ghostsReady=1;

    // For each of the 4 ghosts
    for(uint8_t i=0;i<4;i++){

        // Update and Draw
        UpdateSingleGhost(i);
        DrawGhost(i);
    }
}