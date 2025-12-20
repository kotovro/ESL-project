#include "command_utils.h"

extern COMMAND_DEFINITION command_definitions[];

void print_help_message(char* args)
{
    char msg[1024];
    msg[0] = '\0';

    strncat(msg, "Supported commands:\r\n", sizeof(msg) - strlen(msg) - 1);
    for (size_t i = 0; i < sizeof(command_definitions) / sizeof(COMMAND_DEFINITION); i++)
    {
        if(command_definitions[i].command_type != CMD_UNKNOWN) 
        {
            strncat(msg, command_definitions[i].description, sizeof(msg) - strlen(msg) - 1);
        }
    }
    usb_serial_dumb_print(msg, strlen(msg));
}

COMMAND_DEFINITION help_command = 
{
    .command_type = CMD_HELP,
    .name = "HELP",
    .description = "HELP - prints this message.\r\n",
    .executor = print_help_message
};