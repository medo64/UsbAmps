#include <pic16f1934.h>
#include <pic.h>
#include "config.h"
#include "lcd.h"
#include "measure.h"
#include "touch.h"


void measure();
void showMeasurement(unsigned char unitIndex, unsigned char typeIndex);
void processMeasurement(double value, double *avg, double *min, double *max);


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

            touch_b1_start();
            __delay_ms(1);
            if (touch_bx_stop() < 255) { //current/vout/powerout
                unitIndex = (unitIndex + 1) % 3;
                lcd_writeUnitAndType(unitIndex, typeIndex);
                clrwdt();
                __delay_ms(500);
            }

            touch_b2_start();
            __delay_ms(1);
            if (touch_bx_stop() < 255) { //avg/max/min
                typeIndex = (typeIndex + 1) % 3;
                lcd_writeUnitAndType(unitIndex, typeIndex);
                clrwdt();
                __delay_ms(500);
            }

            __delay_ms(17);
        }

        showMeasurement(unitIndex, typeIndex);
    }
}


const double SMOOTHING_FACTOR = 0.23;

void processMeasurement(double value, double *avg, double *min, double *max) {
    if (*avg < 0) { *avg = value; } else { *avg = *avg + (value - *avg) * SMOOTHING_FACTOR; }
    if (*min < 0) { *min = value; } else if (value < *min) { *min = value; }
    if (*max < 0) { *max = value; } else if (value > *max) { *max = value; }
}


double AvgCurrent = -1, MinCurrent = -1, MaxCurrent = -1;
double AvgVoltage = -1, MinVoltage = -1, MaxVoltage = -1;
double AvgPower   = -1, MinPower   = -1, MaxPower   = -1;

void measure() {
    double current = measure_getCurrent();
    double voltage = measure_getVoltageOut();
    double power   = current * voltage;
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
