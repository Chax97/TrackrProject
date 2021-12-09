#include "RTC.h"
#include "lpc24xx.h"



/*	Initialises the RTC before first use.
		
		Sets the peripheral clock and the RTC interrupts are enabled	
		return int - '1' if the RTC was succesfully intialised
*/
int RTC_Init(void)
{
    //Turn on RTC
    //Set CLK using PCLK_SEL0
    //Output interrupt pin using ILR, CIIR, alarm registers,alarm mask registers
    //If RTC is used, VBAT must be connected to either VDD or external battery.
    //Use alarm registers to specify a date and time for an interrupt
    //To clear interrupts, read ILR and write the value back to it.
    //To use RTC oscillator we must connect 2 cpacitors to the RTCX1 and RTCX2 I/O pins 
    //use 22pF external load cpacitors for both
    
    //Power RTC using PCONP register
    //RTC is on after reset but just incase    
    unsigned long clearRTC_Interrupt;
    PCONP |= 0x200;

    
    //Select CLK from 32kHz oscillator
    RTC_CCR |= (1<<4);
    
    

    //Enable interrupt every minute
    RTC_CIIR |=0x2;	
    
    //Clear all RTC interrupts   
    clearRTC_Interrupt = RTC_ILR;   
    RTC_ILR = clearRTC_Interrupt;
    
    
    //Set Interrupt Vector
    VICVectAddr13 = (unsigned long)RTC_IRQHandler;
    //Set Interrupt Priority to 1
    VICVectPriority13 = 1;
    //Enable VIC interrupt 13 for RTC  .p113 of LCP24xx.pdf
    VICIntEnable |= (1<<13);
   
    
    //RTC interrupt wake up in power down mode .p64 of LCP24xx.pdf
    INTWAKE |= (1<<15);
    
    
    //Set RTC interrupt as IRQ
    VICIntSelect &= ~(1<<13);
    
    return 0;
}

/*	Sets the value of the real time clock
    int year: Year value in the range of 0 to 4095
    int month: Month value in the range of 1 to 12		
    int dom: Day of month value in the range of 1 to 28, 29, 30, or 31 (depending on the month and whether it is a leap year
    int hours: The desired value of the RTC hours (0-23)
    int minutes: The desired value of the RTC minutes (0-59)
    int seconds: The desired value of the RTC seconds (0-59)
    @note assumes the RTC has been initalised and all input parameters are in range
*/
void RTC_Set(const int year, const int month, const int dom, const int hour, const int minutes, const int seconds)
{	
    
    //Disable clock to intialise time counters
    RTC_CCR &= ~0x01;
    
    RTC_YEAR = year;
    RTC_MONTH = month;
    RTC_DOM = dom;
    RTC_HOUR = hour;
    RTC_MIN = minutes;
    RTC_SEC = seconds;
        
    //Enable time counters after initalise
    RTC_CCR |= 0x01;
    
    //Reset Clock tick counter
    RTC_CCR |= 0x2;

    //start clock by clearing reset CCR_CTCRST
    RTC_CCR &= ~0x2;


}
/*  RTC Interrupt Service Routine

*/
	
	
void RTC_IRQHandler (void) __irq
{
    
    unsigned long clearRTC_Interrupt;   
    
    clearRTC_Interrupt = RTC_ILR;   
    RTC_ILR = clearRTC_Interrupt;
    VICVectAddr = 0;
}


