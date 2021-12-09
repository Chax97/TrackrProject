#include "lpc24xx.h"
#include "DelayTimer.h"




// udelay should delay the processor for 'delay_in_us' number of microseconds.
// * LPC24XX.h contains a definition for the PCLK, "Fpclk"
//   use this definition for PCLK as defined in the manual.
// * Use Timer 0. This means you MUST use the prefix T0 for every control
//   register, i.e. T0TCR.
// * Make sure you reset the timer, and when you start the timer ENSURE you
//   set the reset bit back to zero!
void udelay(unsigned int delay_in_us) {
		
		//FPCLK = 72000000 cycles/second 
		//Set	prescale counter to 72 so 1 TC count = 1 microsecond
		
		//Enable Timer Counter and Prescale Counter
		T0TCR |= 0x01;
	
		//Clear Reset bit
		T0TCR &= ~0x02;
		
		//Enable Timer Counter and Prescale Counter
		//Reset Counter
		T0TCR |= 0x02;
	
		
	
		//Stop when TC = MR0
		T0MCR = 0x4;
	
		//Setr Match Register
		T0MR0 = delay_in_us;
	
	
		//wait until TC = delay
		while (T0TC != delay_in_us)
		{
			
		}
		
		//Reset Counter again
		T0TCR = 0x02;
		
	
}
