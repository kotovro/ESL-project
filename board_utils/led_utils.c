#include "led_utils.h"
#include "nrfx_pwm.h"

#define FADE_STEPS 10

extern volatile bool sleep;
extern volatile bool picking_h;
extern volatile bool picking_s;
extern volatile bool picking_v;

nrf_pwm_values_individual_t fade_seq[FADE_STEPS * 2];
static void prepare_fade_sequence(void)
{
    fade_seq[0].channel_0 = 0;
    fade_seq[1].channel_0 = 50;
    fade_seq[2].channel_0 = 150;
    fade_seq[3].channel_0 = 250;
    fade_seq[4].channel_0 = 1000;
    fade_seq[5].channel_0 = 250;
    fade_seq[6].channel_0 = 150;
    fade_seq[7].channel_0 = 50;
    fade_seq[8].channel_0 = 0;
    
    // fade_seq[0].channel_1 = 0;
    // fade_seq[0].channel_2 = 250;
    // fade_seq[0].channel_3 = 0;

    // fade_seq[1].channel_0 = 1000;
    // fade_seq[1].channel_1 = 500;
    // fade_seq[1].channel_2 = 250;
    // fade_seq[1].channel_3 = 0;
}
static const uint32_t m_led_pins[LED_COUNT] = {
    NRF_GPIO_PIN_MAP(0,6),
    NRF_GPIO_PIN_MAP(0,8),
    NRF_GPIO_PIN_MAP(1,9),
    NRF_GPIO_PIN_MAP(0,12) 
};

static nrfx_pwm_t m_pwm = NRFX_PWM_INSTANCE(0);
nrf_pwm_sequence_t seq =
{
        .values.p_individual = fade_seq,
        .length  = 64,
        .repeats = 1000,
        .end_delay = 0
};

nrfx_pwm_config_t config =
{
    .output_pins =
    {
        m_led_pins[0] | NRFX_PWM_PIN_INVERTED,
        m_led_pins[1] | NRFX_PWM_PIN_INVERTED,
        m_led_pins[2] | NRFX_PWM_PIN_INVERTED,
        m_led_pins[3] | NRFX_PWM_PIN_INVERTED
    },
    .irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
    .base_clock   = NRF_PWM_CLK_1MHz,
    .count_mode   = NRF_PWM_MODE_UP,
    .top_value    = 1000,             
    .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
    .step_mode    = NRF_PWM_STEP_AUTO
};



#if defined(BOARD_PCA10059)
/** 
 * This part was taken form boards file
 * Function for configuring UICR_REGOUT0 register
 * to set GPIO output voltage to 3.0V.
 */
static void gpio_output_voltage_setup(void)
{
    // Configure UICR_REGOUT0 register only if it is set to default value.
    if ((NRF_UICR->REGOUT0 & UICR_REGOUT0_VOUT_Msk) ==
        (UICR_REGOUT0_VOUT_DEFAULT << UICR_REGOUT0_VOUT_Pos))
    {
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

        NRF_UICR->REGOUT0 = (NRF_UICR->REGOUT0 & ~((uint32_t)UICR_REGOUT0_VOUT_Msk)) |
                            (UICR_REGOUT0_VOUT_3V0 << UICR_REGOUT0_VOUT_Pos);

        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

        // System reset is needed to update UICR registers.
        NVIC_SystemReset();
    }
}
#endif

void init_leds_init(void)
{
    #if defined(BOARD_PCA10059)
    // If nRF52 USB Dongle is powered from USB (high voltage mode),
    // GPIO output voltage is set to 1.8 V by default, which is not
    // enough to turn on green and blue LEDs. Therefore, GPIO voltage
    // needs to be increased to 3.0 V by configuring the UICR register.
    if (NRF_POWER->MAINREGSTATUS &
       (POWER_MAINREGSTATUS_MAINREGSTATUS_High << POWER_MAINREGSTATUS_MAINREGSTATUS_Pos))
    {
        gpio_output_voltage_setup();
    }
    #endif
}

void init_pwm_leds(void)
{
    prepare_fade_sequence();
    nrfx_pwm_init(&m_pwm, &config, NULL);
}

void pwm_update(void)
{
    if (sleep) {
      nrfx_pwm_stop(&m_pwm, true);  
    } 
    else {
        nrfx_pwm_simple_playback(&m_pwm, &seq, 1, NRFX_PWM_FLAG_LOOP);
    }
    // else if (picking_s) {
    //     fade_seq[0].channel_0 = 0;
    //     fade_seq[1].channel_0 = 50;
    //     fade_seq[3].channel_0 = 150;
    //     fade_seq[4].channel_0 = 1000;
    //     fade_seq[5].channel_0 = 150;
    //     fade_seq[7].channel_0 = 50;
    //     fade_seq[8].channel_0 = 0;
    //     nrfx_pwm_simple_playback(&m_pwm, &seq, 1, NRFX_PWM_FLAG_LOOP);
    // }
    // else if (picking_v) {
    //     fade_seq[0].channel_0 = 0;
    //     fade_seq[4].channel_0 = 1000;
    //     fade_seq[8].channel_0 = 0;
    //     nrfx_pwm_simple_playback(&m_pwm, &seq, 1, NRFX_PWM_FLAG_LOOP);
    // }
    
}
