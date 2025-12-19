#ifndef CLI_UTILS_H
#define CLI_UTILS_H

#include "commons.h"


#define CMD_SET_RGB 0
#define CMD_SET_HSV 1
#define CMD_HELP 2
#define CMD_ADD_RGB 3
#define CMD_ADD_HSV 4
#define CMD_APPLY_COLOR 5
#define CMD_LIST_COLORS 6
#define CMD_UNKNOWN 255 
#define HELP_MESSAGE "Supported commands:\r\n" \
            "RGB <red> <green> <blue> - the device sets current color to specified one.\r\n" \
            "HSV <hue> <saturation> <value> - the same with RGB, but color is specified in HSV.\r\n" \
            "help - print this information.\r\n"
#define ERROR_MESSAGE "Unknown command\r\n"
    
typedef void  (*Command_Executor)(char*);

typedef struct
{
    bool is_required;
    uint16_t  min_value;
    uint16_t  max_value;
} ARG;

typedef struct
{
    uint8_t command_type;
    Command_Executor executor;    
    char* name;
    char* description;
} COMMAND_DEFINITION;




void init_usb_cli(COMMAND_DEFINITION* known_commands, size_t known_commands_size,
                Command_Executor default_command);
void usb_ev_handler(app_usbd_class_inst_t const * p_inst,
                           app_usbd_cdc_acm_user_event_t event);
void usb_serial_dumb_print(char const * p_buffer, size_t len);
#endif // CLI_UTILS_H