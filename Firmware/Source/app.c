#include <pic16f1934.h>
#include <pic.h>
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
void showMeasurement(uint8_t unitIndex, uint8_t typeIndex);
uint8_t getButtonMask();

uint16_t AvgCurrent  = INT16_MAX, MinCurrent  = INT16_MAX, MaxCurrent  = INT16_MAX;
uint16_t AvgVoltage  = INT16_MAX, MinVoltage  = INT16_MAX, MaxVoltage  = INT16_MAX;
uint16_t AvgPower    = INT16_MAX, MinPower    = INT16_MAX, MaxPower    = INT16_MAX;

uint32_t TotCapacity = 0;


void main() {
    init();
    lcd_init();
    measure_init();
    touch_init();
    io_init();

    lcd_writeLoading();


    //initial calibration
    if (settings_getAdcCurrentOffset() == UINT16_MAX) { //on first run you are still connected to PICkit; just wait
        clrwdt();
        wait_250ms(); //we ignore short power-up/down events
        settings_setAdcCurrentOffset(UINT16_MAX-1);
        while(true) {
            clrwdt();
            lcd_writeAll();
            wait_250ms();
            clrwdt();
            lcd_clear();
            wait_250ms();
        }
    } else if (settings_getAdcCurrentOffset() == UINT16_MAX-1) { //on first real run do calibration
        lcd_writeCalibration();
        wait_250ms();
        calibrate();
        measure_reinit();
    }

    lcd_writeLoading();


    //check startup keys presses
    if (touch_outer_pressed() && touch_inner_pressed()) { //both keys are needed to enter calibrate
        bool enterCalibration = true;
        for (uint8_t i=0; i<250; i++) { //first 2.5 seconds show nothing
            clrwdt();
            if (!(touch_outer_pressed() && touch_inner_pressed())) { enterCalibration = false; break; }
            wait_10ms();
        }
        for (uint8_t i=0; i<250; i++) { //quick blink next 2.5 seconds
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


    uint8_t unitIndex = 0; //0:current; 1:voltage; 2:power
    uint8_t typeIndex = 0; //0:avg; 1:max; 2:min
    uint8_t phaseCounter = 0;


    timer1_init();

    while (true) {
        phaseCounter = (phaseCounter + 1) % SETTINGS_LONG_BLINK_MULTIPLIER;

        measure();

        uint8_t buttons = getButtonMask();
        if (buttons != 0) {

            //display to user what comes next
            uint8_t counter = 0;
            uint8_t nextButtons = buttons;
            while (nextButtons != 0) {
                clrwdt();
                measure();
                nextButtons = getButtonMask();
                buttons |= nextButtons;
                switch (buttons) {
                    case BUTTON_OUTER: lcd_writeUnitAndType((unitIndex + 1) % OPTION_COUNT, 0); break; //current/vout/powerout/capacity
                    case BUTTON_INNER:
                        if (unitIndex != OPTION_UNIT_CAPACITY) {
                            lcd_writeUnitAndType(unitIndex, (typeIndex + 1) % 3); break; //avg/max/min
                        } else {
                            lcd_writeUnitAndType(unitIndex, typeIndex); break; //total
                        }
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
                    if (unitIndex != OPTION_UNIT_CAPACITY) {
                        typeIndex = (typeIndex + 1) % 3;
                    } else {
                        typeIndex = 0;
                    }
                    break;
            }

        } else if (timer1_hasSecondPassed()) { //has one second passed

            if (AvgCurrent < INT16_MAX) {
                TotCapacity += AvgCurrent;
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

uint8_t getButtonMask() {
    uint8_t mask = 0;
    if (touch_outer_pressed()) { mask |= BUTTON_OUTER; }
    if (touch_inner_pressed()) { mask |= BUTTON_INNER; }
    return mask;
}


void processMinMax(uint16_t value, uint16_t *min, uint16_t *max) {
    if (value == INT16_MAX) { return; }
    if (*min == INT16_MAX) { *min = value; } else if (value < *min) { *min = value; }
    if (*max == INT16_MAX) { *max = value; } else if (value > *max) { *max = value; }
}

void processAvg(uint32_t sum, uint16_t count, uint16_t *avg) {
    if ((sum == INT16_MAX) || (count == 0)) {
        *avg = INT16_MAX;
    } else {
        uint32_t value = sum / count;
        *avg = (uint16_t)value;
    }
}



void measure() {
    clrwdt();
    uint32_t sumCurrent = 0, sumVoltage = 0, sumPower = 0;
    uint16_t cntCurrent = 0, cntVoltage = 0, cntPower = 0;
    for (uint8_t i=0; i<SETTINGS_AVERAGE_COUNT; i++) {
        uint16_t current = measure_getCurrent_1m();
        uint16_t voltage = measure_getVoltageOut_1m();
        uint16_t power   = ((current == INT16_MAX) || (voltage == INT16_MAX)) ? INT16_MAX : (uint16_t)(((uint32_t)current * (uint32_t)voltage) / 1000L);
        processMinMax(current, &MinCurrent, &MaxCurrent);
        processMinMax(voltage, &MinVoltage, &MaxVoltage);
        processMinMax(power,   &MinPower,   &MaxPower);
        if (current < INT16_MAX) { sumCurrent += current; cntCurrent += 1; }
        if (voltage < INT16_MAX) { sumVoltage += voltage; cntVoltage += 1; }
        if (power   < INT16_MAX) { sumPower   += power;   cntPower   += 1; }
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
    TotCapacity = 0;
}

void showMeasurement(uint8_t unitIndex, uint8_t typeIndex) {
    switch (unitIndex * 3 + typeIndex) {
        case OPTION_UNITTYPE_CURRENT_AVG:  lcd_writeMilliValue(AvgCurrent); break;
        case OPTION_UNITTYPE_CURRENT_MAX:  lcd_writeMilliValue(MaxCurrent); break;
        case OPTION_UNITTYPE_CURRENT_MIN:  lcd_writeMilliValue(MinCurrent); break;
        case OPTION_UNITTYPE_VOLTAGE_AVG:  lcd_writeValue(AvgVoltage); break;
        case OPTION_UNITTYPE_VOLTAGE_MAX:  lcd_writeValue(MaxVoltage); break;
        case OPTION_UNITTYPE_VOLTAGE_MIN:  lcd_writeValue(MinVoltage); break;
        case OPTION_UNITTYPE_POWER_AVG:    lcd_writeValue(AvgPower); break;
        case OPTION_UNITTYPE_POWER_MAX:    lcd_writeValue(MaxPower); break;
        case OPTION_UNITTYPE_POWER_MIN:    lcd_writeValue(MinPower); break;
        case OPTION_UNITTYPE_CAPACITY_TOT: lcd_writeMilliValue((uint16_t)(TotCapacity / 3600)); break;
        case OPTION_UNITTYPE_CAPACITY_NA1: break;
        case OPTION_UNITTYPE_CAPACITY_NA2: break;
    }
}
