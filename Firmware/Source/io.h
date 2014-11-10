#ifndef IO_H
#define IO_H

#define IO_DSHORT_TRIS     TRISC1
#define IO_DSHORT_LAT      LATC1
#define IO_BACKLIGHT_TRIS  TRISC0


void io_init(void);

#define io_dshort_on()         { IO_DSHORT_LAT = 1; }
#define io_dshort_off()        { IO_DSHORT_LAT = 0; }

#define io_backlight_on()      { IO_BACKLIGHT_TRIS = 0; }
#define io_backlight_off()     { IO_BACKLIGHT_TRIS = 1; }
#define io_backlight_toggle()  { IO_BACKLIGHT_TRIS = !IO_BACKLIGHT_TRIS; }

#endif
