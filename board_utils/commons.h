#include <stdint.h>

#ifndef CONFIG_H
#define CONFIG_H

// --- Button settings ---
#define BUTTON_PIN          38
#define BUTTON_PRESSED       0
#define DEBOUNCE_TIME_MS    50
#define DOUBLE_CLICK_MS    400

// --- Modes ---
#define SLEEP               0
#define PICKING_HUE         1
#define PICKING_SATURATION  2
#define PICKING_VALUE       3

// --- Color ---
#define COLOR_CHANGE_MS    100
#define COLOR_CHANGE_DELAY (DOUBLE_CLICK_MS + 50)

// --- Fade steps ---
// Allow user override
#ifndef FADE_STEPS
#define FADE_STEPS         32
#endif

// --- Directions ---
#define INCREASE true
#define DECREASE false

typedef struct {
    int16_t h;
    char s;
    char v;
} COLOR_HSV;

#define DISABLE_APP_STORAGE 1
// Memory address for NVRAM storage

#define PAGE_SIZE  0x1000
#ifndef NRF_DFU_APP_DATA_AREA_SIZE
#define NRF_DFU_APP_DATA_AREA_SIZE PAGE_SIZE * 3
#endif

#ifndef BOOTLOADER_START_ADDR
#define BOOTLOADER_START_ADDR ((NRF_FICR->CODESIZE - 1) * NRF_FICR->CODEPAGESIZE)
#endif

#define NVRAM_SETTINGS_ADDRESS ((uint32_t*)(BOOTLOADER_START_ADDR - NRF_DFU_APP_DATA_AREA_SIZE) - DISABLE_APP_STORAGE)
#define NVRAM_SETTINGS_PAGE_ADDRESS ((uint32_t)(BOOTLOADER_START_ADDR - NRF_DFU_APP_DATA_AREA_SIZE - PAGE_SIZE * DISABLE_APP_STORAGE))

#endif // CONFIG_H
