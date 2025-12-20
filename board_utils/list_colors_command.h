#include "command_utils.h"

void list_colors_executor(char* args)
{
    char msg[512];
    msg[0] = '\0';
    strncat(msg, "Stored colors:\r\n", sizeof(msg) - strlen(msg) - 1);
    for (int i = 0; i < 10; i++) 
    {
        if (colorPalette[i].colorType != 255) 
        {
            char color_info[64];
            if (colorPalette[i].colorType == 0) // RGB
            {
                snprintf(color_info, sizeof(color_info),
                        "%s: R=%u, G=%u, B=%u\r\n", 
                        colorPalette[i].colorName,
                        colorPalette[i].first_component,
                        colorPalette[i].second_component,
                        colorPalette[i].third_component);
            } 
            else if (colorPalette[i].colorType == 1) // HSV
            {
                snprintf(color_info, sizeof(color_info),
                        "%s: H=%u, S=%u, V=%u\r\n", 
                        colorPalette[i].colorName,
                        colorPalette[i].first_component,
                        colorPalette[i].second_component,
                        colorPalette[i].third_component);
            }
            strncat(msg, color_info, sizeof(msg) - strlen(msg) - 1);
        }
    }
    usb_serial_dumb_print(msg, strlen(msg));
}

COMMAND_DEFINITION list_colors_command =
{
        .command_type = CMD_LIST_COLORS,
        .name = "LIST_COLORS",
        .description = "LIST_COLORS - prints all stored colors.\r\n",
        .executor = list_colors_executor
};