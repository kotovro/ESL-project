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



void command_executor(COMMAND cmd)
{
    char msg[400];
    switch(cmd.command_type)
    {
    case CMD_HELP:
    {
        strcpy(msg, HELP_MESSAGE);
        break;                                             
    }
    case CMD_SET_RGB:
    {
        COLOR_RGB color = 
        {
            .r = cmd.arg1 / 255.f * 1024,
            .g = cmd.arg2 / 255.f * 1024,
            .b = cmd.arg3 / 255.f * 1024,
        };
        show_rgb_color(color);
        snprintf(msg, sizeof(msg),
        "Color set to: R=%u, G=%u, B=%u\r\n", cmd.arg1, cmd.arg2, cmd.arg3);
        break;
    }
    case CMD_SET_HSV:
    {
        COLOR_HSV hsv = 
        {
            .h = cmd.arg1,
            .s = cmd.arg2,
            .v = cmd.arg3, 
        };

        COLOR_RGB rgb = hsv_to_rgb(hsv);

        show_rgb_color(rgb);
        snprintf(msg, sizeof(msg),
        "Color set to: H=%u, S=%u, S=%u\r\n", cmd.arg1, cmd.arg2, cmd.arg3);
        break;
    }
    default:
    {
        strcpy(msg, ERROR_MESSAGE);
        break;
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
    timers_init();
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
    init_button();
    init_usb_cli(command_executor);
    main_loop();
}

/**
 *@}
 **/
