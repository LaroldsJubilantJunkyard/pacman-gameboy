#include <gb/gb.h>
#include "util.h"
#include "common.h"
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