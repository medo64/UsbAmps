#ifndef TOUCH_H
#define TOUCH_H

#include <stdbool.h>

void touch_init();

bool touch_outer_pressed();
bool touch_inner_pressed();

#endif
