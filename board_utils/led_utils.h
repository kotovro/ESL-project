#ifndef LED_UTILS
#define LED_UTILS
#include "nrf_delay.h"

void init_leds(void);
void blink_led(uint8_t led_idx, int times);

#endif