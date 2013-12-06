#ifndef SETTINGS_H
#define SETTINGS_H


#define SETTINGS_AVERAGE_COUNT  60
/* Defines amount of smoothing for readings. Higher value will smooth readings
 * and make them more stable but at the cost of speed. Default value is 60. */

#define SETTINGS_THREE_DIGIT_MILLIAMPS  false
/* Defines whether values between 100 and 999 milliamps will be shown with two
 * digits (e.g. 0.54 when setting is false) or with three (542 when setting is
 * true). Default value is false. */


unsigned int settings_getAdcVoltageOffset();
void settings_setAdcVoltageOffset(unsigned int value);

unsigned int settings_getAdcCurrentOffset();
void settings_setAdcCurrentOffset(unsigned int value);

#endif
