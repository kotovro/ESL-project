#ifndef LED_UTILS
#define LED_UTILS
#include "nrf_delay.h"
#include "nrfx_gpiote.h"
#include "nrfx_pwm.h"

typedef enum {
    LED_YELLOW,
    LED_RED,
    LED_GREEN,
    LED_BLUE,
    LED_COUNT
} LedId;

typedef struct {
    uint16_t r;
    uint16_t g;
    uint16_t b;
} COLOR_RGB;

void show_rgb_color(COLOR_RGB color);
void init_leds_init(void);
void init_pwm_leds(void);

#endif