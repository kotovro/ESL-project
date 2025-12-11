#ifndef LED_UTILS
#define LED_UTILS
#include "nrf_delay.h"
#include "nrfx_gpiote.h"
#include "nrfx_pwm.h"
#include "commons.h" 

typedef enum {
    LED_YELLOW,
    LED_RED,
    LED_GREEN,
    LED_BLUE,
    LED_COUNT
} LedId;



void show_rgb_color(COLOR_RGB color);
void init_leds_init(void);
void init_pwm_leds(void);
COLOR_RGB hsv_to_rgb(COLOR_HSV hsv);

#endif