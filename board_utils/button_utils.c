#include "nrfx_gpiote.h"
#include "app_timer.h"
#include "nrfx_clock.h"
#include "button_utils.h"

#define BUTTON_PIN        38
#define BUTTON_PRESSED    0

volatile bool is_button_pressed = false;
volatile bool is_timer_started = false;

APP_TIMER_DEF(button_timer_id);

static void button_timer_handler(void * p_context)
{
     is_timer_started = false;
    bool level = nrf_gpio_pin_read(BUTTON_PIN);
    is_button_pressed = (level == 0);
}

void timers_init(void)
{
    app_timer_init();
    ret_code_t err_code;
    err_code = nrfx_clock_init(NULL);
    APP_ERROR_CHECK(err_code);

    nrfx_clock_lfclk_start();
    
    err_code = app_timer_create(&button_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                button_timer_handler);
    APP_ERROR_CHECK(err_code);
}

static void button_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    is_button_pressed = true;
    if (!is_timer_started)
    {
        uint32_t ticks = APP_TIMER_TICKS(50);
        app_timer_start(button_timer_id, ticks, NULL);
        is_timer_started = true;
    }
}

void init_button(void)
{
    ret_code_t err_code;

    if (!nrfx_gpiote_is_init()) {
        err_code = nrfx_gpiote_init();
        APP_ERROR_CHECK(err_code);
    }

    nrfx_gpiote_in_config_t button_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    button_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrfx_gpiote_in_init(BUTTON_PIN, &button_config, button_handler);
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_in_event_enable(BUTTON_PIN, true);
}
