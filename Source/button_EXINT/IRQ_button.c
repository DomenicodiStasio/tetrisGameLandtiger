#include "button.h"
#include "LPC17xx.h"
#include "../game/tetris.h"
#include <stdlib.h>

extern TetrisGame_t game;

void EINT0_IRQHandler (void)	  
{
    LPC_SC->EXTINT |= (1 << 0);
}

// KEY1 - Start/ Pause / restart
void EINT1_IRQHandler (void)	  
{
	if(game.state == STATE_INTRO || game.state == STATE_GAMEOVER) {
			  // valore attuale del contatore RIT
        // usato come "seme" per il generatore casuale
        srand(LPC_RIT->RICOUNTER);
			
        Tetris_Start();
    }
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	LPC_SC->EXTINT |= (1 << 1);
	//LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

// KEY2 - Hard Drop
void EINT2_IRQHandler (void)	  
{
  NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */
	LPC_SC->EXTINT |= (1 << 2);
	//LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */ 
}