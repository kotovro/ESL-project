void unknown_command_executor(char* args)
{
    char msg[100];
    strcpy(msg, ERROR_MESSAGE);
    usb_serial_dumb_print(msg, strlen(msg));
}