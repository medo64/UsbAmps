#include <pic.h>
#include <limits.h>
#include "config.h"
#include "settings.h"


#define VREF       2048L
#define ADC_COUNT  1024L
#define ADC_MAX    1023L

#define ADC_CHANNEL_CURRENT     10
#define ADC_CHANNEL_VOLTAGE_IN   9
#define ADC_CHANNEL_VOLTAGE_OUT  8

#define VOLTAGE_RATIO  320L //3.2:1 ratio (2K2:1K)
#define CURRENT_RATIO  100L //1V is 1A (3V max)

unsigned int AdcVoltageOffset;
unsigned int AdcCurrentOffset;

const int VOLTAGE_ERROR_SCALE =     0; // 0.0%
const int CURRENT_ERROR_SCALE = -1000; //-1.0%


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

    AdcVoltageOffset = settings_getAdcVoltageOffset();
    AdcCurrentOffset = settings_getAdcCurrentOffset();
}

void measure_reinit() {
    AdcVoltageOffset = settings_getAdcVoltageOffset();
    AdcCurrentOffset = settings_getAdcCurrentOffset();
}


unsigned int getRawAdc(unsigned char channel) {
    ADCON0 = (channel << 2) | 0x01; //Analog Channel Select bits / leave ADON
    __delay_us(10); //to discharge holding cap if there was measurement just before (at least 10us)
    ADGO = 1; //Setting this bit starts an A/D conversion cycle.
    while(ADGO); //A/D conversion cycle in progress.
    ADON = 0; //ADC is disabled and consumes no operating current
    return ADRES;
}

unsigned int getVoltage_1m(unsigned char channel) {
    unsigned int adc = getRawAdc(channel);
    if (adc < ADC_MAX) {
        if (adc <= AdcVoltageOffset) { adc = 0; } else { adc = adc - AdcVoltageOffset; }
        long value = (long)adc * VREF * VOLTAGE_RATIO / ADC_MAX / 100L;
        long errorValue = value * VOLTAGE_ERROR_SCALE / 1000L / 100L;
        long newValue = value + errorValue;
        if (newValue < 0) { newValue = 0; }
        return (unsigned int)newValue;
    } else {
        return INT_MAX;
    }
}

unsigned int measure_getVoltageIn_10u() {
    return getVoltage_1m(ADC_CHANNEL_VOLTAGE_IN);
}

unsigned int measure_getVoltageOut_1m() {
    return getVoltage_1m(ADC_CHANNEL_VOLTAGE_OUT);
}

unsigned int measure_getCurrent_1m() {
    unsigned int adc = getRawAdc(ADC_CHANNEL_CURRENT);
    if (adc < ADC_MAX) {
        if (adc <= AdcCurrentOffset) { adc = 0; } else { adc = adc - AdcCurrentOffset; }
        long value = (long)adc * VREF * CURRENT_RATIO / ADC_MAX / 100L;
        long errorValue = value * CURRENT_ERROR_SCALE / 1000L / 100L;
        long newValue = value + errorValue;
        if (newValue < 0) { newValue = 0; }
        return (unsigned int)newValue;
    } else {
        return INT_MAX;
    }
}


unsigned int measure_getRawCurrent() {
    return getRawAdc(ADC_CHANNEL_CURRENT);
}
