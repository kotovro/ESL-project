#include "command_utils.h"

void add_rgb_executor(char* args)
{
    
    char msg[100];
    COLOR_DESCRIPTION colorDesc;
    colorDesc.colorType = 0; // RGB
    bool is_args_valid = true;
    int cur_pos = 0;
    
    if (!try_parse_str_arg(args, &cur_pos, colorDesc.colorName, 11)){
        is_args_valid = false;
    }
    if(!is_args_valid || !try_parse_int_arg(args, &cur_pos, &(colorDesc.first_component)) || colorDesc.first_component > 255){
        is_args_valid = false;
    }     
    uint16_t value = colorDesc.second_component;
    if(!is_args_valid || !try_parse_int_arg(args, &cur_pos, &(value)) || value > 255){
        is_args_valid = false;
    } 
    colorDesc.second_component = (uint8_t)value;
    value = colorDesc.third_component;
    
    if(!is_args_valid || !try_parse_int_arg(args, &cur_pos, &(value)) || colorDesc.third_component > 255){
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
                "Color added: R=%u, G=%u, B=%u\r\n", colorDesc.first_component, colorDesc.second_component, colorDesc.third_component);
        
        usb_serial_dumb_print(msg, strlen(msg));
    }
}

COMMAND_DEFINITION add_rgb_command =  
{
        .command_type = CMD_ADD_RGB,
        .name = "ADD_RGB",
        .description = "ADD_RGB <color_name> <red> <green> <blue> - the device adds specified RGB color to the list of stored.\r\n",
        .executor = add_rgb_executor
};