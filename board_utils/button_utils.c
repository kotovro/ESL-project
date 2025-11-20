
#include <nrf_gpio.h> 
#include <nrfx_gpiote.h> 
#include "button_utils.h"


#define BUTTON_PIN 38
#define BUTTON_PRESSED 0

volatile bool is_button_pressed = false;

inline static void button_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (pin == BUTTON_PIN)
        is_button_pressed = !is_button_pressed;
}

void init_button(void)
{
    nrfx_gpiote_init();
    nrfx_gpiote_in_config_t button_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    button_config.pull = NRF_GPIO_PIN_PULLUP;

    nrfx_gpiote_in_init(BUTTON_PIN, &button_config, button_handler);

    nrfx_gpiote_in_event_enable(BUTTON_PIN, true);
    nrf_gpio_cfg_input(BUTTON_PIN, NRF_GPIO_PIN_PULLUP);
}

