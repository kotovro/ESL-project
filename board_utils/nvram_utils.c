#include "nvram_utils.h"
#include "nrfx_nvmc.h"
#include "led_utils.h"
#include <string.h>

bool is_erase_needed = true;

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

void update_version(uint32_t version)
{
    nrfx_nvmc_page_erase(APPDATA_START_ADDR);
    is_erase_needed = false;

    nrfx_nvmc_word_write(VERSION_SETTING_ADDRESS, version);
    while (!nrfx_nvmc_write_done_check()) {
    }
    return;
}

bool is_version_changed(uint32_t version)
{
    return *(uint32_t*)VERSION_SETTING_ADDRESS != version;
}

void nvram_save_settings(COLOR_HSV current_color)
{
    COLOR_HSV stored_color = uint_to_color_hsv(*(uint32_t*)HSV_SETTING_ADDR);
    if (stored_color.h == current_color.h &&
        stored_color.s == current_color.s &&
        stored_color.v == current_color.v)
        return;
    if (is_erase_needed || !nrfx_nvmc_word_writable_check((uint32_t)HSV_SETTING_ADDR, color_hsv_to_uint(current_color))) 
    {
        nrfx_nvmc_page_erase(APPDATA_START_ADDR);
        nrfx_nvmc_word_write(VERSION_SETTING_ADDRESS, CURRENT_VERSION);
        while (!nrfx_nvmc_write_done_check()) {
        }
        is_erase_needed = false;
    } else {
        is_erase_needed = true;
    }

    nrfx_nvmc_word_write(HSV_SETTING_ADDR, color_hsv_to_uint(current_color));
    while (!nrfx_nvmc_write_done_check()) {
    }
}

void nvram_load_settings(COLOR_HSV* current_color)
{
    COLOR_HSV stored_color = uint_to_color_hsv(*(uint32_t*)HSV_SETTING_ADDR);
    if (stored_color.h > 360 ||
        stored_color.s > 100 ||
        stored_color.v > 100) 
        return;
    
    current_color->h = stored_color.h;
    current_color->s = stored_color.s;
    current_color->v = stored_color.v;
}