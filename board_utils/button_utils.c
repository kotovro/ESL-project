#include "nrfx_gpiote.h"
#include "button_utils.h"
#include "nrfx_pwm.h"
#include "commons.h"
#include "nvram_utils.h"

static int m_click_counter = 0; 
Double_Click_Executor m_double_click_action;
Button_Press_Executor m_press_action;

volatile bool m_is_button_pressed = false;
bool m_is_debouncing = false;


APP_TIMER_DEF(debounce_timer_id);
APP_TIMER_DEF(double_click_timer_id);
APP_TIMER_DEF(button_press_timer_id);

extern nrf_pwm_values_individual_t led_seq[FADE_STEPS];


static void double_click_timer_handler()
{   

    if (m_click_counter > 1)
    {
        NRF_LOG_INFO("We got double-click");
        app_timer_stop(button_press_timer_id);
        m_double_click_action();
    }
    m_click_counter = 0;
}

static void debounce_timer_handler(void * p_context)
{
    NRF_LOG_INFO("Debouncing timer fired");
    m_is_button_pressed = nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED;
    m_is_debouncing = false;
    uint32_t ticks = APP_TIMER_TICKS(DOUBLE_CLICK_MS);
    app_timer_start(double_click_timer_id, ticks, NULL);   
}

static void button_press_handler(void * p_context)
{
    if (nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED) {
        // change_hsv(mode_global);
        m_press_action();
        app_timer_start(button_press_timer_id, COLOR_CHANGE_MS, NULL); /// rename to check press ms
    }
    /// refaor so that inside we check if still pressed and mode is not sleep, then call press_executor again
    // if (mode_global == SLEEP) {
    //     return;
    // }
    // else if (nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED) {
    //     // change_hsv(mode_global);
    //     ///execute press_executor();
    //     app_timer_start(button_press_timer_id, COLOR_CHANGE_MS, NULL);
    // }
}   

void timers_init(void)
{
    app_timer_init();
    ret_code_t err_code;
    
    err_code = app_timer_create(&debounce_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                debounce_timer_handler);
    APP_ERROR_CHECK(err_code);

    
    err_code = app_timer_create(&double_click_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                double_click_timer_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&button_press_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                button_press_handler);
    APP_ERROR_CHECK(err_code);

}

static void button_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    
    app_timer_start(button_press_timer_id, COLOR_CHANGE_DELAY, NULL);
    
    if (!m_is_debouncing && (nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED))
    {
        m_click_counter++;
        m_is_debouncing = true;
        uint32_t ticks = APP_TIMER_TICKS(DEBOUNCE_TIME_MS);
        app_timer_start(debounce_timer_id, ticks, NULL);
    }
}

void init_button(Double_Click_Executor double_click_executor, Button_Press_Executor press_executor) /// pass there 2 different handlers: one for double click, one for press
{
    m_double_click_action = double_click_executor;
    m_press_action = press_executor;
    timers_init();

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
