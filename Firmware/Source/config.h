#ifndef CONFIG_H
#define CONFIG_H

#define _XTAL_FREQ 16000000

void init();
void clrwdt();
void wait_10ms();
void wait_250ms();

#endif
