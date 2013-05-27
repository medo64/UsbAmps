#include <limits.h>
#include <pic.h>
#include "calibrate.h"
#include "measure.h"
#include "settings.h"
#include "lcd.h"
#include "config.h"


#define VERIFY_COUNT     1000
#define VERIFY_TOLERANCE   10


void calibrate() {
    unsigned int oldAdcCurrentOffset = settings_getAdcCurrentOffset();
    unsigned int newAdcCurrentOffset = INT_MAX;

    for (unsigned int i = 0; i < 10; i++) {
        unsigned int invalidCount = 0;
        for (unsigned int j = 0; j < VERIFY_COUNT; j++) {
            clrwdt();
            if (measure_getRawCurrent() > i) { invalidCount += 1; }
        }
        if (invalidCount <= VERIFY_TOLERANCE) {
            newAdcCurrentOffset = i;
            break;
        }
    }

    if (newAdcCurrentOffset < INT_MAX) {
        settings_setAdcCurrentOffset(newAdcCurrentOffset);
        for (unsigned char i=0; i<6; i++) {
            clrwdt();
            switch (i % 3) {
                case 0: lcd_writeMilliValue(newAdcCurrentOffset); break;
                case 1: lcd_writeCalibration(); break;
            }
            __delay_ms(250);
        }
    } else {
        if (oldAdcCurrentOffset == INT_MAX) { oldAdcCurrentOffset = 2; } //just a guess in case it is first run and there is an error.
        settings_setAdcCurrentOffset(oldAdcCurrentOffset);
        for (unsigned char i=0; i<6; i++) {
            clrwdt();
            switch (i % 3) {
                case 0: lcd_writeError(); break;
                case 1: lcd_writeMilliValue(oldAdcCurrentOffset); break;
                case 2: lcd_writeCalibration(); break;
            }
            __delay_ms(250);
        }
    }
}
