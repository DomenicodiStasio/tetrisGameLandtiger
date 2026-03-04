#include "LPC17xx.h"
#include "adc.h"
#include "../led/led.h"

unsigned short AD_current;   
unsigned short AD_last = 0xFFFF;     /* Last converted value               */

extern volatile float gameSpeedDivider;


/* k=1/f'*f/n  k=f/(f'*n) k=25MHz/(f'*45) */

/* NOTE MUSICALI */
const int freqs[8]={4240,3779,3367,3175,2834,2525,2249,2120};
/* 
131Hz		k=4240 C3
147Hz		k=3779
165Hz		k=3367
175Hz		k=3175
196Hz		k=2834		
220Hz		k=2525
247Hz		k=2249
262Hz		k=2120 C4
*/

//const int freqs[8]={2120,1890,1684,1592,1417,1263,1125,1062};
/*
262Hz	k=2120		c4
294Hz	k=1890		
330Hz	k=1684		
349Hz	k=1592		
392Hz	k=1417		
440Hz	k=1263		
494Hz	k=1125		
523Hz	k=1062		c5

*/


/*
	Description: ADC ha rumore "LFSB noise"
	con questa funzione "pulisci il rumore rimuovendo gli ultimi bit

	Parameters: nBitDesired è la nuova lunghezza in bit che si vuole
							N.B. Non è il numero di bit che si vuole pulire
*/
unsigned short ADC_CleanNoise(unsigned short rawValue, unsigned short nBitDesired){
	unsigned short nShift = 12-nBitDesired;
	if(nBitDesired<12 && nBitDesired>0){
		return rawValue >> nShift;
	}else{
		return rawValue;
	}
}

/*----------------------------------------------------------------------------

  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)

 *----------------------------------------------------------------------------*/
void ADC_IRQHandler(void) {
  	
  //AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);		// Read Conversion Result
	AD_current = ADC_CleanNoise(((LPC_ADC->ADGDR>>4) & 0xFFF), 4);		// FILTER: Read Conversion Result and consider only first n bit
																																		// N.B.: 3 bit per 8 livelli di potenziometro, 4 bit per 16, 5 per 32 ecc.
  if(AD_current != AD_last){
		/*YOUR CODE HERE*/
		gameSpeedDivider = ((AD_current*4)/15.0) + 1;
		LED_Out(gameSpeedDivider);
		AD_last = AD_current;
  }	
}