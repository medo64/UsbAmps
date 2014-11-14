#include <pic.h>
#include <stdbool.h>
#include "option.h"
#include "settings.h"


//LCDDATA0:  C2  A1 DP1  E2  F1  G1   -  C3
//LCDDATA1:  D2 DP2  D3  E3  G2  A2  F2  G3
//LCDDATA2:  E1  D1  C1  B3  A3  F3  B2  B1

const uint8_t DIGIT_SEGMENTS[]         = {
                                           0b01001000, 0b00000000, 0b11100001, //D1: 0 [ABCDEF ]
                                           0b00000000, 0b00000000, 0b00100001, //D1: 1 [ BC    ]
                                           0b01000100, 0b00000000, 0b11000001, //D1: 2 [AB DE G]
                                           0b01000100, 0b00000000, 0b01100001, //D1: 3 [ABCD  G]
                                           0b00001100, 0b00000000, 0b00100001, //D1: 4 [ BC  FG]
                                           0b01001100, 0b00000000, 0b01100000, //D1: 5 [A CD FG]
                                           0b01001100, 0b00000000, 0b11100000, //D1: 6 [A CDEFG]
                                           0b01000000, 0b00000000, 0b00100001, //D1: 7 [ABC    ]
                                           0b01001100, 0b00000000, 0b11100001, //D1: 8 [ABCDEFG]
                                           0b01001100, 0b00000000, 0b01100001, //D1: 9 [ABCD FG]

                                           0b10010000, 0b10000110, 0b00000010, //D2: 0 [ABCDEF ]
                                           0b10000000, 0b00000000, 0b00000010, //D2: 1 [ BC    ]
                                           0b00010000, 0b10001100, 0b00000010, //D2: 2 [AB DE G]
                                           0b10000000, 0b10001100, 0b00000010, //D2: 3 [ABCD  G]
                                           0b10000000, 0b00001010, 0b00000010, //D2: 4 [ BC  FG]
                                           0b10000000, 0b10001110, 0b00000000, //D2: 5 [A CD FG]
                                           0b10010000, 0b10001110, 0b00000000, //D2: 6 [A CDEFG]
                                           0b10000000, 0b00000100, 0b00000010, //D2: 7 [ABC    ]
                                           0b10010000, 0b10001110, 0b00000010, //D2: 8 [ABCDEFG]
                                           0b10000000, 0b10001110, 0b00000010, //D2: 9 [ABCD FG]

                                           0b00000001, 0b00110000, 0b00011100, //D3: 0 [ABCDEF ]
                                           0b00000001, 0b00000000, 0b00010000, //D3: 1 [ BC    ]
                                           0b00000000, 0b00110001, 0b00011000, //D3: 2 [AB DE G]
                                           0b00000001, 0b00100001, 0b00011000, //D3: 3 [ABCD  G]
                                           0b00000001, 0b00000001, 0b00010100, //D3: 4 [ BC  FG]
                                           0b00000001, 0b00100001, 0b00001100, //D3: 5 [A CD FG]
                                           0b00000001, 0b00110001, 0b00001100, //D3: 6 [A CDEFG]
                                           0b00000001, 0b00000000, 0b00011000, //D3: 7 [ABC    ]
                                           0b00000001, 0b00110001, 0b00011100, //D3: 8 [ABCDEFG]
                                           0b00000001, 0b00100001, 0b00011100  //D3: 9 [ABCD FG]
                                         };
const uint8_t DECIMAL_POINT_SEGMENTS[] = { 0b00100000, 0b00000000, 0b00000000 };

const uint8_t UNITTYPE_SEGMENTS[]      = {
                                           0b00010000, 0b00000010, 0b00000000, //·I· [       ][    EF ][       ]
                                           0b00000000, 0b00010000, 0b00000100, //··I [       ][       ][    EF ]
                                           0b00001000, 0b00000000, 0b10000000, //I·· [    EF ][       ][       ]
                                           0b10010000, 0b10000010, 0b00000010, //·U· [       ][ BCDEF ][       ]
                                           0b00000001, 0b00110000, 0b00010100, //··U [       ][       ][ BCDEF ]
                                           0b00001000, 0b00000000, 0b11100001, //U·· [ BCDEF ][       ][       ]
                                           0b00010000, 0b00001110, 0b00000010, //·P· [       ][AB  EFG][       ]
                                           0b00000000, 0b00010001, 0b00011100, //··P [       ][       ][AB  EFG]
                                           0b01001100, 0b00000000, 0b10000001, //P·· [AB  EFG][       ][       ]
                                           0b00010000, 0b10000110, 0b00000000, //·C· [       ][A  DEF ][       ]
                                           0b00000000, 0b00110000, 0b00001100, //··C [       ][       ][A  DEF ]
//                                           0b00000000, 0b00000000, 0b00000000, //··· [       ][       ][       ]
                                         };

const uint8_t CALIBRATION_SEGMENTS[]   = { 0b11011010, 0b00111110, 0b11000110 }; //CAL [A  DEF ][ABC EFG][   DEF ]
const uint8_t ERROR_SEGMENTS[]         = { 0b01011100, 0b00011001, 0b11000000 }; //Err [D  DEFG][    E G][    E G]
const uint8_t LOADING_SEGMENTS[]       = { 0b00000000, 0b00001000, 0b00000000 }; //·-· [       ][      G][       ]
const uint8_t HIGHPOWER_L_SEGMENTS[]   = { 0b00011100, 0b00010001, 0b10111100 }; //hiP [  C EFG][    E  ][AB  EFG]
const uint8_t HIGHPOWER_U_SEGMENTS[]   = { 0b00011100, 0b00010011, 0b10111101 }; //HIP [ BC EFG][    EF ][AB  EFG]
const uint8_t STATSRESET_SEGMENTS[]    = { 0b00000100, 0b00000001, 0b00000000 }; //-·- [      G][       ][      G]
const uint8_t OVERLOAD_SEGMENTS[]      = { 0b01011000, 0b10000010, 0b11100001 }; //OL· [ABCDEF ][   DEF ][       ]
const uint8_t NOVALUE_SEGMENTS[]       = { 0b00000000, 0b00000001, 0b00000000 }; //··- [       ][       ][      G]


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


void writeDigits(uint8_t digit1, uint8_t digit2, uint8_t digit3, bool includeDecimalPoint) {
    uint8_t segments0 = 0;
    uint8_t segments1 = 0;
    uint8_t segments2 = 0;

    if (digit1 < 10) {
        uint8_t index1 = 0 + digit1 * 3;
        segments0 = segments0 | DIGIT_SEGMENTS[index1 + 0];
        segments1 = segments1 | DIGIT_SEGMENTS[index1 + 1];
        segments2 = segments2 | DIGIT_SEGMENTS[index1 + 2];
    }

    if (digit2 < 10) {
        uint8_t index2 = 30 + digit2 * 3;
        segments0 = segments0 | DIGIT_SEGMENTS[index2 + 0];
        segments1 = segments1 | DIGIT_SEGMENTS[index2 + 1];
        segments2 = segments2 | DIGIT_SEGMENTS[index2 + 2];
    }

    if (digit3 < 10) {
        uint8_t index3 = 60 + digit3 * 3;
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

void lcd_writeHighPowerLower() {
    LCDDATA0 = HIGHPOWER_L_SEGMENTS[0];
    LCDDATA1 = HIGHPOWER_L_SEGMENTS[1];
    LCDDATA2 = HIGHPOWER_L_SEGMENTS[2];
}

void lcd_writeHighPowerUpper() {
    LCDDATA0 = HIGHPOWER_U_SEGMENTS[0];
    LCDDATA1 = HIGHPOWER_U_SEGMENTS[1];
    LCDDATA2 = HIGHPOWER_U_SEGMENTS[2];
}

void lcd_writeStatsReset() {
    LCDDATA0 = STATSRESET_SEGMENTS[0];
    LCDDATA1 = STATSRESET_SEGMENTS[1];
    LCDDATA2 = STATSRESET_SEGMENTS[2];
}

void lcd_writeAll() {
    LCDDATA0 = 0xFF;
    LCDDATA1 = 0xFF;
    LCDDATA2 = 0xFF;
}

void lcd_writeNoValue() {
    LCDDATA0 = NOVALUE_SEGMENTS[0];
    LCDDATA1 = NOVALUE_SEGMENTS[1];
    LCDDATA2 = NOVALUE_SEGMENTS[2];
}


void lcd_writeNumber(uint16_t value_1m, bool noMillies) {
    if (value_1m >= 10000) {
        LCDDATA0 = OVERLOAD_SEGMENTS[0];
        LCDDATA1 = OVERLOAD_SEGMENTS[1];
        LCDDATA2 = OVERLOAD_SEGMENTS[2];
    } else {
        uint8_t d0 = (uint8_t)((value_1m / 1000) % 10);
        uint8_t d1 = (uint8_t)((value_1m / 100) % 10);
        uint8_t d2 = (uint8_t)((value_1m / 10) % 10);
        uint8_t d3 = (uint8_t)(value_1m % 10);
        if ((value_1m >= 1000) || noMillies) { //100-9999 mX => 4.20
            writeDigits(d0, d1, d2, 1);
        } else if (value_1m >= 100) { //100-9999 mX => 4.20
            if (OPTION_THREE_DIGIT_MILLIAMPS) {
                writeDigits(d1, d2, d3, 0);
            } else {
                writeDigits(d0, d1, d2, 1);
            }
        } else if (value_1m >= 10) { //10-99 mX => 42
            writeDigits(UINT8_MAX, d2, d3, 0);
        } else if (value_1m >= 1) { //1-9 mX => 4
            writeDigits(UINT8_MAX, UINT8_MAX, d3, 0);
        } else {
            writeDigits(UINT8_MAX, UINT8_MAX, 0, 0);
        }
    }
}

void lcd_writeValue(uint16_t value_1m) {
    lcd_writeNumber(value_1m, 1);
}

void lcd_writeMilliValue(uint16_t value_1m) {
    lcd_writeNumber(value_1m, 0);
}

void lcd_writeUnitAndType(uint8_t unitIndex, uint8_t typeIndex) {
    uint8_t index = (unitIndex * 3 + typeIndex) * 3;
    LCDDATA0 = UNITTYPE_SEGMENTS[index + 0];
    LCDDATA1 = UNITTYPE_SEGMENTS[index + 1];
    LCDDATA2 = UNITTYPE_SEGMENTS[index + 2];
}
