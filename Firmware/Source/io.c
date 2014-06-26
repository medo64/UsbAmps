#include <pic.h>

#define DSHORT_TRIS     TRISC1
#define DSHORT_LAT      LATC1
#define BACKLIGHT_TRIS  TRISC0


void io_init() {
    DSHORT_TRIS = 0; //C1 is an output
    BACKLIGHT_TRIS = 1;
}

void io_dshort_on() {
    DSHORT_LAT = 1;
}


void io_backlight_on() {
    BACKLIGHT_TRIS = 0;
}

void io_backlight_off() {
    BACKLIGHT_TRIS = 1;
}

void io_backlight_toggle() {
    BACKLIGHT_TRIS = !BACKLIGHT_TRIS;
}
