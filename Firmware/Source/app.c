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
unsigned char getButtonMask();


void main() {
    init();
    lcd_init();
    measure_init();
    touch_init();

    unsigned char unitIndex = 0; //0:current; 1:voltage; 2:power
    unsigned char typeIndex = 0; //0:avg; 1:max; 2:min

    while (1) {
        measure();

        unsigned char buttons = getButtonMask();
        if (buttons != 0) {
            unsigned char counter = 0;
            unsigned char nextButtons = buttons;
            while (nextButtons != 0) {
                counter = (counter + 1) % 2;
                measure();
                nextButtons = getButtonMask();
                buttons |= nextButtons;
                switch (buttons) {
                    case 0x01: lcd_writeUnitAndType((unitIndex + 1) % 3, typeIndex); break; //current/vout/powerout
                    case 0x02: lcd_writeUnitAndType(unitIndex, (typeIndex + 1) % 3); break; //avg/max/min
                    case 0x03: //reset min/max
                        if (counter == 0) {
                            lcd_clear();
                        } else {
                            lcd_writeUnitAndType(unitIndex, typeIndex);
                        }
                        break;
                }
            }
            switch (buttons) {
                case 0x01: unitIndex = (unitIndex + 1) % 3; break; //current/vout/powerout
                case 0x02: typeIndex = (typeIndex + 1) % 3; break; //avg/max/min
                case 0x03: resetMeasurements();             break; //reset min/max
            }
        } else {
            showMeasurement(unitIndex, typeIndex);
        }
    }
}

unsigned char getButtonMask() {
    unsigned char mask = 0x00;
    if (touch_b1_pressed()) { mask |= 0x01; }
    if (touch_b2_pressed()) { mask |= 0x02; }
    return mask;
}


void processMinMax(unsigned long value, unsigned long *min, unsigned long *max) {
    if (value == LONG_MAX) { return; }
    if (*min == LONG_MAX) { *min = value; } else if (value < *min) { *min = value; }
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
    clrwdt();
    unsigned long sumCurrent = 0, sumVoltage = 0, sumPower = 0;
    unsigned int  cntCurrent = 0, cntVoltage = 0, cntPower = 0;
    for (unsigned char i=0; i<AVG_COUNT; i++) {
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
