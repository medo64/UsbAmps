#ifndef SETTINGS_H
#define SETTINGS_H


#include <stdbool.h>
#include <stdint.h>


uint8_t settings_getAdcVoltageOffset();
void settings_setAdcVoltageOffset(uint8_t value);

uint8_t settings_getAdcCurrentOffset();
void settings_setAdcCurrentOffset(uint8_t value);

#endif
