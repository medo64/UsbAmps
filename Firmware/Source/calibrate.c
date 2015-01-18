#include <pic.h>
#include <stdint.h>
#include "calibrate.h"
#include "measure.h"
#include "settings.h"
#include "lcd.h"
#include "config.h"


#define CURRENT_OFFSET_MAX     5 //offset must be less or equal to 5 (~10 mA)
#define VERIFY_COUNT        2000
#define VERIFY_TOLERANCE       5


void calibrate() {
    uint16_t oldAdcCurrentOffset = settings_getAdcCurrentOffset();
    uint16_t newAdcCurrentOffset = UINT16_MAX;

    for (uint16_t i = 0; i <= CURRENT_OFFSET_MAX; i++) {
        uint16_t invalidCount = 0;
        for (uint16_t j = 0; j < VERIFY_COUNT; j++) {
            clrwdt();
            if (measure_getRawCurrent() > i) { invalidCount += 1; }
        }
        if (invalidCount <= VERIFY_TOLERANCE) {
            newAdcCurrentOffset = i;
            break;
        }
    }

    if (newAdcCurrentOffset < UINT16_MAX) {
        settings_setAdcCurrentOffset(newAdcCurrentOffset);
        settings_setAdcVoltageOffset(newAdcCurrentOffset / 2); //just set voltage to half of current offset
        for (uint8_t i=0; i<6; i++) {
            clrwdt();
            switch (i % 3) {
                case 0: lcd_writeMilliValue(newAdcCurrentOffset); break;
                case 1: lcd_writeCalibration(); break;
            }
            wait_250ms();
        }
    } else {
        for (uint8_t i=0; i<6; i++) {
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
