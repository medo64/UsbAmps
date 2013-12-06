#include <pic.h>
#include "config.h"


#pragma config FOSC     = INTOSC
#pragma config WDTE     = ON
#pragma config PWRTE    = ON
#pragma config MCLRE    = OFF
#pragma config CP       = OFF
#pragma config CPD      = OFF
#pragma config BOREN    = ON
#pragma config CLKOUTEN = OFF
#pragma config IESO     = OFF
#pragma config FCMEN    = ON

#pragma config WRT      = OFF
#pragma config VCAPEN   = RA6
#pragma config PLLEN    = OFF
#pragma config STVREN   = ON
#pragma config BORV     = HI
#pragma config LVP      = OFF


void init() {
    GIE = 0; //disable interrupts

    IRCF3 = 1; //16 MHz oscillator
    IRCF2 = 1;
    IRCF1 = 1;
    IRCF0 = 1;

    WDTPS4 = 0; //1s watchdog (1:32768)
    WDTPS3 = 1;
    WDTPS2 = 0;
    WDTPS1 = 1;
    WDTPS0 = 0;

    ANSELA = 0;
    ANSELB = 0;
    ANSELD = 0;

    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;

    LATA = 0;
    LATB = 0;
    LATC = 0;
    LATD = 0;
}

void clrwdt() {
    asm("CLRWDT");
}

void wait_250ms() {
    __delay_ms(250);
}
