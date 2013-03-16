#include <pic16f1934.h>
#include <pic.h>
#include <limits.h>
#include "config.h"
#include "lcd.h"
#include "measure.h"
#include "touch.h"


void measure();
void resetMeasurements();
void showMeasurement(unsigned char unitIndex, unsigned char typeIndex);


void main() {
    init();
    lcd_init();
    measure_init();
    touch_init();

    unsigned char unitIndex = 0; //0:current; 1:voltage; 2:power
    unsigned char typeIndex = 0; //0:avg; 1:max; 2:min
    unsigned char delayDisplay = 0;

    while (1) {
        clrwdt();
        measure();
        if (delayDisplay > 0) { delayDisplay--; continue; }

        unsigned char buttons = 0x00;
        if (touch_b1_pressed()) { buttons |= 0x01; }
        if (touch_b2_pressed()) { buttons |= 0x02; }
        if (buttons > 0x00) {
            switch (buttons) {
                case 0x01: //current/vout/powerout
                    unitIndex = (unitIndex + 1) % 3;
                    break;
                case 0x02: //avg/max/min
                    typeIndex = (typeIndex + 1) % 3;
                    break;
                case 0x03: //reset min/max
                    unitIndex = 0;
                    typeIndex = 0;
                    resetMeasurements();
                    break;
            }
            lcd_writeUnitAndType(unitIndex, typeIndex);
            delayDisplay = 1;
        } else { //show
            showMeasurement(unitIndex, typeIndex);
        }
    }
}


void processMinMax(unsigned long value, unsigned long *min, unsigned long *max) {
    if (value == LONG_MAX) { return; }
    if (*min == LONG_MAX) { *min = value; } else if ((value > 0) && (value < *min)) { *min = value; }
    if (*max == LONG_MAX) { *max = value; } else if (value > *max) { *max = value; }
}

void processAvg(unsigned long sum, unsigned int count, unsigned long *avg) {
    if ((sum == LONG_MAX) || (count == 0)) {
        *avg = LONG_MAX;
    } else {
        unsigned long value = sum / count;
        if (*avg == LONG_MAX) { *avg = value; } else { *avg = (*avg + value) / 2; }
    }
}

const unsigned char AVG_COUNT = 100;
unsigned long AvgCurrent = LONG_MAX, MinCurrent = LONG_MAX, MaxCurrent = LONG_MAX;
unsigned long AvgVoltage = LONG_MAX, MinVoltage = LONG_MAX, MaxVoltage = LONG_MAX;
unsigned long AvgPower   = LONG_MAX, MinPower   = LONG_MAX, MaxPower   = LONG_MAX;

void measure() {
    unsigned long sumCurrent = 0, sumVoltage = 0, sumPower = 0;
    unsigned int  cntCurrent = 0, cntVoltage = 0, cntPower = 0;
    for (unsigned char a=0; a<AVG_COUNT; a++) {
        unsigned long current = measure_getCurrent_10u();
        unsigned long voltage = measure_getVoltageOut_10u();
        unsigned long power   = ((current == LONG_MAX) || (voltage == LONG_MAX)) ? LONG_MAX : (current/100) * (voltage/100) / 10;
        processMinMax(current, &MinCurrent, &MaxCurrent);
        processMinMax(voltage, &MinVoltage, &MaxVoltage);
        processMinMax(power,   &MinPower,   &MaxPower);
        if (current < LONG_MAX) { sumCurrent += current; cntCurrent += 1; }
        if (voltage < LONG_MAX) { sumVoltage += voltage; cntVoltage += 1; }
        if (power   < LONG_MAX) { sumPower   += power;   cntPower   += 1; }
    }
    processAvg(sumCurrent, cntCurrent, &AvgCurrent);
    processAvg(sumVoltage, cntVoltage, &AvgVoltage);
    processAvg(sumPower  , cntPower,   &AvgPower);
}

void resetMeasurements() {
    MinCurrent = LONG_MAX;
    MaxCurrent = LONG_MAX;
    MinVoltage = LONG_MAX;
    MaxVoltage = LONG_MAX;
    MinPower   = LONG_MAX;
    MaxPower   = LONG_MAX;
}

void showMeasurement(unsigned char unitIndex, unsigned char typeIndex) {
    switch (unitIndex * 3 + typeIndex) {
        case 0: lcd_writeNumber(AvgCurrent); break;
        case 1: lcd_writeNumber(MaxCurrent); break;
        case 2: lcd_writeNumber(MinCurrent); break;
        case 3: lcd_writeNumber(AvgVoltage); break;
        case 4: lcd_writeNumber(MaxVoltage); break;
        case 5: lcd_writeNumber(MinVoltage); break;
        case 6: lcd_writeNumber(AvgPower); break;
        case 7: lcd_writeNumber(MaxPower); break;
        case 8: lcd_writeNumber(MinPower); break;
    }
}
