#include "lpc24xx.h"


// sets PINSEL of P0[23] to 01 for use of ADC[0].
void adc_set(void);

// returns the most recent value of ADC[0]
unsigned long adc_result(void);
