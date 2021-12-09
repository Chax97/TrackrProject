#include "RFT.h"


void set_rf_pin(void) {
    FIO1DIR |= 1;
}

void set_rf_high(void) {
    FIO1PIN |= 1;
}


void set_rf_low(void) {
    FIO1PIN &= ~1;
}
