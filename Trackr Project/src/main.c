#include "lpc24xx.h"
#include "RTC.h"
#include "MCI.h"
#include "ADC.h"
#include "RFT.h"
#include "DelayTimer.h"

#define YEAR 2021
#define MONTH 7
#define DOM 29
#define HOUR 0
#define MIN 0
#define SEC 0


int main(void) {

		int i;
        unsigned long ADC_Sample; 
    
		RTC_Init();    
		RTC_Set(YEAR,MONTH,DOM,HOUR,MIN,SEC);
        
        MCI_Init();
        MCI_SET_ADD();
        MCI_Send_ACMD();
        //MCI_Send_Status();
        //MCLCLK set to normal from slow
        //MCLCLK frequency = MCLK / [2�(ClkDiv+1)].
        MCI_CLOCK &= 0x1;
        for (i = 0; i< 0x100; i++);
        //4_bit bus
        MCI_CLOCK |= (1 << 11);
        MCI_Set_BlockLen(BLOCK_LENGTH);
    
        adc_set();

        set_rf_pin();
    
        while (1)
        {
                        
            //read data from ADC
            ADC_Sample = adc_result();
            
            //store in MCI FIFO which triggers interrupt to save data
            MCI_FIFO = ADC_Sample;
            if (MCI_Write_Block(1) == 0)
            {
                //error
            }

            //turnon RF transmitter for 3 seconds
            set_rf_high();
            udelay(3000);
            set_rf_low();
            
            //turn on power down mode after storing data         
            PCON |= (1 << 1);
            PCON &= ~((1 << 0) | (1 << 7)); 
        }
        		
		return 0;

}
