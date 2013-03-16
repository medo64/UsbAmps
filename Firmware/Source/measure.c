#include <pic.h>
#include <limits.h>
#include "config.h"


const unsigned long VREF = 2048;
const unsigned long ADC_MAX = 1023;
const unsigned long RATIO_VOLTAGE = 320; //2:1 ratio (1K:1K)
const unsigned long RATIO_CURRENT = 100; //1V is 1A (3V max)


void measure_init() {
    ADFVR1 = 1; //A/D Converter Fixed Voltage Reference Peripheral output is 2x (2.048V)
    ADFVR0 = 0;
    FVREN = 1; //Fixed Voltage Reference is enabled

    TRISB = TRISB | 0b00001110; //RB1, RB2, and RB3 are inputs
    ANSELB = ANSELB | 0b00001110; //RB1, RB2, and RB3 are analog
    ADFM = 1; //Right justified. Six Most Significant bits of ADRESH are set to '0' when the conversion result is loaded.
    ADCS2 = 1; //Frc (clock supplied from a dedicated RC oscillator)
    ADCS1 = 1;
    ADCS0 = 1;
    ADNREF = 0; //Vref- is connected to AVss
    ADPREF1 = 1; //Vref+ is connected to internal fixed voltage reference
    ADPREF0 = 1;
    CHS4 = 1; //Fixed Voltage Reference (FVR) Buffer 1 Output
    CHS3 = 1;
    CHS2 = 1;
    CHS1 = 1;
    CHS0 = 1;
    ADON = 1; //ADC is enabled
}

unsigned int getRawAdc(unsigned char channel) {
    ADCON0 = (channel << 2) | 0x01; //Analog Channel Select bits / leave ADON
    __delay_us(10); //to discharge holding cap if there was measurement just before (at least 10us)
    ADGO = 1; //Setting this bit starts an A/D conversion cycle.
    while(ADGO); //A/D conversion cycle in progress.
    ADON = 0; //ADC is disabled and consumes no operating current
    return ADRES;
}

unsigned long measure_getVoltageIn_10u() {
    unsigned int adc = getRawAdc(9);
    if (adc < ADC_MAX) {
        unsigned long value = adc * VREF * RATIO_VOLTAGE / ADC_MAX;
        if (value >= 100) { value -= 100; }
        return value;
    } else {
        return LONG_MAX;
    }
}

unsigned long measure_getVoltageOut_10u() {
    unsigned int adc = getRawAdc(8);
    if (adc < ADC_MAX) {
        unsigned long value = adc * VREF * RATIO_VOLTAGE / ADC_MAX;
        if (value >= 100) { value -= 100; }
        return value;
    } else {
        return LONG_MAX;
    }
}

unsigned long measure_getCurrent_10u() {
    unsigned int adc = getRawAdc(10);
    if (adc < ADC_MAX) {
        unsigned long value = adc * VREF * RATIO_CURRENT / ADC_MAX;
        if (value >= 100) { value -= 100; }
        return value;
    } else {
        return LONG_MAX;
    }
}
