#ifndef LED_UTILS
#define LED_UTILS
#include "nrf_delay.h"
#include "nrfx_gpiote.h"


typedef enum {
    LED_YELLOW,
    LED_RED,
    LED_GREEN,
    LED_BLUE,
    LED_COUNT
} LedId;


void init_leds_init(void);
void blink_led(uint8_t led_idx, int times);
void populate_blinking_sequnce(uint32_t* sequence);

#endif