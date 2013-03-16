#include <pic.h>
#include "config.h"


void touch_init() {
    TRISD = TRISD | 0b00000110; //RD2, RD1 are inputs
    ANSELD = ANSELD | 0b00000110; //RD2, RD1 are analog
    CPSRNG1 = 1; //Oscillator is in high range. Charge/discharge current is nominally 18 µA.
    CPSRNG0 = 1;
    CPSOUT = 1; //Oscillator is sourcing current (Current flowing out the pin)
    T0XCS = 1; //Timer0 clock source is the capacitive sensing oscillator
    CPSON = 0; //Capacitive Sensing Module is disabled
}

void touch_bx_reset() {
    CPSON = 0; //Capacitive Sensing Module is disabled
    TMR0 = 0;
    TMR0IF = 0; //TMR0 register did not overflow
}

void touch_b1_start() {
    touch_bx_reset();
    CPSCH3 = 1; //CPS10
    CPSCH2 = 0;
    CPSCH1 = 1;
    CPSCH0 = 0;
    CPSON = 1; //Capacitive Sensing Module is enabled
}

void touch_b2_start() {
    touch_bx_reset();
    CPSCH3 = 1; //CPS9
    CPSCH2 = 0;
    CPSCH1 = 0;
    CPSCH0 = 1;
    CPSON = 1; //Capacitive Sensing Module is enabled
}

unsigned char touch_bx_stop() {
    CPSON = 0; //Capacitive Sensing Module is disabled
    if (TMR0IF) { //overflow happened, just return max value
        return 0xFF;
    } else {
        return TMR0;
    }
}

bit touch_b1_pressed() {
    touch_b1_start();
    __delay_ms(1);
    return (touch_bx_stop() < 255) ? 1 : 0;
}

bit touch_b2_pressed() {
    touch_b2_start();
    __delay_ms(1);
    return (touch_bx_stop() < 255) ? 1 : 0;
}
