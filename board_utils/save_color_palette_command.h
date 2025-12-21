#include "command_utils.h"

void save_current_palette()
{
    memcpy(settings.colorPalette, colorPalette, sizeof(settings.colorPalette));
    nvram_save_settings((uint32_t*)&settings, sizeof(settings));
    char msg[100];
    snprintf(msg, sizeof(msg),
                "Color palette successfully saved\r\n");    
    usb_serial_dumb_print(msg, strlen(msg));
}


COMMAND_DEFINITION save_color_palette_command =
{
        .command_type = CMD_SAVE_COLORS,
        .name = "SAVE_COLORS",
        .description = "SAVE_COLORS - saves all stored colors to NVRAM.\r\n",
        .executor = save_current_palette
};