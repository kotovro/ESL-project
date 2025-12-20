#include "command_utils.h"

void apply_color_executor(char* args)
{
    char msg[100];
    char color_name[11];
    bool is_args_valid = true;
    int cur_pos = 0;
    
    if (!try_parse_str_arg(args, &cur_pos, color_name, 11)){
        is_args_valid = false;
    }

    if (is_args_valid)
    {
        bool is_color_found = false;
        for (int i = 0; i < 10; i++) 
        {
            if (strcmp(colorPalette[i].colorName, color_name) == 0 && colorPalette[i].colorType != 255) 
            {
                show_color(colorPalette[i]);
                is_color_found = true;
                snprintf(msg, sizeof(msg),
                        "Color applied: %s\r\n", color_name);
                
                usb_serial_dumb_print(msg, strlen(msg));
                break;
            }
        }
        if (!is_color_found) 
        {
            unknown_command_executor(args);
            NRF_LOG_INFO("Color not found: %s", color_name);
        }
    }
    else 
    {
        unknown_command_executor(args);
        NRF_LOG_INFO("Invalid apply color arguments detected: %s", args);
    }
}

COMMAND_DEFINITION apply_color_command =
{
        .command_type = CMD_APPLY_COLOR,
        .name = "APPLY_COLOR",
        .description = "APPLY_COLOR <color_name>  - the device sets specified color from the list of stored.\r\n",
        .executor = apply_color_executor
};