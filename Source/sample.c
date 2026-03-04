#include "LPC17xx.h"
#include "GLCD.h"
#include "button.h"
#include "timer.h"
#include "RIT/RIT.h"
#include "game/tetris.h"


void joystick_init(void) {
    LPC_PINCON->PINSEL3 &= ~(
        (3<<18) |  // P1.25 - Joystick Select (Click centrale)
        (3<<20) |  // P1.26 - Joystick Down
        (3<<22) |  // P1.27 - Joystick Left
        (3<<24) |  // P1.28 - Joystick Right
        (3<<26)    // P1.29 - Joystick Up
    );
    LPC_GPIO1->FIODIR &= ~(
        (1<<25) |  // P1.25 Input
        (1<<26) |  // P1.26 Input
        (1<<27) |  // P1.27 Input
        (1<<28) |  // P1.28 Input
        (1<<29)    // P1.29 Input
    );
}

int main(void) {
    
  SystemInit();  
	
	BUTTON_init();
	joystick_init();
	ADC_init();
	
	LPC_SC->PCONP |= (1 << 15); // GPIO
	
  LCD_Initialization();

  Tetris_Init();
  
  init_RIT(5000000);
  enable_RIT();

	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);

	LPC_GPIO0->FIODIR |= (1<<26);
	
	// 1. AUDIO (Timer 0): PRIORITÀ MASSIMA (0)
	// per avere suono pulito
	NVIC_SetPriority(TIMER0_IRQn, 0);

	// 2. METRONOMO (Timer 1): PRIORITÀ ALTA (1)
	NVIC_SetPriority(TIMER1_IRQn, 1);

	// 3. GIOCO (RIT): PRIORITÀ BASSA (4 o più)
	// può tardare perchè è impercettibile
	NVIC_SetPriority(RIT_IRQn, 4); 

	// 4. BUTTON INTERRUPT
	NVIC_SetPriority(EINT1_IRQn, 2);

	while(1) {
		__ASM("wfi");   
  }
}







