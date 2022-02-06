#include <gb/gb.h>
#include <gb/metasprites.h>
#include <gb/cgb.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "ghosts.h"
#include "dots.h"
#include "common.h"
#include "pacman.h"
#include "Character.h"
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
#include "states/Gameplay.h"
#include "states/GameFirstLoad.h"

void main(void){

     GameFirstLoad();

    uint8_t currentState = GAMEFIRSTLOAD;
    uint8_t nextState=GAMEPLAY;


    while(TRUE){

        // If we are changing game states
        if(currentState!=nextState){

            // Change our current state
            if(nextState!=RELOAD_GAMESTATE) currentState=nextState;

            // What is our new state
            switch (currentState)
            {
                // Setup accordingly
                case GAMEPLAY: SetupGameplay();break;
            }

        }

        joypadPrevious=joypadCurrent;
        joypadCurrent=joypad();

        // What is our current state
        switch (currentState)
        {
            // Update accordingly
            case GAMEPLAY: nextState=UpdateGameplay();break;
        }

        wait_vbl_done();

    }

    
}