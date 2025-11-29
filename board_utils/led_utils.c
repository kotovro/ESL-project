#include "led_utils.h"
#include "nrfx_pwm.h"

const int top_value = 1024; // PWM top value for 1 kHz frequency with 1 MHz base clock
#define FADE_STEPS  32

static const uint32_t m_led_pins[LED_COUNT] = {
    NRF_GPIO_PIN_MAP(0,6),
    NRF_GPIO_PIN_MAP(0,8),
    NRF_GPIO_PIN_MAP(1,9),
    NRF_GPIO_PIN_MAP(0,12) 
};

nrfx_pwm_config_t config_pwm0 =
{
    .output_pins =
    {
        m_led_pins[0] | NRFX_PWM_PIN_INVERTED,
        NRFX_PWM_PIN_NOT_USED,
        NRFX_PWM_PIN_NOT_USED,
        NRFX_PWM_PIN_NOT_USED
        // m_led_pins[1] | NRFX_PWM_PIN_INVERTED,
        // m_led_pins[2] | NRFX_PWM_PIN_INVERTED,
        // m_led_pins[3] | NRFX_PWM_PIN_INVERTED
    },
    .irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
    .base_clock   = NRF_PWM_CLK_1MHz,
    .count_mode   = NRF_PWM_MODE_UP,
    .top_value    = top_value,             
    .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
    .step_mode    = NRF_PWM_STEP_AUTO
};


nrfx_pwm_config_t config_pwm1 =
{
    .output_pins =
    {
        NRFX_PWM_PIN_NOT_USED,
        // m_led_pins[0] | NRFX_PWM_PIN_INVERTED,
        m_led_pins[1] | NRFX_PWM_PIN_INVERTED,
        m_led_pins[2] | NRFX_PWM_PIN_INVERTED,
        NRFX_PWM_PIN_NOT_USED
        // m_led_pins[2] | NRFX_PWM_PIN_INVERTED,
        // m_led_pins[3] | NRFX_PWM_PIN_INVERTED
    },
    .irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
    .base_clock   = NRF_PWM_CLK_1MHz,
    .count_mode   = NRF_PWM_MODE_UP,
    .top_value    = top_value,             
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

static nrfx_pwm_t m_pwn_yellow_led = NRFX_PWM_INSTANCE(0);
static nrfx_pwm_t m_pwm_oher_led = NRFX_PWM_INSTANCE(1);


nrf_pwm_values_individual_t seq_buffer[FADE_STEPS];
nrf_pwm_values_individual_t seq_buffer_saturatuion[FADE_STEPS / 16];
nrf_pwm_values_individual_t seq_buffer_another_led[FADE_STEPS / 16];

nrf_pwm_sequence_t seq = {
    .values.p_individual = seq_buffer,
    .length = 4 * FADE_STEPS,
    .repeats = 50,
    .end_delay = 0
};

nrf_pwm_sequence_t seq_saturation = {
    .values.p_individual = seq_buffer_saturatuion,
    .length = 8,
    .repeats = 50,
    .end_delay = 0
};

nrf_pwm_sequence_t seq_another_led = {
    .values.p_individual = seq_buffer_another_led,
    .length = 8,
    .repeats = 50,
    .end_delay = 0
};

// Sleep (LED OFF)
void pattern_sleep(void) {
    for (int i = 0; i < FADE_STEPS; i++)
        seq_buffer[i].channel_0 = 0;
}

// Hue fade (example: fade up and down)
void pattern_hue(void) {
    for (int i = 0; i < FADE_STEPS; i++)
        seq_buffer[i].channel_0 =
        (i <= FADE_STEPS / 2)
        ? (i * top_value) / (FADE_STEPS / 2)
        : ((FADE_STEPS - i) * top_value) / (FADE_STEPS / 2);
    
    seq_buffer_another_led[0].channel_1 = 0;
    seq_buffer_another_led[0].channel_2 = 0;
    seq_buffer_another_led[1].channel_1 = top_value;
    seq_buffer_another_led[1].channel_2 = top_value;
    nrfx_pwm_simple_playback(&m_pwn_yellow_led, &seq, 1, NRFX_PWM_FLAG_LOOP);
    nrfx_pwm_simple_playback(&m_pwm_oher_led, &seq_another_led, 1, NRFX_PWM_FLAG_LOOP);

}

// Saturation example (faster blink)
void pattern_saturation(void) {
        for (int i = 0; i < FADE_STEPS; i++)
        seq_buffer[i].channel_1 =
        (i <= FADE_STEPS / 2)
        ? (i * top_value) / (FADE_STEPS / 2)
        : ((FADE_STEPS - i) * top_value) / (FADE_STEPS / 2);
    
    seq_buffer_saturatuion[0].channel_0 = 0;
    seq_buffer_saturatuion[1].channel_0 = top_value;

    nrfx_pwm_simple_playback(&m_pwn_yellow_led, &seq_saturation, 1, NRFX_PWM_FLAG_LOOP);
    nrfx_pwm_simple_playback(&m_pwm_oher_led, &seq, 1, NRFX_PWM_FLAG_LOOP);
}

// Value example (solid ON)
void pattern_value(void) {
    for (int i = 0; i < FADE_STEPS; i++)
        seq_buffer[i].channel_0 = top_value;

    seq_buffer_saturatuion[0].channel_0 = 0;
    seq_buffer_saturatuion[1].channel_0 = top_value;
    
    nrfx_pwm_simple_playback(&m_pwn_yellow_led, &seq, 1, NRFX_PWM_FLAG_LOOP);
    nrfx_pwm_simple_playback(&m_pwm_oher_led, &seq_saturation, 1, NRFX_PWM_FLAG_LOOP);
}


// -------------------- Init PWM --------------------
void init_pwm_leds(void) {
    nrfx_pwm_init(&m_pwn_yellow_led, &config_pwm0, NULL);    
    nrfx_pwm_init(&m_pwm_oher_led, &config_pwm1, NULL);
}