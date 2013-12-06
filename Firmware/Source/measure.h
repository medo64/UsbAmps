#ifndef MEASURE_H
#define	MEASURE_H

void measure_init();
void measure_reinit();

unsigned int measure_getVoltageIn_1m();
unsigned int measure_getVoltageOut_1m();
unsigned int measure_getCurrent_1m();
unsigned int measure_getRawCurrent();

#endif	/* MEASURE_H */
