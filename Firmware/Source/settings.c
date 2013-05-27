#include <limits.h>
#include "settings.h"


__eeprom unsigned int EEPROM_ADC_VOLTAGE_OFFSET = 0;
__eeprom unsigned int EEPROM_ADC_CURRENT_OFFSET = INT_MAX;


unsigned int settings_getAdcVoltageOffset() {
    return EEPROM_ADC_VOLTAGE_OFFSET;
}

void settings_setAdcVoltageOffset(unsigned int value) {
    EEPROM_ADC_VOLTAGE_OFFSET = value;
}


unsigned int settings_getAdcCurrentOffset() {
    return EEPROM_ADC_CURRENT_OFFSET;
}

void settings_setAdcCurrentOffset(unsigned int value) {
    EEPROM_ADC_CURRENT_OFFSET = value;
}
