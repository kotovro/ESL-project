#include "button_utils.h"


#define BUTTON_PIN 38
#define BUTTON_PRESSED 0


void init_button(void)
{
    nrf_gpio_cfg_input(BUTTON_PIN, NRF_GPIO_PIN_PULLUP);
}

bool is_button_pressed(void)
{
    return nrf_gpio_pin_read(BUTTON_PIN) == 0;
}
