#ifndef LCD_H
#define	LCD_H

void lcd_init();
void lcd_clear();

void lcd_writeCalibration();
void lcd_writeError();
void lcd_writeLoading();
void lcd_writeHighPowerLower();
void lcd_writeHighPowerUpper();
void lcd_writeStatsReset();
void lcd_writeAll();
void lcd_writeNoValue();

void lcd_writeValue(uint16_t value_1m);
void lcd_writeMilliValue(uint16_t value_1m);
void lcd_writeUnitAndType(uint8_t unitIndex, uint8_t typeIndex);

#endif
