#ifndef LCD_H
#define	LCD_H

void lcd_init();
void lcd_clear();

void lcd_writeCalibration();
void lcd_writeError();
void lcd_writeLoading();
void lcd_writeHighPower();

void lcd_writeValue(unsigned int value_1m);
void lcd_writeMilliValue(unsigned int value_1m);
void lcd_writeUnitAndType(unsigned char unitIndex, unsigned char typeIndex);

#endif
