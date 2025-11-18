#ifndef BUTTON_UTILS
#define BUTTON_UTILS

#include <stdbool.h> 
#include <nrf_gpio.h> 


void init_button(void);
bool is_button_pressed(void);
int get_button_press_time_ms(void);

#endif