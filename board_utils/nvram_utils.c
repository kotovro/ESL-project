#include "nvram_utils.h"
#include "nrfx_nvmc.h"
#include "led_utils.h"
#include <string.h>

bool is_erase_needed = true;


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
    bool is_version_changed = (*(uint32_t*)VERSION_SETTING_ADDRESS != version);
    NRF_LOG_INFO(is_version_changed ? "Version changed, set default color" 
                                    : "Version not changed, load saved color");
    LOG_BACKEND_USB_PROCESS();
    return is_version_changed;
}

void nvram_save_settings(uint32_t* settings, size_t settings_size)
{
    // Check if we need to erase the page
    bool page_needs_erase = is_erase_needed;

    if (!page_needs_erase) {
        // Check if all words can be written without erasing
        for (size_t i = 0; i < settings_size / 4; i++) {
            if (!nrfx_nvmc_word_writable_check((uint32_t)APPDATA_START_ADDR + i * 4, settings[i])) {
                page_needs_erase = true;
                break;
            }
        }
    }

    // Erase page if needed
    if (page_needs_erase) {
        nrfx_nvmc_page_erase(APPDATA_START_ADDR);
        nrfx_nvmc_word_write(VERSION_SETTING_ADDRESS, CURRENT_VERSION);
        while (!nrfx_nvmc_write_done_check()) {
        }
    }

    // Write all settings
    for (size_t i = 1; i < settings_size / 4; i++) {
        nrfx_nvmc_word_write((uint32_t)APPDATA_START_ADDR + i * 4, settings[i]);
        while (!nrfx_nvmc_write_done_check()) {
        }
    }
    NRF_LOG_INFO("Settings successfully saved");
    LOG_BACKEND_USB_PROCESS();
}

void nvram_load_settings(uint32_t* settings, size_t settings_size)
{
    memcpy(settings, (uint32_t*)APPDATA_START_ADDR, settings_size);
}