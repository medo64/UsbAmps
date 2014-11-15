#include <pic.h>
#include "config.h"
#include "measure.h"
#include "settings.h"


#define VREF       2048L
#define ADC_COUNT  1024L
#define ADC_MAX    1023L

#define ADC_CHANNEL_CURRENT     10
#define ADC_CHANNEL_VOLTAGE_IN   9
#define ADC_CHANNEL_VOLTAGE_OUT  8

#define VOLTAGE_RATIO  320L //3.2:1 ratio (2K2:1K)
#define CURRENT_RATIO  100L //1V is 1A (3V max)

uint16_t AdcVoltageOffset;
uint16_t AdcCurrentOffset;

#define VOLTAGE_ERROR_SCALE   -250 //-0.25%
#define CURRENT_ERROR_SCALE  -1000 //-1.00%


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

    measure_reinit();
}

void measure_reinit() {
    AdcCurrentOffset = settings_getAdcCurrentOffset();
    AdcVoltageOffset = settings_getAdcVoltageOffset();
    if (AdcCurrentOffset >= ADC_COUNT) { AdcCurrentOffset = 0; }
    if (AdcVoltageOffset >= ADC_COUNT) { AdcVoltageOffset = 0; }
}


uint16_t getRawAdc(uint8_t channel, uint8_t offset) {
    ADCON0 = (channel << 2) | 0x01; //Analog Channel Select bits / leave ADON
    __delay_us(10); //to discharge holding cap if there was measurement just before (at least 10us)
    ADGO = 1; //Setting this bit starts an A/D conversion cycle.
    while(ADGO); //A/D conversion cycle in progress.
    ADON = 0; //ADC is disabled and consumes no operating current
    uint16_t adc = ADRES;
    if (adc <= offset) { return 0; } else { return adc - offset; }
}


uint16_t measure_getVoltage_1m() {
    uint16_t adc = getRawAdc(ADC_CHANNEL_VOLTAGE_OUT, AdcVoltageOffset); //measure consuption of a connected device (ADC_CHANNEL_VOLTAGE_OUT), not whole UsbAmps (ADC_CHANNEL_VOLTAGE_IN)
    if (adc < ADC_MAX) {
        int32_t value = (int32_t)adc * VREF * VOLTAGE_RATIO / ADC_MAX / 100L;
        int32_t errorValue = value * VOLTAGE_ERROR_SCALE / 1000L / 100L;
        int32_t newValue = value + errorValue;
        if (newValue < 0) { newValue = 0; }
        return (uint16_t)newValue;
    } else {
        return INT16_MAX;
    }
}

uint16_t measure_getCurrent_1m() {
    uint16_t adc = getRawAdc(ADC_CHANNEL_CURRENT, AdcCurrentOffset);
    if (adc < ADC_MAX) {
        int32_t value = (int32_t)adc * VREF * CURRENT_RATIO / ADC_MAX / 100L;
        int32_t errorValue = value * CURRENT_ERROR_SCALE / 1000L / 100L;
        int32_t newValue = value + errorValue;
        if (newValue < 0) { newValue = 0; }
        return (uint16_t)newValue;
    } else {
        return INT16_MAX;
    }
}


uint16_t measure_getRawCurrent() {
    return getRawAdc(ADC_CHANNEL_CURRENT, 0);
}
