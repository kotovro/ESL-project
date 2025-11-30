#include "nrfx_gpiote.h"
#include "app_timer.h"
#include "nrfx_clock.h"
#include "button_utils.h"
#include "nrfx_pwm.h"

#define BUTTON_PIN        38
#define BUTTON_PRESSED    0
#define DEBOUNCE_TIME_MS   50
#define DOUBLE_CLICK_MS   400

#ifndef FADE_STEPS
#define FADE_STEPS  32
#endif

int click_counter = 0; 
volatile bool sleep = true;
volatile bool picking_h = false;
volatile bool picking_s = true;
volatile bool picking_v = false;
volatile bool is_debouncing = false;
volatile bool is_button_pressed = false;


void pattern_sleep(void);
void pattern_hue(void);
void pattern_saturation(void);
void pattern_value();

APP_TIMER_DEF(debounce_timer_id);
APP_TIMER_DEF(double_click_timer_id);
APP_TIMER_DEF(color_change_timer_id);

extern nrf_pwm_values_individual_t seq_smooth_blink[FADE_STEPS];

static void double_click_timer_handler()
{   

    if (click_counter > 1)
    {
        
        if (sleep)
        {
            sleep = false;
            picking_h = true;
            pattern_hue();     
        }
        // else if (picking_h)
        // {
        //     picking_h = false;
        //     sleep = true;
        // }
        else if (picking_h)
        {
            picking_h = false;
            picking_s = true;
            app_timer_start(color_change_timer_id, 5, NULL);
        
            pattern_saturation();
        }
        else if (picking_s)
        {
            picking_s = false;
            picking_v = true;
            pattern_value();
        }
        else if (picking_v)
        {
            picking_v = false;
            sleep = true;
            pattern_sleep();
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
    if (picking_s) {
    // for (int i = 0; i < FADE_STEPS; i++)
    //     seq_smooth_blink[i].channel_1 =
    //     (i <= FADE_STEPS / 2)
    //     ? (i * 1024) / (FADE_STEPS / 2)
        //     : ((FADE_STEPS - i) * 1024) / (FADE_STEPS / 2);
        // int top_value = 1024;
        // while (nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED)
        // {
        //     // First: increase or decrease current value
            
        //     for (int i = 0; i < FADE_STEPS; i++)
        //     {
        //         seq_smooth_blink[i].channel_1 =
        //         (i <= FADE_STEPS / 2)
        //         ? top_value
        //         : 0;
        //     }
        //     // int h = 10;
        //     // while(h < 100000000)
        //     // {
        //     //     h += 1;
        //     // }
        //     // top_value = (top_value < 1024) ? top_value + 1 : top_value - 1;
        // }
        // for (int i = 0; i < FADE_STEPS; i++)
        //     {
        //         seq_smooth_blink[i].channel_1 = 0;
        //     }
    }
    if (sleep) {
        // int cur_value = 1;
        while(nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED)
        {
            for (int i = 0; i < FADE_STEPS; i++)
            {
                seq_smooth_blink[i].channel_1 = 0;
            }
            // seq_smooth_blink[0].channel_1 = cur_value;
            // cur_value = (cur_value < 1024) ? cur_value + 1 : cur_value - 1;
            // int h = 10;
            // while(h < 100000000)
            // {
            //     h += 1;
            // }
            // top_value = (top_value < 1024) ? top_value + 1 : top_value - 1;
        }
    }
    // Second: replace fade values with just 0 or top_value
    
    // This function can be used to handle color change timing if needed
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
                                APP_TIMER_MODE_REPEATED,
                                color_change_timer_handler);
    APP_ERROR_CHECK(err_code);

}

static void button_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    
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
