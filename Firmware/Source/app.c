#include <pic16f1934.h>
#include <pic.h>
#include <limits.h>
#include "config.h"
#include "lcd.h"
#include "measure.h"
#include "touch.h"


void measure();
void showMeasurement(unsigned char unitIndex, unsigned char typeIndex);
void processMeasurement(unsigned long value, unsigned long *avg, unsigned long *min, unsigned long *max);
void resetMeasurements();

void main() {
    init();
    lcd_init();
    measure_init();
    touch_init();

    unsigned char unitIndex = 0; //0:current; 1:voltage; 2:power
    unsigned char typeIndex = 0; //0:avg; 1:max; 2:min

    while (1) {
        for (int i=0; i<10; i++) {
            clrwdt();
            measure(unitIndex, typeIndex);

            unsigned char buttons = 0x00;

            touch_b1_start();
            __delay_ms(1);
            if (touch_bx_stop() < 255) { buttons |= 0x01; }

            touch_b2_start();
            __delay_ms(1);
            if (touch_bx_stop() < 255) { buttons |= 0x02; }

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
                clrwdt();
                __delay_ms(500);
            }

            __delay_ms(17);
        }

        showMeasurement(unitIndex, typeIndex);
    }
}


void processMeasurement(unsigned long value, unsigned long *avg, unsigned long *min, unsigned long *max) {
    *avg = value;
    if (*min == LONG_MAX) { *min = value; } else if ((value > 0) && (value < *min)) { *min = value; }
    if (*max == LONG_MAX) { *max = value; } else if (value > *max) { *max = value; }
}


unsigned long AvgCurrent = LONG_MAX, MinCurrent = LONG_MAX, MaxCurrent = LONG_MAX;
unsigned long AvgVoltage = LONG_MAX, MinVoltage = LONG_MAX, MaxVoltage = LONG_MAX;
unsigned long AvgPower   = LONG_MAX, MinPower   = LONG_MAX, MaxPower   = LONG_MAX;

void resetMeasurements() {
    MinCurrent = LONG_MAX;
    MaxCurrent = LONG_MAX;
    MinVoltage = LONG_MAX;
    MaxVoltage = LONG_MAX;
    MinPower   = LONG_MAX;
    MaxPower   = LONG_MAX;
}

void measure() {
    unsigned long current = measure_getCurrent_10u();
    unsigned long voltage = measure_getVoltageOut_10u();
    unsigned long power   = current * voltage;
    processMeasurement(current, &AvgCurrent, &MinCurrent, &MaxCurrent);
    processMeasurement(voltage, &AvgVoltage, &MinVoltage, &MaxVoltage);
    processMeasurement(power,   &AvgPower,   &MinPower,   &MaxPower);
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
