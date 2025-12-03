#include "nrfx_gpiote.h"
#include "app_timer.h"
#include "nrfx_clock.h"
#include "button_utils.h"
#include "nrfx_pwm.h"
#include "commons.h"
#include "nvram_utils.h"

extern COLOR_HSV current_hsv;
extern volatile bool is_blinking;
int click_counter = 0; 
volatile int mode_global = SLEEP;
volatile bool is_button_pressed = false;
bool is_debouncing = false;

void pattern_off(void);
void pattern_slow_blinking(void);
void pattern_rapid_blinking(void);
void pattern_on(void);

APP_TIMER_DEF(debounce_timer_id);
APP_TIMER_DEF(double_click_timer_id);
APP_TIMER_DEF(color_change_timer_id);

extern nrf_pwm_values_individual_t led_seq[FADE_STEPS];

void change_hsv(int mode);

static void double_click_timer_handler()
{   

    if (click_counter > 1)
    {
        app_timer_stop(color_change_timer_id);
        if (mode_global == SLEEP)
        {
            mode_global = PICKING_HUE;
            pattern_slow_blinking();     
        }
        else if (mode_global == PICKING_HUE)
        {
            mode_global = PICKING_SATURATION;
            pattern_rapid_blinking();
        }
        else if (mode_global == PICKING_SATURATION)
        {
            mode_global = PICKING_VALUE;
            pattern_on();
        }
        else if (mode_global == PICKING_VALUE)
        {
            mode_global = SLEEP;
            pattern_off();
            nvram_save_settings(current_hsv);
        }
    }
    click_counter = 0;
}

static void debounce_timer_handler(void * p_context)
{
    is_button_pressed = nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED;
    is_debouncing = false;
    uint32_t ticks = APP_TIMER_TICKS(DOUBLE_CLICK_MS);
    app_timer_start(double_click_timer_id, ticks, NULL);   
}

static void color_change_timer_handler(void * p_context)
{
    if (mode_global == SLEEP) {
        return;
    }
    else if (nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED) {
        change_hsv(mode_global);
        app_timer_start(color_change_timer_id, COLOR_CHANGE_MS, NULL);
    }
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

    err_code = app_timer_create(&color_change_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                color_change_timer_handler);
    APP_ERROR_CHECK(err_code);

}

static void button_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    
    app_timer_start(color_change_timer_id, COLOR_CHANGE_DELAY, NULL);
    
    if (!is_debouncing && (nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED))
    {
        click_counter++;
        is_debouncing = true;
        uint32_t ticks = APP_TIMER_TICKS(DEBOUNCE_TIME_MS);
        app_timer_start(debounce_timer_id, ticks, NULL);
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
