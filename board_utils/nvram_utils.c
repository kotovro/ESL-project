#include "nvram_utils.h"
#include "nrfx_nvmc.h"
#include "led_utils.h"
#include <string.h>


uint32_t color_hsv_to_uint(COLOR_HSV s)
{
    uint32_t value;
    memcpy(&value, &s, sizeof(value));
    return value;
}

COLOR_HSV uint_to_color_hsv(uint32_t value)
{
    COLOR_HSV s;
    memcpy(&s, &value, sizeof(s));
    return s;
}

void nvram_save_settings(COLOR_HSV current_color)
{
    //uint32_t stored_value = *NVRAM_SETTINGS_ADDRESS;
    // COLOR_HSV stored_color = uint_to_color_hsv(stored_value);
    COLOR_HSV stored_color = uint_to_color_hsv(*(uint32_t*)APPDATA_START_ADDR);
    if (stored_color.h == current_color.h &&
        stored_color.s == current_color.s &&
        stored_color.v == current_color.v)
        return;
    // if (!nrfx_nvmc_word_writable_check((uint32_t)NVRAM_SETTINGS_ADDRESS, color_hsv_to_uint(current_color))) {
    //     nrfx_nvmc_page_erase(NVRAM_SETTINGS_PAGE_ADDRESS);
    // }
    // uint32_t pg_num = NRF_FICR->CODESIZE - 1;
    // uint32_t pg_size = NRF_FICR->CODEPAGESIZE;
    // uint32_t addr = 0xE0000 - 0x1000/*(pg_num * pg_size)*/;
    nrfx_nvmc_page_erase(APPDATA_START_ADDR);
    // nrfx_nvmc_page_erase(NVRAM_SETTINGS_PAGE_ADDRESS);
    // nrfx_nvmc_word_write((uint32_t)NVRAM_SETTINGS_ADDRESS, color_hsv_to_uint(current_color));
    nrfx_nvmc_word_write(APPDATA_START_ADDR, color_hsv_to_uint(current_color));
    while (!nrfx_nvmc_write_done_check()) {
    }
}

void nvram_load_settings(COLOR_HSV* current_color)
{
    //uint32_t stored_value = *NVRAM_SETTINGS_ADDRESS;
    // COLOR_HSV stored_color = uint_to_color_hsv(stored_value);
    COLOR_HSV stored_color = uint_to_color_hsv(*(uint32_t*)APPDATA_START_ADDR);
    if (stored_color.h > 360 ||
        stored_color.s > 100 ||
        stored_color.v > 100) 
        return;
    
    current_color->h = stored_color.h;
    current_color->s = stored_color.s;
    current_color->v = stored_color.v;

}