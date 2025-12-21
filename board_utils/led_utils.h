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

void pattern_slow_blinking(void);
void pattern_rapid_blinking(void);
void pattern_off(void);
void pattern_on(void);
void show_rgb_color(COLOR_RGB color);
void show_color(COLOR_DESCRIPTION color);
void init_leds_init(void);
void init_pwm_leds(void);

#endif