#include "LPC17xx.h"
#include "joystick.h"
#include "../led/led.h"

/*----------------------------------------------------------------------------
  Function that turns on requested led
 *----------------------------------------------------------------------------*/
void joystick_On(unsigned int num) {
	LED_Out(num);
}