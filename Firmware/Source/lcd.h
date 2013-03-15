#ifndef LCD_H
#define	LCD_H

void lcd_init();
void lcd_writeNumber(double value);
void lcd_writeUnitAndType(unsigned char unitIndex, unsigned char typeIndex);

#endif
