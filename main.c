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
COLOR_HSV current_hsv = {22, 100, 100};
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
    if (mode_global == PICKING_HUE) {
        if (hue_d == INCREASE) {
            current_hsv.h += STEP_OF_COLOR_CHANGE;
            if (current_hsv.h >= 360) {
                current_hsv.h = 360;
                hue_d = DECREASE;
            }
        } else {
            current_hsv.h -= STEP_OF_COLOR_CHANGE;
            if (current_hsv.h <= 0) {
                current_hsv.h = 0;
                hue_d = INCREASE;
            }
        }
    } 
    else if (mode_global == PICKING_SATURATION) {
        if (saturation_d == INCREASE) {
            current_hsv.s += STEP_OF_COLOR_CHANGE;
            if (current_hsv.s >= 100) {
                current_hsv.s = 100;
                saturation_d = DECREASE;
            }
        } else {
            current_hsv.s -= STEP_OF_COLOR_CHANGE;
            if (current_hsv.s <= 0) {
                current_hsv.s = 0;
                saturation_d = INCREASE;
            }
        }
    } 
    else if (mode_global == PICKING_VALUE) {
        if (value_d == INCREASE) {
            current_hsv.v += STEP_OF_COLOR_CHANGE;
            if (current_hsv.v >= 100) {
                current_hsv.v = 100;
                value_d = DECREASE;
            }
        } else {
            current_hsv.v -= STEP_OF_COLOR_CHANGE;
            if (current_hsv.v <= 0) {
                current_hsv.v = 0;
                value_d = INCREASE;
            }
        }
    }

    COLOR_RGB c = hsv_to_rgb(current_hsv);
    NRF_LOG_INFO("R=%d, G=%d, B=%d", c.r, c.g, c.b);
    show_rgb_color(c);
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
        nvram_save_settings(current_hsv);
    }
}

void unknown_command_executor(char* args)
{
    char msg[100];
    strcpy(msg, ERROR_MESSAGE);
    usb_serial_dumb_print(msg, strlen(msg));
}

bool try_parse_int_arg(char * carg, int * pos, uint16_t * arg)
{
    uint16_t parsed_arg = 0;
    int max_pos = strlen(carg);
    while((*pos < max_pos) && (carg[*pos] == ' '))
        *pos = *pos + 1;

    int digits = 0;
    while(*pos < max_pos && carg[*pos] != ' ')
    {
        if (carg[*pos] < '0' || carg[*pos] > '9')
            return false; // not a digit
        if (++digits > 3)
            return false; // too big number
        parsed_arg = parsed_arg * 10 + (uint16_t)(carg[*pos] - '0');
        *pos = *pos + 1;
    }

    *arg = parsed_arg;
    return digits > 0;
}


void set_rgb_executor(char* args)
{
    char msg[100];
    COLOR_RGB color = 
    {
        .r = 0,
        .g = 0,
        .b = 0,
    };
    bool is_args_valid = true;
    int cur_pos = 0;
    if(!try_parse_int_arg(args, &cur_pos, &(color.r)) || color.r > 255){
        is_args_valid = false;
    }     
    if(!is_args_valid || !try_parse_int_arg(args, &cur_pos, &(color.g)) || color.g > 255){
        is_args_valid = false;
    } 
    
    if(!is_args_valid || !try_parse_int_arg(args, &cur_pos, &(color.b)) || color.b > 255){
        is_args_valid = false;
    }

    if (is_args_valid)
    {
        show_rgb_color(color);
        snprintf(msg, sizeof(msg),
                "Color set to: R=%u, G=%u, B=%u\r\n", color.r, color.g, color.b);
        
        usb_serial_dumb_print(msg, strlen(msg));
    }
    else 
    {
        unknown_command_executor(args);
        NRF_LOG_INFO("Invalid set rgb arguments detected: %s", args);
    }
}



void set_hsv_executor(char* args)
{
    char msg[100];
    COLOR_HSV color = 
    {
        .h = 0,
        .s = 0,
        .v = 0,
    };
    
    int cur_pos = 0;
    bool is_args_valid = true;
    if(!try_parse_int_arg(args, &cur_pos, &(color.h)) || color.h > 360){
        is_args_valid = false;
    }

    uint16_t parsed_value = 0;
    if(!is_args_valid || !try_parse_int_arg(args, &cur_pos, &parsed_value) || parsed_value > 100){
        is_args_valid = false;
    } else {
        color.s = (char)parsed_value;
    }
    
    if(!is_args_valid || !try_parse_int_arg(args, &cur_pos, &parsed_value) || parsed_value > 100){
        is_args_valid = false;
    } else {
        color.v = (char)parsed_value;
    }

    if (is_args_valid)
    {
        COLOR_RGB rgb = hsv_to_rgb(color);

        show_rgb_color(rgb);
        snprintf(msg, sizeof(msg),
                "Color set to: H=%u, S=%u, V=%u\r\n", color.h, color.s, color.v);
        
        usb_serial_dumb_print(msg, strlen(msg));
    }
    else 
    {
        unknown_command_executor(args);
        NRF_LOG_INFO("Invalid set hsv arguments detected: %s", args);
    }
    
}


void print_help_message(char* args);

COMMAND_DEFINITION command_definitions[] = {
    {
        .command_type = CMD_SET_RGB,
        .name = "RGB",
        .description = "RGB <red> <green> <blue> - the device sets current color to specified one.\r\n",
        .executor = set_rgb_executor
    },
    {
        .command_type = CMD_SET_HSV,
        .name = "HSV",
        .description = "HSV <hue> <saturation> <value> - the same with RGB, but color is specified in HSV.\r\n",
        .executor =  set_hsv_executor
    },
    {
        .command_type = CMD_HELP,
        .name = "HELP",
        .description = "HELP - prints this message.\r\n",
        .executor = print_help_message
    }
    // {
    //     .command_type = CMD_ADD_RGB,
    //     .name = "ADD_RGB",
    //     .description = "ADD_RGB <red> <green> <blue> - the device adds specified color to the list of stored.\r\n",
    //     .executor = set_rgb_executor
    // },
    // {
    //     .command_type = LIST_COLORS,
    //     .name = "LIST_COLORS",
    //     .description = "LIST_COLORS - prints all stored colors.\r\n",
    //     .executor = list_colors_executor
    // }
};

void print_help_message(char* args)
{
    char msg[1024];
    msg[0] = '\0';

    strncat(msg, "Supported commands:\r\n", sizeof(msg) - strlen(msg) - 1);
    for (size_t i = 0; i < sizeof(command_definitions) / sizeof(COMMAND_DEFINITION); i++)
    {
        if(command_definitions[i].command_type != CMD_UNKNOWN) 
        {
            strncat(msg, command_definitions[i].description, sizeof(msg) - strlen(msg) - 1);
        }
    }
    usb_serial_dumb_print(msg, strlen(msg));
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

int main(void)
{
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk
            | SCB_SHCSR_BUSFAULTENA_Msk
            | SCB_SHCSR_USGFAULTENA_Msk;
    logs_init();
    if (is_version_changed(CURRENT_VERSION))
    {
        update_version(CURRENT_VERSION);    
    } 
    else
    {
        nvram_load_settings(&current_hsv);
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
