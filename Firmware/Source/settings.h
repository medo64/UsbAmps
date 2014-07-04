#ifndef SETTINGS_H
#define SETTINGS_H


#include <stdbool.h>
#include <stdint.h>


uint16_t settings_getAdcVoltageOffset();
void settings_setAdcVoltageOffset(uint16_t value);

uint16_t settings_getAdcCurrentOffset();
void settings_setAdcCurrentOffset(uint16_t value);

#endif
