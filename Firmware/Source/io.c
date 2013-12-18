#include <pic.h>

#define DSHORT_TRIS     TRISC1
#define DSHORT_LAT      LATC1


void io_init() {
    DSHORT_TRIS = 0; //C1 is an output
}

void io_dshort_on() {
    DSHORT_LAT = 1;
}
