#include <limits.h>
#include <pic.h>
#include <stdbool.h>
#include "settings.h"


//LCDDATA0:  C2  A1 DP1  E2  F1  G1   -  C3
//LCDDATA1:  D2   -  D3  E3  G2  A2  F2  G3
//LCDDATA2:  E1  D1  C1  B3  A3  F3  B2  B1

const unsigned char DIGIT_SEGMENTS[] = {
                                           0b01001000, 0b00000000, 0b11100001, //D1: 0 ABCDEF
                                           0b00000000, 0b00000000, 0b00100001, //D1: 1  BC
                                           0b01000100, 0b00000000, 0b11000001, //D1: 2 AB DE G
                                           0b01000100, 0b00000000, 0b01100001, //D1: 3 ABCD  G
                                           0b00001100, 0b00000000, 0b00100001, //D1: 4  BC  FG
                                           0b01001100, 0b00000000, 0b01100000, //D1: 5 A CD FG
                                           0b01001100, 0b00000000, 0b11100000, //D1: 6 A CDEFG
                                           0b01000000, 0b00000000, 0b00100001, //D1: 7 ABC
                                           0b01001100, 0b00000000, 0b11100001, //D1: 8 ABCDEFG
                                           0b01001100, 0b00000000, 0b01100001, //D1: 9 ABCD FG

                                           0b10010000, 0b10000110, 0b00000010, //D2: 0 ABCDEF
                                           0b10000000, 0b00000000, 0b00000010, //D2: 1  BC
                                           0b00010000, 0b10001100, 0b00000010, //D2: 2 AB DE G
                                           0b10000000, 0b10001100, 0b00000010, //D2: 3 ABCD  G
                                           0b10000000, 0b00001010, 0b00000010, //D2: 4  BC  FG
                                           0b10000000, 0b10001110, 0b00000000, //D2: 5 A CD FG
                                           0b10010000, 0b10001110, 0b00000000, //D2: 6 A CDEFG
                                           0b10000000, 0b00000100, 0b00000010, //D2: 7 ABC
                                           0b10010000, 0b10001110, 0b00000010, //D2: 8 ABCDEFG
                                           0b10000000, 0b10001110, 0b00000010, //D2: 9 ABCD FG

                                           0b00000001, 0b00110000, 0b00011100, //D3: 0 ABCDEF
                                           0b00000001, 0b00000000, 0b00010000, //D3: 1  BC
                                           0b00000000, 0b00110001, 0b00011000, //D3: 2 AB DE G
                                           0b00000001, 0b00100001, 0b00011000, //D3: 3 ABCD  G
                                           0b00000001, 0b00000001, 0b00010100, //D3: 4  BC  FG
                                           0b00000001, 0b00100001, 0b00001100, //D3: 5 A CD FG
                                           0b00000001, 0b00110001, 0b00001100, //D3: 6 A CDEFG
                                           0b00000001, 0b00000000, 0b00011000, //D3: 7 ABC
                                           0b00000001, 0b00110001, 0b00011100, //D3: 8 ABCDEFG
                                           0b00000001, 0b00100001, 0b00011100  //D3: 9 ABCD FG
                                       };
const unsigned char DECIMAL_POINT_SEGMENTS[] = { 0b00100000, 0b00000000, 0b00000000 };

//LCDDATA0:  C2  A1 DP1  E2  F1  G1   -  C3
//LCDDATA1:  D2   -  D3  E3  G2  A2  F2  G3
//LCDDATA2:  E1  D1  C1  B3  A3  F3  B2  B1
const unsigned char UNITTYPE_SEGMENTS[] = {
                                              0b00010000, 0b00000010, 0b00000000, //-I-:     EF
                                              0b00000000, 0b00010000, 0b00000100, //--I:     EF
                                              0b00001000, 0b00000000, 0b10000000, //I--:     EF
                                              0b10010000, 0b10000010, 0b00000010, //-U-:  BCDEF
                                              0b00000001, 0b00110000, 0b00010100, //--U:  BCDEF
                                              0b00001000, 0b00000000, 0b11100001, //U--:  BCDEF
                                              0b00010000, 0b00001110, 0b00000010, //-P-: AB  EFG
                                              0b00000000, 0b00010001, 0b00011100, //--P: AB  EFG
                                              0b01001100, 0b00000000, 0b10000001, //P--: AB  EFG
                                          };

const unsigned char CALIBRATION_SEGMENTS[] = { 0b11011010, 0b01111110, 0b11000110 };
const unsigned char ERROR_SEGMENTS[] = { 0b01011100, 0b01011001, 0b11000000 };
const unsigned char LOADING_SEGMENTS[] = { 0b00000000, 0b00001000, 0b00000000 };
const unsigned char OVERLOAD_SEGMENTS[] = { 0b01011000, 0b10000010, 0b11100001 };


void lcd_init() {
    WFT = 0; //Type-A phase changes within each common type
    BIASMD = 0; // Static Bias mode
    LCDA = 1; //LCD driver module is active
    WA = 1; //Write into the LCDDATAx registers is allowed
    LP3 = 0; //1:4
    LP2 = 0;
    LP1 = 1;
    LP0 = 1;

    LCDSE0 = 0b11111101; //SEG1 is not used
    LCDSE1 = 0b11111111;
    LCDSE2 = 0b11111111;

    LMUX1 = 0; //Common selection: Static bias
    LMUX0 = 0;
    CS1 = 1; //LFINTOSC
    CS0 = 0;
    SLPEN = 0; //LCD driver module is enabled in Sleep mode

    LCDDATA0 = 0x00; //set initial data
    LCDDATA1 = 0x00;
    LCDDATA2 = 0x00;

    LCDIF = 0;

    LRLAP1 = 1; //Internal LCD Reference Ladder is powered in high-power mode
    LRLAP0 = 1;
    LRLBP1 = 1; //Internal LCD Reference Ladder is powered in high-power mode
    LRLBP0 = 1;
    LRLAT2 = 0; //Internal LCD Reference Ladder is always in ?B? power mode
    LRLAT1 = 0;
    LRLAT0 = 0;
    LCDIRE = 1; //Internal LCD Reference is enabled and connected to the Internal Contrast Control circuit
    LCDIRS = 1; //Internal LCD Contrast Control is powered by a 3.072V output of the FVR.
    LCDIRI = 0; //The LCD Internal FVR Buffer ignores the LCD Reference Ladder Power mode.
    VLCD3PE = 0; //The VLCD3 pin is not connected
    VLCD2PE = 0; //The VLCD2 pin is not connected
    VLCD1PE = 0; //The VLCD1 pin is not connected

    LCDCST2 = 0; //Resistor ladder is at 3/7th of maximum resistance
    LCDCST1 = 1;
    LCDCST0 = 1;

    LCDEN = 1; //LCD driver module is enabled
}


void writeDigits(unsigned char digit1, unsigned char digit2, unsigned char digit3, unsigned char includeDecimalPoint) {
    unsigned char segments0 = 0;
    unsigned char segments1 = 0;
    unsigned char segments2 = 0;

    if (digit1 < 10) {
        unsigned char index1 = 0 + digit1 * 3;
        segments0 = segments0 | DIGIT_SEGMENTS[index1 + 0];
        segments1 = segments1 | DIGIT_SEGMENTS[index1 + 1];
        segments2 = segments2 | DIGIT_SEGMENTS[index1 + 2];
    }

    if (digit2 < 10) {
        unsigned char index2 = 30 + digit2 * 3;
        segments0 = segments0 | DIGIT_SEGMENTS[index2 + 0];
        segments1 = segments1 | DIGIT_SEGMENTS[index2 + 1];
        segments2 = segments2 | DIGIT_SEGMENTS[index2 + 2];
    }

    if (digit3 < 10) {
        unsigned char index3 = 60 + digit3 * 3;
        segments0 = segments0 | DIGIT_SEGMENTS[index3 + 0];
        segments1 = segments1 | DIGIT_SEGMENTS[index3 + 1];
        segments2 = segments2 | DIGIT_SEGMENTS[index3 + 2];
    }

    if (includeDecimalPoint) {
        segments0 = segments0 | DECIMAL_POINT_SEGMENTS[0];
        segments1 = segments1 | DECIMAL_POINT_SEGMENTS[1];
        segments2 = segments2 | DECIMAL_POINT_SEGMENTS[2];
    }

    LCDDATA0 = segments0;
    LCDDATA1 = segments1;
    LCDDATA2 = segments2;
}

void lcd_clear() {
    LCDDATA0 = 0;
    LCDDATA1 = 0;
    LCDDATA2 = 0;
}


void lcd_writeCalibration() {
    LCDDATA0 = CALIBRATION_SEGMENTS[0];
    LCDDATA1 = CALIBRATION_SEGMENTS[1];
    LCDDATA2 = CALIBRATION_SEGMENTS[2];
}

void lcd_writeError() {
    LCDDATA0 = ERROR_SEGMENTS[0];
    LCDDATA1 = ERROR_SEGMENTS[1];
    LCDDATA2 = ERROR_SEGMENTS[2];
}

void lcd_writeLoading() {
    LCDDATA0 = LOADING_SEGMENTS[0];
    LCDDATA1 = LOADING_SEGMENTS[1];
    LCDDATA2 = LOADING_SEGMENTS[2];
}


void lcd_writeNumber(unsigned int value_1m, unsigned char noMillies) {
    if (value_1m >= 10000) {
        LCDDATA0 = OVERLOAD_SEGMENTS[0];
        LCDDATA1 = OVERLOAD_SEGMENTS[1];
        LCDDATA2 = OVERLOAD_SEGMENTS[2];
    } else {
        unsigned char d0 = (unsigned char)((value_1m / 1000) % 10);
        unsigned char d1 = (unsigned char)((value_1m / 100) % 10);
        unsigned char d2 = (unsigned char)((value_1m / 10) % 10);
        unsigned char d3 = (unsigned char)(value_1m % 10);
        if ((value_1m >= 1000) || (noMillies != 0)) { //100-9999 mX => 4.20
            writeDigits(d0, d1, d2, 1);
        } else if (value_1m >= 100) { //100-9999 mX => 4.20
            if (SETTINGS_THREE_DIGIT_MILLIAMPS) {
                writeDigits(d1, d2, d3, 0);
            } else {
                writeDigits(d0, d1, d2, 1);
            }
        } else if (value_1m >= 10) { //10-99 mX => 42
            writeDigits(UCHAR_MAX, d2, d3, 0);
        } else if (value_1m >= 1) { //1-9 mX => 4
            writeDigits(UCHAR_MAX, UCHAR_MAX, d3, 0);
        } else {
            writeDigits(UCHAR_MAX, UCHAR_MAX, 0, 0);
        }
    }
}

void lcd_writeValue(unsigned int value_1m) {
    lcd_writeNumber(value_1m, 1);
}

void lcd_writeMilliValue(unsigned int value_1m) {
    lcd_writeNumber(value_1m, 0);
}

void lcd_writeUnitAndType(unsigned char unitIndex, unsigned char typeIndex) {
    unsigned char index = (unitIndex * 3 + typeIndex) * 3;
    LCDDATA0 = UNITTYPE_SEGMENTS[index + 0];
    LCDDATA1 = UNITTYPE_SEGMENTS[index + 1];
    LCDDATA2 = UNITTYPE_SEGMENTS[index + 2];
}
