#include <string.h>
#include "LPC17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include <stdio.h>
#include "../music/music.h"

static int dac_toggle = 0;

extern void Music_PlayNext(void); 

uint16_t SinTable[45] = {
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER0_IRQHandler (void)
{
    static int sineticks = 0;
    static int currentValue; 

    currentValue = SinTable[sineticks];
    
    // Normalizzazione
    currentValue -= 410;
    currentValue /= 1;
    currentValue += 410;

    LPC_DAC->DACR = currentValue << 6;

    sineticks++;
    if(sineticks == 45) sineticks = 0;

    LPC_TIM0->IR = 1; // Clear interrupt flag
}

void TIMER1_IRQHandler (void)
{
    disable_timer(0);     // ferma suono
    LPC_TIM1->IR = 1;     // Clear interrupt
    
    Music_PlayNext();
}