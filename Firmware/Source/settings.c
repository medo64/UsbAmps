#include <stdint.h>
#include "settings.h"


__eeprom uint16_t EEPROM_ADC_VOLTAGE_OFFSET = 0;
__eeprom uint16_t EEPROM_ADC_CURRENT_OFFSET = UINT16_MAX;


uint16_t settings_getAdcVoltageOffset() {
    return EEPROM_ADC_VOLTAGE_OFFSET;
}

void settings_setAdcVoltageOffset(uint16_t value) {
    EEPROM_ADC_VOLTAGE_OFFSET = value;
}


uint16_t settings_getAdcCurrentOffset() {
    return EEPROM_ADC_CURRENT_OFFSET;
}

void settings_setAdcCurrentOffset(uint16_t value) {
    EEPROM_ADC_CURRENT_OFFSET = value;
}
