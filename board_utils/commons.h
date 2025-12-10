
#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>
#include <stdbool.h>
#include "nrf.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"
#include "app_usbd.h"
#include "app_usbd_serial_num.h"

#define CURRENT_VERSION ((uint32_t)3)


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

#define STEP_OF_COLOR_CHANGE 1

// --- Directions ---
#define INCREASE true
#define DECREASE false

typedef struct {
    int16_t h;
    char s;
    char v;
} COLOR_HSV;

// Memory address for NVRAM storage

#ifndef BOOTLOADER_START_ADDR
#define BOOTLOADER_START_ADDR (NRF_UICR->NRFFW[0]) 
#endif
#define PAGE_SIZE  0x1000
#define NRF_DFU_APP_DATA_PAGES 3

#define APPDATA_START_ADDR  (BOOTLOADER_START_ADDR - (NRF_DFU_APP_DATA_PAGES * PAGE_SIZE))
#define HSV_SETTING_ADDR   (APPDATA_START_ADDR)
#define VERSION_SETTING_ADDRESS ((uint32_t)((uint32_t*)APPDATA_START_ADDR + 1))

#endif // CONFIG_H
