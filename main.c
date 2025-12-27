/**
 * Copyright (c) 2014 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example
 * @brief Blinky Example Application main file.
 *
 * This file contains the source code for a sample application to blink LEDs.
 *
 */
 
#include "board_utils.h"
#include "unknown_command.h"
#include "set_rgb_command.h"
#include "set_hsv_command.h"
#include "help_command.h"
#include "add_rgb_command.h"
#include "add_hsv_command.h"
#include "apply_color_command.h"
#include "list_colors_command.h"
#include "save_color_palette_command.h"
#include "save_current_color_command.h"

volatile bool hue_d = DECREASE;
volatile bool saturation_d = DECREASE;
volatile bool value_d = DECREASE;
volatile int mode_global = SLEEP;

void MemManage_Handler(void)
{
        NRF_LOG_INFO("Memory Manage Fault");
        NRF_LOG_PROCESS(); 
}

void BusFault_Handler(void)
{
        NRF_LOG_INFO("Bus Fault");
        NRF_LOG_PROCESS();     
}

void logs_init()
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


void change_hsv() 
{
    
    if (current_color_description.colorType != 1) 
    {
        return; // Not in HSV mode
    }

    if (mode_global == PICKING_HUE) {
        if (hue_d == INCREASE) {
            current_color_description.first_component+= STEP_OF_COLOR_CHANGE;
            if (current_color_description.first_component>= 360) {
                current_color_description.first_component= 360;
                hue_d = DECREASE;
            }
        } else {
            current_color_description.first_component-= STEP_OF_COLOR_CHANGE;
            if ((int16_t)current_color_description.first_component<= 0) {
                current_color_description.first_component= 0;
                hue_d = INCREASE;
            }
        }
    } 
    else if (mode_global == PICKING_SATURATION) {
        if (saturation_d == INCREASE) {
            current_color_description.second_component += STEP_OF_COLOR_CHANGE;
            if (current_color_description.second_component >= 100) {
                current_color_description.second_component = 100;
                saturation_d = DECREASE;
            }
        } else {
            current_color_description.second_component -= STEP_OF_COLOR_CHANGE;
            if ((int8_t)current_color_description.second_component <= 0) {
                current_color_description.second_component = 0;
                saturation_d = INCREASE;
            }
        }
    } 
    else if (mode_global == PICKING_VALUE) {
        if (value_d == INCREASE) {
            current_color_description.third_component += STEP_OF_COLOR_CHANGE;
            if (current_color_description.third_component >= 100) {
                current_color_description.third_component = 100;
                value_d = DECREASE;
            }
        } else {
            current_color_description.third_component -= STEP_OF_COLOR_CHANGE;
            if ((int8_t)current_color_description.third_component <= 0) {
                current_color_description.third_component = 0;
                value_d = INCREASE;
            }
        }
    }
    show_color(current_color_description);
}


void double_click_executor()
{   
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
        save_current_color();
    }
}


void fill_command_definitions()
{
    init_command_definitions();
    command_definitions[0] = set_rgb_command;
    command_definitions[1] = set_hsv_command;
    command_definitions[2] = help_command;
    command_definitions[3] = add_rgb_command;
    command_definitions[4] = add_hsv_command;
    command_definitions[5] = apply_color_command;
    command_definitions[6] = list_colors_command;
    command_definitions[7] = save_current_color_command;
    command_definitions[8] = save_color_palette_command;
}

void main_loop(void)
{
    
    while (true)
    {
        LOG_BACKEND_USB_PROCESS();

        if (!NRF_LOG_PROCESS())
        {
        }
        __WFE();
        __SEV();
        __WFE();
    }
}

void read_current_color_from_nvm()
{   
    if (settings.saved_color.h > 360 ||
        settings.saved_color.s > 100 ||
        settings.saved_color.v > 100) 
        return;
    
    current_color_description.first_component = settings.saved_color.h;
    current_color_description.second_component = settings.saved_color.s;
    current_color_description.third_component = settings.saved_color.v;
}

void read_palette_from_nvm()
{
    for (int i = 0; i < AVAILABLE_COLOR_SLOTS; i++) 
    {
        colorPalette[i] = settings.colorPalette[i];
    }
}

int main(void)
{
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk
            | SCB_SHCSR_BUSFAULTENA_Msk
            | SCB_SHCSR_USGFAULTENA_Msk;
    init_color_palette();
    fill_command_definitions();
    logs_init();
    if (is_version_changed(CURRENT_VERSION))
    {
        update_version(CURRENT_VERSION);    
    } 
    else
    {
        nvram_load_settings((uint32_t*)&settings, sizeof(settings));
        read_current_color_from_nvm();
        read_palette_from_nvm();
    } 
    init_leds_init();
    init_pwm_leds();
    init_button(double_click_executor, change_hsv);
    init_usb_cli(command_definitions, sizeof(command_definitions) / sizeof(COMMAND_DEFINITION),
                unknown_command_executor);
    main_loop();
}
/**
 *@}
 **/
