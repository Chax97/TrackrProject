#include "ADC.h"



void adc_set(void){
    
    // Setup Pin Connect for AD0[0].
    // Set bits [15:14] = 01.
	PINSEL1 |= 0x2000;
    PINSEL1 &= ~(1 << 15);
    
    /*on reset, the ADC is disabled. First set the PCADC bit then enable
      AD0CR    
    */
    //Set power control reg for adc
    PCONP |= (1 << 12);
    //Set PDN bit in ADOCR
    AD0CR |= (1 << 21);
    
    //Use Pin 0 to be sampled and converted
    AD0CR |= (1 << 0);
    //CLK should be 4.5Mhz or less, PCLK = 72 Mhz, CLKDIV = 15;
    AD0CR |= (0xF << 8);
    
    return;
}


unsigned long adc_result(void) {
    
    unsigned long dataReg;
    int result;
    //read AD0R0 to clear done bit
    dataReg = AD0DR0;
    if (dataReg & (1 << 31))
    {
       result |= (dataReg & 0x7FE0);
    }
    return result;
    
}

