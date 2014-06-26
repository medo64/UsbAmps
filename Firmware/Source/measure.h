#ifndef MEASURE_H
#define	MEASURE_H

#include <stdint.h>


void measure_init();
void measure_reinit();

uint16_t measure_getVoltageIn_1m();
uint16_t measure_getVoltageOut_1m();
uint16_t measure_getCurrent_1m();
uint16_t measure_getRawCurrent();

#endif
