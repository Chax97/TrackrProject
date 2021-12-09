#ifndef RTC_h
#define RTC_h
#include "lpc24xx.h"



/*	Initialises the RTC before first use.
		
		Sets the peripheral clock and the RTC interrupts are enabled	
		return int - '1' if the RTC was succesfully intialised
*/
int RTC_Init(void);

/*	Sets the value of the real time clock
		int year: Year value in the range of 0 to 4095
		int month: Month value in the range of 1 to 12		
		int dom: Day of month value in the range of 1 to 28, 29, 30, or 31 (depending on the month and whether it is a leap year
		int hours: The desired value of the RTC hours (0-23)
		int minutes: The desired value of the RTC minutes (0-59)
		int seconds: The desired value of the RTC seconds (0-59)
		@note assumes the RTC has been initalised and all input parameters are in range
*/
void RTC_Set(const int year, const int month, const int dom, const int hour, const int minutes, const int seconds);


void RTC_IRQHandler (void) __irq;


#endif
