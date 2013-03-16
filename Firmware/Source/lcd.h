#ifndef LCD_H
#define	LCD_H

void lcd_init();
void lcd_writeNumber(unsigned long value_10u);
void lcd_writeUnitAndType(unsigned char unitIndex, unsigned char typeIndex);

#endif
