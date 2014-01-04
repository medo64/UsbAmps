#include <limits.h>
#include <pic.h>
#include "calibrate.h"
#include "measure.h"
#include "settings.h"
#include "lcd.h"
#include "config.h"


#define CURRENT_OFFSET_MAX     5 //offset must be less than 5 (~10 mA)
#define VERIFY_COUNT        2000
#define VERIFY_TOLERANCE       5


void calibrate() {
    unsigned int oldAdcCurrentOffset = settings_getAdcCurrentOffset();
    unsigned int newAdcCurrentOffset = UINT_MAX;

    for (unsigned int i = 0; i < CURRENT_OFFSET_MAX; i++) {
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

    if (newAdcCurrentOffset < UINT_MAX) {
        settings_setAdcCurrentOffset(newAdcCurrentOffset);
        for (unsigned char i=0; i<6; i++) {
            clrwdt();
            switch (i % 3) {
                case 0: lcd_writeMilliValue(newAdcCurrentOffset); break;
                case 1: lcd_writeCalibration(); break;
            }
            wait_250ms();
        }
    } else {
        for (unsigned char i=0; i<6; i++) {
            clrwdt();
            switch (i % 3) {
                case 0: lcd_writeError(); break;
                case 1: lcd_writeMilliValue(oldAdcCurrentOffset); break;
                case 2: lcd_writeCalibration(); break;
            }
            wait_250ms();
        }
    }
}
