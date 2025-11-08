#include "button_utils.h"


#define BUTTON_PIN 38
#define BUTTON_PRESSED 0

static uint32_t press_time = 0;
static uint32_t last_state = 1;

void init_button(void)
{
    nrf_gpio_cfg_input(BUTTON_PIN, NRF_GPIO_PIN_PULLUP);
}

bool is_button_pressed(void)
{
    return nrf_gpio_pin_read(BUTTON_PIN) == 0;
}

int get_button_press_time_ms(void)
{
    uint32_t curr_state = nrf_gpio_pin_read(BUTTON_PIN);
    int result = 0;

    if (last_state != BUTTON_PRESSED && curr_state == BUTTON_PRESSED)
    {
        press_time = app_timer_cnt_get();//app_timer_cnt_get();
    }

    if (last_state == BUTTON_PRESSED && curr_state != BUTTON_PRESSED)
    {
        uint32_t release_time = app_timer_cnt_get();//20;  
        result = app_timer_cnt_diff_compute(release_time, press_time) * 10000 / APP_TIMER_CLOCK_FREQ; //release_time - press_time;
        return result;
    }

    last_state = curr_state;
    return result;
}
