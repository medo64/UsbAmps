#include <pic.h>
#include <limits.h>
#include "config.h"


#define VREF       2048L
#define ADC_COUNT  1024L
#define ADC_MAX    1023L

#define VOLTAGE_RATIO  320L //3.2:1 ratio (2K2:1K)
#define CURRENT_RATIO  100L //1V is 1A (3V max)

const unsigned long VOLTAGE_ADC_OFFSET = 4; //just a guess
const unsigned long CURRENT_ADC_OFFSET = 2; //just a guess

const unsigned long VOLTAGE_ERROR_SCALE =  500; //0.5%
const unsigned long CURRENT_ERROR_SCALE = 1000; //1.0%


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

unsigned long getVoltage_10u(unsigned char channel) {
    unsigned int adc = getRawAdc(channel);
    if (adc < ADC_MAX) {
        if (adc <= VOLTAGE_ADC_OFFSET) { adc = 0; } else { adc = adc; }
        unsigned long value = adc * VREF * VOLTAGE_RATIO / ADC_MAX;
        unsigned long errorValue = value * VOLTAGE_ERROR_SCALE / 100000;
        return ((value - errorValue) / 1000) * 1000;
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
        if (adc <= CURRENT_ADC_OFFSET) { adc = 0; } else { adc = adc; }
        unsigned long value = adc * VREF * CURRENT_RATIO / ADC_MAX;
        unsigned long errorValue = value * CURRENT_ERROR_SCALE / 100000;
        return ((value - errorValue) / 1000) * 1000;
    } else {
        return LONG_MAX;
    }
}
