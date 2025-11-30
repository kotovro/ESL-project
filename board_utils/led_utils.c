#include "led_utils.h"
#include "nrfx_pwm.h"
#include "constants.h"


const int top_value = 1024; // PWM top value for 1 kHz frequency with 1 MHz base clock


int hue_v = 22;
int saturation_v = 100;
int value_v = 100;

bool hue_d = DECREASE;
bool saturation_d = DECREASE;
bool value_d = DECREASE;

typedef struct {
    uint16_t r;
    uint16_t g;
    uint16_t b;
} color;

color hsv_to_rgb(int h, int s, int v) {
    float r, g, b;
    float hf = h / 60.0f;
    float sf = s / 100.0f;
    float vf = v / 100.0f;
    int i = (int)hf % 6;
    float f = hf - i;
    float p = vf * (1 - sf);
    float q = vf * (1 - f * sf);
    float t = vf * (1 - (1 - f) * sf);

    switch (i) {
        case 0: r = vf; g = t; b = p; break;
        case 1: r = q; g = vf; b = p; break;
        case 2: r = p; g = vf; b = t; break;
        case 3: r = p; g = q; b = vf; break;
        case 4: r = t; g = p; b = vf; break;
        case 5: r = vf; g = p; b = q; break;
        default: r = g = b = 0; break;
    }
    color result_color;
    result_color.r = (uint16_t)(r * top_value);
    result_color.g = (uint16_t)(g * top_value);
    result_color.b = (uint16_t)(b * top_value);
    return result_color;
}

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
        m_led_pins[1] | NRFX_PWM_PIN_INVERTED,
        m_led_pins[2] | NRFX_PWM_PIN_INVERTED,
        m_led_pins[3] | NRFX_PWM_PIN_INVERTED
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

static nrfx_pwm_t m_pwn_status_led = NRFX_PWM_INSTANCE(0);

nrf_pwm_values_individual_t led_seq[FADE_STEPS];

nrf_pwm_sequence_t seq_smooth = {
    .values.p_individual = led_seq,
    .length = 4 * FADE_STEPS,
    .repeats = 50,
    .end_delay = 0
};

void initial_color() {
    color c = hsv_to_rgb(hue_v, saturation_v, value_v);
    for (int i = 0; i < FADE_STEPS; i++) 
    {
        led_seq[i].channel_0 = 0;
        led_seq[i].channel_1 = c.r;
        led_seq[i].channel_2 = c.g;
        led_seq[i].channel_3 = c.b;
    }
}

void pattern_off(void) {
    for (int i = 0; i < FADE_STEPS; i++) {
        led_seq[i].channel_0 = 0;
    }
}

void pattern_slow_blinking(void) {
    for (int i = 0; i < FADE_STEPS; i++) {
        led_seq[i].channel_0 =
        (i <= FADE_STEPS / 2)
        ? (i * top_value) / (FADE_STEPS / 2)
        : ((FADE_STEPS - i) * top_value) / (FADE_STEPS / 2);
    }
}

void pattern_rapid_blinking(void) {
    for (int i = 0; i < FADE_STEPS; i++) {
        led_seq[i].channel_0 = i % 2 ? 0 : top_value;
    }      
}

void pattern_on(void) {
    for (int i = 0; i < FADE_STEPS; i++)
        led_seq[i].channel_0 = top_value;
}

void change_hsv(int mode) 
{
    if (mode == PICKING_HUE) {
        if (hue_d == INCREASE) {
            hue_v += 5;
            if (hue_v >= 360) {
                hue_v = 360;
                hue_d = DECREASE;
            }
        } else {
            hue_v -= 5;
            if (hue_v <= 0) {
                hue_v = 0;
                hue_d = INCREASE;
            }
        }
    } 
    else if (mode == PICKING_SATURATION) {
        if (saturation_d == INCREASE) {
            saturation_v += 5;
            if (saturation_v >= 100) {
                saturation_v = 100;
                saturation_d = DECREASE;
            }
        } else {
            saturation_v -= 5;
            if (saturation_v <= 0) {
                saturation_v = 0;
                saturation_d = INCREASE;
            }
        }
    } 
    else if (mode == PICKING_VALUE) {
        if (value_d == INCREASE) {
            value_v += 5;
            if (value_v >= 100) {
                value_v = 100;
                value_d = DECREASE;
            }
        } else {
            value_v -= 5;
            if (value_v <= 0) {
                value_v = 0;
                value_d = INCREASE;
            }
        }
    }

    color c = hsv_to_rgb(hue_v, saturation_v, value_v);
    for (int i = 0; i < FADE_STEPS; i++) 
    {
        led_seq[i].channel_1 = c.r;
        led_seq[i].channel_2 = c.g;
        led_seq[i].channel_3 = c.b;
    }
}


// -------------------- Init PWM --------------------
void init_pwm_leds(void) {
    nrfx_pwm_init(&m_pwn_status_led, &config_pwm0, NULL);
    initial_color();
    nrfx_pwm_simple_playback(&m_pwn_status_led, &seq_smooth, 1, NRFX_PWM_FLAG_LOOP);
}