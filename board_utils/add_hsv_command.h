#include "command_utils.h"

void add_hsv_executor(char* args)
{
    char msg[100];
    COLOR_DESCRIPTION colorDesc;
    colorDesc.colorType = 1; // HSV
    bool is_args_valid = true;
    int cur_pos = 0;
    
    if (!try_parse_str_arg(args, &cur_pos, colorDesc.colorName, 11)){
        is_args_valid = false;
    }
    if(!is_args_valid || !try_parse_int_arg(args, &cur_pos, &(colorDesc.first_component)) || colorDesc.first_component > 360){
        is_args_valid = false;
    }     
    uint16_t value = colorDesc.second_component;
    if(!is_args_valid || !try_parse_int_arg(args, &cur_pos, &(value)) || value > 100){
        is_args_valid = false;
    } 
    colorDesc.second_component = (uint8_t)value;
    value = colorDesc.third_component;
    
    if(!is_args_valid || !try_parse_int_arg(args, &cur_pos, &(value)) || colorDesc.third_component >  100){
        is_args_valid = false;
    }
    colorDesc.third_component = (uint8_t)value;
    if (is_args_valid)
    {   
        memmove(&colorPalette[1],
        &colorPalette[0],
        9 * sizeof(COLOR_DESCRIPTION));

        // Now index 0 is free
        colorPalette[0] = colorDesc;

        snprintf(msg, sizeof(msg),
                "Color added: H=%u, S=%u, V=%u\r\n", colorDesc.first_component, colorDesc.second_component, colorDesc.third_component);
        
        usb_serial_dumb_print(msg, strlen(msg));
    }
}

COMMAND_DEFINITION add_hsv_command = 
{
        .command_type = CMD_ADD_HSV,
        .name = "ADD_HSV",
        .description = "ADD_HSV <color_name> <hue> <saturation> <value> - the device adds specified HSV color to the list of stored.\r\n",
        .executor = add_hsv_executor
};