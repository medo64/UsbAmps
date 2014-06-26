#include <pic.h>
#include <stdbool.h>
#include <stdint.h>


void timer1_init() {
    TMR1CS1 = 0; //Timer1 clock source is instruction clock (FOSC/4)
    TMR1CS0 = 0;
    T1CKPS1 = 1; //1:8 Prescale value
    T1CKPS0 = 1;
    TMR1IF = 0; //turn off interrupt
    TMR1ON = 1;
}


uint8_t timer1_passes = 0;

bool timer1_hasSecondPassed() { //needs to be called at least 5 times per second; adjusted for 16 MHz clock
    if (TMR1IF) {
        TMR1IF = 0;
        timer1_passes++;
        if (timer1_passes > 4) {
            timer1_passes = 0;
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}
