#ifndef RFT_h
#define RFT_h

#include "lpc24xx.h"
//#inlude "timer" !!!(TO BE IMPLEMENTED)!!!

// selects GPIO1[0] as an output.
void set_rf_pin(void);

// sets RF signal HIGH.
void set_rf_high(void);

// sets RF signal LOW.
void set_rf_low(void);

#endif
