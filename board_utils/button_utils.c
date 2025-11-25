#include "nrfx_gpiote.h"
#include "app_timer.h"
#include "nrfx_clock.h"
#include "button_utils.h"

#define BUTTON_PIN        38
#define BUTTON_PRESSED    0
#define DEBOUNCE_TIME_MS   50
#define DOUBLE_CLICK_MS   250

int click_counter = 0; 
volatile bool is_button_pressed = false;
volatile bool is_debouncing = false;
volatile bool freeze_pwm = false;


APP_TIMER_DEF(debounce_timer_id);
APP_TIMER_DEF(double_click_timer_id);

static void double_click_timer_handler()
{
    bool level = nrf_gpio_pin_read(BUTTON_PIN);

    if (click_counter > 1 && level == BUTTON_PRESSED)
    {
        if (!is_button_pressed)
        {
            is_button_pressed = true;
            freeze_pwm = false;
        }
        else
        {   
            freeze_pwm = !freeze_pwm;
        }
    }
    click_counter = 0;
}

static void debounce_timer_handler(void * p_context)
{
    is_debouncing = false;
    uint32_t ticks = APP_TIMER_TICKS(DOUBLE_CLICK_MS);
    app_timer_start(double_click_timer_id, ticks, NULL);   
}

void timers_init(void)
{
    app_timer_init();
    ret_code_t err_code;
    err_code = nrfx_clock_init(NULL);
    nrfx_clock_lfclk_start();
    APP_ERROR_CHECK(err_code);

    
    err_code = app_timer_create(&debounce_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                debounce_timer_handler);
    APP_ERROR_CHECK(err_code);

    
    err_code = app_timer_create(&double_click_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                double_click_timer_handler);
    APP_ERROR_CHECK(err_code);

}

static void button_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    bool level = nrf_gpio_pin_read(BUTTON_PIN);
    if (level != BUTTON_PRESSED)
    {
        
        is_button_pressed = false;
        click_counter = 0;
        is_debouncing = false;
        app_timer_stop(debounce_timer_id);
        app_timer_stop(double_click_timer_id);
    }

    if (!is_debouncing)
    {
        uint32_t ticks = APP_TIMER_TICKS(DEBOUNCE_TIME_MS);
        app_timer_start(debounce_timer_id, ticks, NULL);
        click_counter++;
        is_debouncing = true;
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
