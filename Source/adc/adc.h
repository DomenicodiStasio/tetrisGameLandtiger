#include <string.h>

/* lib_adc.c */
void ADC_init (void);
void ADC_start_conversion (void);

/* IRQ_adc.c */
void ADC_IRQHandler(void);

unsigned short ADC_CleanNoise(unsigned short rawValue, unsigned short nBitDesired);
