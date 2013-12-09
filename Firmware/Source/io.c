#include <pic.h>


void io_init() {
    TRISC1 = 0; //C1 is an output
}

void io_dshort_on() {
    LATC1 = 1;
}
