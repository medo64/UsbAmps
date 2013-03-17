#include <pic.h>
#include <limits.h>
#include "config.h"


#define VREF       4096L
#define ADC_COUNT  1024L
#define ADC_MAX    1023L

#define VOLTAGE_RATIO  200L //2:1 ratio (1K:1K)
#define CURRENT_RATIO  100L //1V is 1A (3V max)

const unsigned long VOLTAGE_ERROR_OFFSET = VREF * VOLTAGE_RATIO / ADC_COUNT / 2 + 1; //just a guess
const unsigned long VOLTAGE_ERROR_FACTOR = 100000L / 200; //0.2%

const unsigned long CURRENT_ERROR_OFFSET = VREF * CURRENT_RATIO / ADC_COUNT / 2 + 1; //just a guess
const unsigned long CURRENT_ERROR_FACTOR = 100000L / 200; //0.2%


void measure_init() {
    ADFVR1 = 1; //A/D Converter Fixed Voltage Reference Peripheral output is 4x (4.096V)
    ADFVR0 = 1;
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
    __delay_us(13); //to discharge holding cap if there was measurement just before (at least 10us)
    ADGO = 1; //Setting this bit starts an A/D conversion cycle.
    while(ADGO); //A/D conversion cycle in progress.
    ADON = 0; //ADC is disabled and consumes no operating current
    return ADRES;
}

unsigned long getVoltage_10u(unsigned char channel) {
    unsigned int adc = getRawAdc(channel);
    if (adc < ADC_MAX) {
        unsigned long value = adc * VREF * VOLTAGE_RATIO / ADC_MAX;
        if (value > VOLTAGE_ERROR_OFFSET) { value -= VOLTAGE_ERROR_OFFSET; } else { value = 0; }
        unsigned long errorValue = value / VOLTAGE_ERROR_FACTOR;
        return ((value - errorValue) / 100) * 100;
    } else {
        return LONG_MAX;
    }
}

unsigned long measure_getVoltageIn_10u() {
    return getVoltage_10u(9);
}

unsigned long measure_getVoltageOut_10u() {
    return getVoltage_10u(8);
}

unsigned long measure_getCurrent_10u() {
    unsigned int adc = getRawAdc(10);
    if (adc < ADC_MAX) {
        unsigned long value = adc * VREF * CURRENT_RATIO / ADC_MAX;
        if (value > CURRENT_ERROR_OFFSET) { value -= CURRENT_ERROR_OFFSET; } else { value = 0; }
        unsigned long errorValue = value / CURRENT_ERROR_FACTOR;
        return ((value - errorValue) / 100) * 100;
    } else {
        return LONG_MAX;
    }
}
