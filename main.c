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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "led_utils.h"
#include "button_utils.h"

#define LED_GREEN 0
#define LED_RED 1
#define LED_BLUE 2
#define BLINK_SEQUENCE_LEN 14
 
static int last_idx = 0;

void populate_blinking_sequnce(uint32_t* sequence)
{
    for (int i = 0; i < 6; ++i)
        sequence[i] = LED_GREEN;
    for (int i = 6; i < 11; ++i)
        sequence[i] = LED_RED;
    for (int i = 11; i < 14; ++i)
        sequence[i] = LED_BLUE;
}

void main_loop(void)
{
    // while (true)
    // {
    //     int button_press_time_ms = get_button_press_time_ms();
    //     if (button_press_time_ms)
    //     {
    //         for (int i = 0; i < button_press_time_ms; ++i)
    //         {
    //             blink_led(last_idx % BLINK_SEQUENCE_LEN, 1);
    //             last_idx++;
    //         }
            
    //     }
    // }
}

int main(void)
{
    uint32_t* sequence = (uint32_t*) malloc(BLINK_SEQUENCE_LEN);
    populate_blinking_sequnce(sequence);


    init_leds();
    init_button(); 
    while (true)
    {
        int button_press_time_ms = get_button_press_time_ms();
        if (true)
        {
            for (int i = 0; i < button_press_time_ms; ++i)
            {
                last_idx %= BLINK_SEQUENCE_LEN;
                blink_led(sequence[last_idx % BLINK_SEQUENCE_LEN], 1);
                last_idx++;
            }
        }
    }
}

/**
 *@}
 **/
