#include <pic.h>
#include "io.h"


void io_init() {
    IO_DSHORT_TRIS = 0; //C1 is an output
    IO_BACKLIGHT_TRIS = 1;
}
