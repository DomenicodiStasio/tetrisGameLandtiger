#include "LPC17xx.h"
#include "joystick.h"

void joystick_init(void) {
    // --- 1. CONFIGURAZIONE PIN FUNCTION (PINSEL) ---
    // Impostiamo i pin da P1.25 a P1.29 come GPIO (valore 00)
    // Ogni pin occupa 2 bit nel registro PINSEL3.
    // Calcolo: (Pin - 16) * 2 -> Es. Pin 25: (25-16)*2 = bit 18
    
    LPC_PINCON->PINSEL3 &= ~(
        (3<<18) |  // P1.25 - Joystick Select (Click centrale)
        (3<<20) |  // P1.26 - Joystick Down
        (3<<22) |  // P1.27 - Joystick Left
        (3<<24) |  // P1.28 - Joystick Right
        (3<<26)    // P1.29 - Joystick Up
    );

    // --- 2. CONFIGURAZIONE DIREZIONE (FIODIR) ---
    // Impostiamo i pin a 0 per renderli INPUT
    
    LPC_GPIO1->FIODIR &= ~(
        (1<<25) |  // P1.25 Input
        (1<<26) |  // P1.26 Input
        (1<<27) |  // P1.27 Input
        (1<<28) |  // P1.28 Input
        (1<<29)    // P1.29 Input
    );
}