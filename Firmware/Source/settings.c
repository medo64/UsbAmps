#include <stdint.h>
#include "settings.h"


__eeprom uint8_t EEPROM_ADC_VOLTAGE_OFFSET = 0;
__eeprom uint8_t EEPROM_ADC_CURRENT_OFFSET = UINT8_MAX;


uint8_t settings_getAdcVoltageOffset() {
    return EEPROM_ADC_VOLTAGE_OFFSET;
}

void settings_setAdcVoltageOffset(uint8_t value) {
    EEPROM_ADC_VOLTAGE_OFFSET = value;
}


uint8_t settings_getAdcCurrentOffset() {
    return EEPROM_ADC_CURRENT_OFFSET;
}

void settings_setAdcCurrentOffset(uint8_t value) {
    EEPROM_ADC_CURRENT_OFFSET = value;
}
