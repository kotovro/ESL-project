#include "led_utils.h"
#include "nrfx_pwm.h"

#define FADE_STEPS 8


extern volatile bool sleep;
extern volatile bool picking_h;
extern volatile bool picking_s;
extern volatile bool picking_v;

static const uint32_t m_led_pins[LED_COUNT] = {
    NRF_GPIO_PIN_MAP(0,6),
    NRF_GPIO_PIN_MAP(0,8),
    NRF_GPIO_PIN_MAP(1,9),
    NRF_GPIO_PIN_MAP(0,12) 
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

// PWM instance and sequence buffer
static nrfx_pwm_t m_pwm = NRFX_PWM_INSTANCE(0);
static nrf_pwm_values_individual_t seq_buffer[4 * FADE_STEPS];

// PWM sequence structure
static nrf_pwm_sequence_t seq = {
    .values.p_individual = seq_buffer,
    .length = 4 * FADE_STEPS,
    .repeats = 500,
    .end_delay = 0
};


// Sleep (LED OFF)
void pattern_sleep(void) {
    for (int i = 0; i < FADE_STEPS; i++)
        seq_buffer[i].channel_0 = 0;
}

// Hue fade (example: fade up and down)
void pattern_hue(void) {
    seq_buffer[0].channel_0 = 0;
    seq_buffer[1].channel_0 = 50;
    seq_buffer[2].channel_0 = 150;
    seq_buffer[3].channel_0 = 250;
    seq_buffer[4].channel_0 = 1000;
    seq_buffer[5].channel_0 = 250;
    seq_buffer[6].channel_0 = 150;
    seq_buffer[7].channel_0 = 50;
    seq_buffer[8].channel_0 = 0;
}

// Saturation example (faster blink)
void pattern_saturation(void) {
    for (int i = 0; i < FADE_STEPS; i++)
        seq_buffer[i].channel_0 = (i % 2) ? 1000 : 0;
}

// Value example (solid ON)
void pattern_value(void) {
    for (int i = 0; i < FADE_STEPS; i++)
        seq_buffer[i].channel_0 = 1000;
}

// -------------------- Apply pattern --------------------
void apply_pattern(void (*pattern_func)(void)) {
    pattern_func(); // fill sequence buffer
    nrfx_pwm_simple_playback(&m_pwm, &seq, 1, NRFX_PWM_FLAG_LOOP);
}

// -------------------- Init PWM --------------------
void init_pwm_leds(void) {
    nrfx_pwm_init(&m_pwm, &config, NULL);
}

// -------------------- Update LED --------------------
void pwm_update(void) {
    if (sleep) {
        nrfx_pwm_stop(&m_pwm, true);
        return;
    }

    if (picking_h)      apply_pattern(pattern_hue);
    else if (picking_s) apply_pattern(pattern_saturation);
    else if (picking_v) apply_pattern(pattern_value);
}