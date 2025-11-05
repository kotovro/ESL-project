#ifndef BUTTON_UTILS
#define BUTTON_UTILS

#include "app_timer.h"
#include "nrf_drv_clock.h"

void init_button(void);
bool is_button_pressed(void);
int get_button_press_time_ms(void);

#endif