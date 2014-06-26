#include <pic16f1934.h>
#include <pic.h>
#include <limits.h>
#include <stdint.h>
#include "calibrate.h"
#include "config.h"
#include "lcd.h"
#include "measure.h"
#include "touch.h"
#include "settings.h"
#include "io.h"
#include "timer1.h"
#include "option.h"


#define BUTTON_OUTER  0x01
#define BUTTON_INNER  0x02

void measure();
void resetStats();
void showMeasurement(unsigned char unitIndex, unsigned char typeIndex);
unsigned char getButtonMask();

unsigned int AvgCurrent  = INT_MAX, MinCurrent  = INT_MAX, MaxCurrent  = INT_MAX;
unsigned int AvgVoltage  = INT_MAX, MinVoltage  = INT_MAX, MaxVoltage  = INT_MAX;
unsigned int AvgPower    = INT_MAX, MinPower    = INT_MAX, MaxPower    = INT_MAX;

uint32_t TotalCapacity = 0;


void main() {
    init();
    lcd_init();
    measure_init();
    touch_init();
    io_init();

    lcd_writeLoading();


    //initial calibration
    if (settings_getAdcCurrentOffset() == UINT_MAX) { //on first run you are still connected to PICkit; just wait
        clrwdt();
        wait_250ms(); //we ignore short power-up/down events
        settings_setAdcCurrentOffset(UINT_MAX-1);
        while(true) {
            clrwdt();
            lcd_writeAll();
            wait_250ms();
            clrwdt();
            lcd_clear();
            wait_250ms();
        }
    } else if (settings_getAdcCurrentOffset() == UINT_MAX-1) { //on first real run do calibration
        lcd_writeCalibration();
        wait_250ms();
        calibrate();
        measure_reinit();
    }

    lcd_writeLoading();


    //check startup keys presses
    if (touch_outer_pressed() && touch_inner_pressed()) { //both keys are needed to enter calibrate
        bool enterCalibration = true;
        for (unsigned char i=0; i<250; i++) { //first 2.5 seconds show nothing
            clrwdt();
            if (!(touch_outer_pressed() && touch_inner_pressed())) { enterCalibration = false; break; }
            wait_10ms();
        }
        for (unsigned char i=0; i<250; i++) { //quick blink next 2.5 seconds
            if (enterCalibration) {
                clrwdt();
                if (!(touch_outer_pressed() && touch_inner_pressed())) { enterCalibration = false; break; }
                if (i % 30 == 0) {
                    lcd_writeStatsReset();
                } else if (i % 30 == 15) {
                    lcd_writeLoading();
                }
                wait_10ms();
            }
        }
        if (enterCalibration) {
            lcd_writeCalibration();
            calibrate();
            measure_reinit();
        }
        lcd_writeLoading();
    } else if (touch_inner_pressed()) { //inner key signifies High power mode (no USB connectivity)
        lcd_writeHighPower();
        io_dshort_on();
    }


    while (touch_inner_pressed() || touch_outer_pressed()) {  clrwdt(); } //both keys must be released to start


    unsigned char unitIndex = 0; //0:current; 1:voltage; 2:power
    unsigned char typeIndex = 0; //0:avg; 1:max; 2:min
    unsigned char phaseCounter = 0;


    timer1_init();

    while (true) {
        phaseCounter = (phaseCounter + 1) % SETTINGS_LONG_BLINK_MULTIPLIER;

        measure();

        unsigned char buttons = getButtonMask();
        if (buttons != 0) {

            //display to user what comes next
            unsigned char counter = 0;
            unsigned char nextButtons = buttons;
            while (nextButtons != 0) {
                clrwdt();
                measure();
                nextButtons = getButtonMask();
                buttons |= nextButtons;
                switch (buttons) {
                    case BUTTON_OUTER: lcd_writeUnitAndType((unitIndex + 1) % OPTION_COUNT, 0); break; //current/vout/powerout/capacity
                    case BUTTON_INNER: lcd_writeUnitAndType(unitIndex, (typeIndex + 1) % 3); break; //avg/max/min
                }

                //detect long key press
                counter += 1;
                if (counter > SETTINGS_LONG_PRESS_MULTIPLIER) {
                    counter = 0;
                    switch (buttons) {
                        case BUTTON_OUTER: //nothing
                            lcd_clear();
                            break;
                        case BUTTON_INNER: //long press on inner - reset values
                            lcd_writeStatsReset();
                            resetStats();
                            break;
                        default: //any other key (e.g. both)
                            lcd_writeUnitAndType(unitIndex, typeIndex);
                            break;
                    }
                    while (getButtonMask() != 0) { clrwdt(); }
                    buttons = 0; //don't do any other action
                    showMeasurement(unitIndex, typeIndex);
                    break;
                }
            }

            //actual work for short key presses
            switch (buttons) {
                case BUTTON_OUTER: //current/vout/powerout/capacity
                    unitIndex = (unitIndex + 1) % OPTION_COUNT;
                    typeIndex = 0;
                    break;
                case BUTTON_INNER: //avg/max/min
                    typeIndex = (typeIndex + 1) % 3;
                    break;
            }

        } else if (timer1_hasSecondPassed()) { //has one second passed

            if (AvgCurrent < INT_MAX) {
                TotalCapacity += AvgCurrent;
            }

        } else { //display the value
            if (SETTINGS_BLINK_ON_MIN_MAX && (typeIndex != 0) && (phaseCounter == 0) && (unitIndex != 4)) { //if not current measurement, blink occasionally (except on capacity)
                lcd_clear();
            } else {
                showMeasurement(unitIndex, typeIndex);
            }
        }
    }
}

unsigned char getButtonMask() {
    unsigned char mask = 0;
    if (touch_outer_pressed()) { mask |= BUTTON_OUTER; }
    if (touch_inner_pressed()) { mask |= BUTTON_INNER; }
    return mask;
}


void processMinMax(unsigned int value, unsigned int *min, unsigned int *max) {
    if (value == INT_MAX) { return; }
    if (*min == INT_MAX) { *min = value; } else if (value < *min) { *min = value; }
    if (*max == INT_MAX) { *max = value; } else if (value > *max) { *max = value; }
}

void processAvg(unsigned long sum, unsigned int count, unsigned int *avg) {
    if ((sum == INT_MAX) || (count == 0)) {
        *avg = INT_MAX;
    } else {
        unsigned long value = sum / count;
        *avg = (unsigned int)value;
    }
}



void measure() {
    clrwdt();
    unsigned long sumCurrent = 0, sumVoltage = 0, sumPower = 0;
    unsigned int  cntCurrent = 0, cntVoltage = 0, cntPower = 0;
    for (unsigned char i=0; i<SETTINGS_AVERAGE_COUNT; i++) {
        unsigned int current = measure_getCurrent_1m();
        unsigned int voltage = measure_getVoltageOut_1m();
        unsigned int power   = ((current == INT_MAX) || (voltage == INT_MAX)) ? INT_MAX : (unsigned int)((unsigned long)current * (unsigned long)voltage / 1000L);
        processMinMax(current, &MinCurrent, &MaxCurrent);
        processMinMax(voltage, &MinVoltage, &MaxVoltage);
        processMinMax(power,   &MinPower,   &MaxPower);
        if (current < INT_MAX) { sumCurrent += current; cntCurrent += 1; }
        if (voltage < INT_MAX) { sumVoltage += voltage; cntVoltage += 1; }
        if (power   < INT_MAX) { sumPower   += power;   cntPower   += 1; }
    }
    processAvg(sumCurrent, cntCurrent, &AvgCurrent);
    processAvg(sumVoltage, cntVoltage, &AvgVoltage);
    processAvg(sumPower  , cntPower,   &AvgPower);
}

void resetStats() {
    MinCurrent    = AvgCurrent;
    MaxCurrent    = AvgCurrent;
    MinVoltage    = AvgVoltage;
    MaxVoltage    = AvgVoltage;
    MinPower      = AvgPower;
    MaxPower      = AvgPower;
#if SETTINGS_SHOW_CAPACITY
    TotalCapacity = 0;
#endif
}

void showMeasurement(unsigned char unitIndex, unsigned char typeIndex) {
    switch (unitIndex * 3 + typeIndex) {
        case 0: lcd_writeMilliValue(AvgCurrent); break;
        case 1: lcd_writeMilliValue(MaxCurrent); break;
        case 2: lcd_writeMilliValue(MinCurrent); break;
        case 3: lcd_writeValue(AvgVoltage); break;
        case 4: lcd_writeValue(MaxVoltage); break;
        case 5: lcd_writeValue(MinVoltage); break;
        case 6: lcd_writeValue(AvgPower); break;
        case 7: lcd_writeValue(MaxPower); break;
        case 8: lcd_writeValue(MinPower); break;
#if SETTINGS_SHOW_CAPACITY
        case 9:
        case 10:
        case 11: { //capacity
            unsigned int capacityInmAh = (unsigned int)(TotalCapacity / 3600);
            lcd_writeMilliValue(capacityInmAh);
            break;
        }
#endif
    }
}
