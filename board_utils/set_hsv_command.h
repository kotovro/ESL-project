#include "command_utils.h"

void set_hsv_executor(char* args)
{
    char msg[100];
    COLOR_HSV color = 
    {
        .h = 0,
        .s = 0,
        .v = 0,
    };
    
    int cur_pos = 0;
    bool is_args_valid = true;
    if(!try_parse_int_arg(args, &cur_pos, &(color.h)) || color.h > 360){
        is_args_valid = false;
    }

    uint16_t parsed_value = 0;
    if(!is_args_valid || !try_parse_int_arg(args, &cur_pos, &parsed_value) || parsed_value > 100){
        is_args_valid = false;
    } else {
        color.s = (char)parsed_value;
    }
    
    if(!is_args_valid || !try_parse_int_arg(args, &cur_pos, &parsed_value) || parsed_value > 100){
        is_args_valid = false;
    } else {
        color.v = (char)parsed_value;
    }

    if (is_args_valid)
    {
        COLOR_RGB rgb = hsv_to_rgb(color);

        show_rgb_color(rgb);
        snprintf(msg, sizeof(msg),
                "Color set to: H=%u, S=%u, V=%u\r\n", color.h, color.s, color.v);
        
        usb_serial_dumb_print(msg, strlen(msg));
    }
    else 
    {
        unknown_command_executor(args);
        NRF_LOG_INFO("Invalid set hsv arguments detected: %s", args);
    }
    
}


COMMAND_DEFINITION set_hsv_command  = 
{
        .command_type = CMD_SET_HSV,
        .name = "HSV",
        .description = "HSV <hue> <saturation> <value> - the same with RGB, but color is specified in HSV.\r\n",
        .executor =  set_hsv_executor
};