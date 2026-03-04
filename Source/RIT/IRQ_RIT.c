#include "LPC17xx.h"
#include "RIT.h"
#include "../game/tetris.h"
#include "adc/adc.h"

#define JOY_UP    ((LPC_GPIO1->FIOPIN & (1 << 29)) == 0)
#define JOY_DOWN  ((LPC_GPIO1->FIOPIN & (1 << 26)) == 0)
#define JOY_LEFT  ((LPC_GPIO1->FIOPIN & (1 << 27)) == 0)
#define JOY_RIGHT ((LPC_GPIO1->FIOPIN & (1 << 28)) == 0)

static int tick_count = 0;
// static int gravity_threshold = 20; // Rimosso perché calcolato dinamicamente

static int prev_left = 0;
static int prev_right = 0;
static int prev_up = 0;

static int normalTick = 20; // Normal speed
volatile float gameSpeedDivider = 1;

static int b_eint1_down=0;
static int b_eint2_down=0;

void RIT_IRQHandler (void)
{		
		if(game.flag_update_display) {
          Tetris_Draw_Board();
      }
		
			// KEY 1 Management
			// + Debouncing 
		if(((LPC_PINCON->PINSEL4 & (1 << 22)) == 0)){
			if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
				b_eint1_down = b_eint1_down + 1;
					switch(b_eint1_down){
						case 1:
							if(game.state == STATE_PLAYING) {
								game.state = STATE_PAUSED;
								Music_Stop();
								GUI_Text(80, 230, (uint8_t *)"PAUSED", Red, White);
							}else if(game.state == STATE_PAUSED) {
								game.state = STATE_PLAYING;
								Music_Start();
								GUI_Text(80, 230, (uint8_t *)"______", Black, Black);
							}
						break;
						default:
						break;
					}
			}else{
				b_eint1_down = 0;
				NVIC_ClearPendingIRQ(EINT1_IRQn);
				NVIC_EnableIRQ(EINT1_IRQn);							  /* enable Button interrupts			*/
				LPC_PINCON->PINSEL4    |= (1 << 22);      /* External interrupt 0 pin selection */
			}
			//NVIC_EnableIRQ(EINT1_IRQn);
		}
		// KEY 2 Management
		// debouncing
		if((LPC_PINCON->PINSEL4 & (1 << 24)) == 0){															// KEY2
			if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
				b_eint2_down = b_eint2_down + 1;
				switch(b_eint2_down){
					case 1:
						if(game.state == STATE_PLAYING) {
							Action_HardDrop();
						}
						break;
					default:
						break;
				}
			}else{
				b_eint2_down = 0;
				// Pulisci eventuali flag "fantasma" generati dal rimbalzo del rilascio
        LPC_SC->EXTINT |= (1 << 2);
				// 2. Pulisci la memoria del processore (il "fantasma" nel NVIC)
				NVIC_ClearPendingIRQ(EINT2_IRQn); //
				NVIC_EnableIRQ(EINT2_IRQn);							  /* enable Button interrupts			*/
				LPC_PINCON->PINSEL4    |= (1 << 24);      /* External interrupt 0 pin selection */
			}
		}
		
		
    if(game.flag_update_display != 0) {
        LPC_RIT->RICTRL |= 0x1;
        return;
    }

    if(game.state == STATE_PLAYING) {
        // Joystick movementi
        if(JOY_LEFT && !prev_left) Action_MoveLeft();
        prev_left = JOY_LEFT;

        if(JOY_RIGHT && !prev_right) Action_MoveRight();
        prev_right = JOY_RIGHT;

        if(JOY_UP && !prev_up) Action_Rotate();
        prev_up = JOY_UP;

        // --- GRAVITA' E POWERUP ---
        tick_count++;
        
        int current_threshold;

				// if slowdown active
        if (game.timer_powerup_slow > 0) {
            current_threshold = (JOY_DOWN) ? (normalTick/2) : normalTick; 
            
            game.timer_powerup_slow--; // Decremento il timer (15 sec = 300 ticks)
        } 
        else {
            // Comportamento Normale
            current_threshold = (JOY_DOWN) ? (normalTick/2) : normalTick;
            
            ADC_start_conversion(); // Leggo ADC per aggiornare gameSpeedDivider
            
            if ((int)gameSpeedDivider > 0) {
                current_threshold = current_threshold / (int)gameSpeedDivider;
            }
        }

        // Discesa tetromino
        if(tick_count >= current_threshold) {
            Action_GravityTick();
            tick_count = 0;
        }
    }

	LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
}