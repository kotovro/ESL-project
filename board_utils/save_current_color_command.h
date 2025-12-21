#include "command_utils.h"

void save_current_color()
{
    settings.saved_color.h = current_color_description.first_component;
    settings.saved_color.s = (uint8_t)current_color_description.second_component;
    settings.saved_color.v = (uint8_t)current_color_description.third_component;
    nvram_save_settings((uint32_t*)&settings, sizeof(settings));
    char msg[100];
    snprintf(msg, sizeof(msg),
                "Current color successfully saved\r\n");    
    usb_serial_dumb_print(msg, strlen(msg));
}


COMMAND_DEFINITION save_current_color_command =
{
        .command_type = CMD_SAVE_CURRENT_COLOR,
        .name = "SAVE_COLOR",
        .description = "SAVE_COLOR - save current LED color to NVRAM.\r\n",
        .executor = save_current_color
};
