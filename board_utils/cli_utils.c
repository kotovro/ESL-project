#include "cli_utils.h"

static char m_command_buffer[MAX_COMMAND_SIZE];
static char m_rx_buffer[READ_SIZE];
static volatile bool echo_command_done = true;
static int counter = 0;


APP_USBD_CDC_ACM_GLOBAL_DEF(usb_cdc_acm,
                        usb_ev_handler,
                        CDC_ACM_COMM_INTERFACE,
                        CDC_ACM_DATA_INTERFACE,
                        CDC_ACM_COMM_EPIN,
                        CDC_ACM_DATA_EPIN,
                        CDC_ACM_DATA_EPOUT,                   
                        APP_USBD_CDC_COMM_PROTOCOL_NONE);
void init_usb_cli()
{                       
    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&usb_cdc_acm);
    ret_code_t ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);
}

bool is_command_rgb()
{
    return (m_command_buffer[0] == 'R' || m_command_buffer[0] == 'r')
        && (m_command_buffer[1] == 'G' || m_command_buffer[1] == 'g')
        && (m_command_buffer[2] == 'B' || m_command_buffer[2] == 'b')
        && (m_command_buffer[3] == ' ');
}

bool is_command_hsv()
{
    return (m_command_buffer[0] == 'H' || m_command_buffer[0] == 'h')
        && (m_command_buffer[1] == 'S' || m_command_buffer[1] == 's')
        && (m_command_buffer[2] == 'V' || m_command_buffer[2] == 'v')
        && (m_command_buffer[3] == ' ');
}

bool is_command_help()
{
    return (m_command_buffer[0] == 'H' || m_command_buffer[0] == 'h')
        && (m_command_buffer[1] == 'E' || m_command_buffer[1] == 'e')
        && (m_command_buffer[2] == 'L' || m_command_buffer[2] == 'l')
        && (m_command_buffer[3] == 'P' || m_command_buffer[3] == 'p');
}

void execute_command(COMMAND cmd)
{
    ret_code_t ret;
    switch(cmd.command_type)
    {
    case CMD_UNKNOWN:
    {
        ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                             "Unknown command\r\n",
                                             18);
        APP_ERROR_CHECK(ret);
        break;
    }
    case CMD_HELP:
    {
        echo_command_done = false;
        ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                            "Supported commands:\r\n",
                                            22);
        APP_ERROR_CHECK(ret);

        while (!echo_command_done)
        {
            while (app_usbd_event_queue_process())
            {
                /* Wait until we're ready to send the data again */
            }
        }

        ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                            "RGB <red> <green> <blue> - ",
                                            28);
        APP_ERROR_CHECK(ret);
        // ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
        //                                     "the device sets current color to specified one.\r\n",
        //                                     50);   
        // APP_ERROR_CHECK(ret);                             
        // ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
        //                                     "HSV <hue> <saturation> <value> - ",
        //                                     34);  
        // APP_ERROR_CHECK(ret);
        // ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
        //                                     "the same with RGB, but color is specified in HSV.\r\n",
        //                                     52);
        // APP_ERROR_CHECK(ret);       
        // ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
        //                                     "help - print this information.\r\n",
        //                                     33);  
        // APP_ERROR_CHECK(ret);
        break;                                             
    }
    }
}

bool try_parse_arg(uint16_t * arg, int * pos)
{
    uint16_t parsed_arg = 0;
    while((*pos < counter) && (m_command_buffer[*pos] == ' '))
        *pos = *pos + 1;
    int digits = 0;
    while(*pos < counter && m_command_buffer[*pos] != ' ')
    {
        if (m_command_buffer[*pos] < '0' || m_command_buffer[*pos] > '9')
            return false; // not a digit
        if (++digits > 3)
            return false; // too big number
        parsed_arg = parsed_arg * 10 + (uint16_t)(m_command_buffer[*pos] - '0');
    }
    *arg = parsed_arg;
    return digits > 0;
}

bool try_parse_args(COMMAND * cmd)
{
    int cur_pos = 4;
    
    if (cmd->command_type == CMD_HELP)
    {
        if (counter == 4 || m_command_buffer[cur_pos] == ' ')
            return true;
    }
    else if (cmd->command_type == CMD_SET_RGB)
    {
        uint16_t parsed_arg = 0;
        if(try_parse_arg(&parsed_arg, &cur_pos)){
            if (parsed_arg > 255) {
                return false;
            }
            cmd->arg1 = parsed_arg;
        }
        if(try_parse_arg(&parsed_arg, &cur_pos)){
            if (parsed_arg > 255) {
                return false;
            }
            cmd->arg2 = parsed_arg;
        }
        if(try_parse_arg(&parsed_arg, &cur_pos)){
            if (parsed_arg > 255) {
                return false;
            }
            cmd->arg3 = parsed_arg;
        }   
    }
    else if (cmd->command_type == CMD_SET_HSV)
    {
        uint16_t parsed_arg = 0;
        if(try_parse_arg(&parsed_arg, &cur_pos)){
            if (parsed_arg > 360) {
                return false;
            }
            cmd->arg1 = parsed_arg;
        }
        if(try_parse_arg(&parsed_arg, &cur_pos)){
            if (parsed_arg > 100) {
                return false;
            }
            cmd->arg2 = parsed_arg;
        }
        if(try_parse_arg(&parsed_arg, &cur_pos)){
            if (parsed_arg > 100) {
                return false;
            }
            cmd->arg3 = parsed_arg;
        }   
    }
    return true;
}

COMMAND parse_command()
{
    COMMAND parsed_command = {CMD_UNKNOWN, 0, 0, 0};
    if (counter < 4) {
        NRF_LOG_INFO("Invalid command detected");
    }
    else if (is_command_rgb()) 
    {
        parsed_command.command_type = CMD_SET_RGB;
        if (try_parse_args(&parsed_command)) 
        {
            NRF_LOG_INFO("Valid set rgb command detected");
        }
        else
        {
            parsed_command.command_type = CMD_UNKNOWN;
            NRF_LOG_INFO("Invalid set rgb arguments detected");
        }
    }
    else if (is_command_hsv()) 
    {
        parsed_command.command_type = CMD_SET_HSV;
        if (try_parse_args(&parsed_command)) 
        {
            NRF_LOG_INFO("Valid set hsv command detected");
        }
        else
        {
            parsed_command.command_type = CMD_UNKNOWN;
            NRF_LOG_INFO("Invalid set hsv arguments detected");
        }

    }
    else if (is_command_help())
    {
        parsed_command.command_type = CMD_HELP;
        if (try_parse_args(&parsed_command)) 
        {
            NRF_LOG_INFO("Valid help command detected");
        }
        else
        {
            parsed_command.command_type = CMD_UNKNOWN;
            NRF_LOG_INFO("Invalid help command detected");
        }
    }
    return parsed_command;
}



void usb_ev_handler(app_usbd_class_inst_t const * p_inst,
                           app_usbd_cdc_acm_user_event_t event)
{
    switch (event)
    {
    case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
    {
        counter = 0;
        ret_code_t ret;
        ret = app_usbd_cdc_acm_read(&usb_cdc_acm, m_rx_buffer, READ_SIZE);
        UNUSED_VARIABLE(ret);
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
    {
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
    {
        echo_command_done = true;
        NRF_LOG_INFO("tx done");
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
    {
        ret_code_t ret;
        do
        {
            /*Get amount of data transfered*/
            size_t size = app_usbd_cdc_acm_rx_size(&usb_cdc_acm);
            NRF_LOG_INFO("rx size: %d", size);

            /* It's the simple version of an echo. Note that writing doesn't
             * block execution, and if we have a lot of characters to read and
             * write, some characters can be missed.
             */
            echo_command_done = false;
            if (m_rx_buffer[0] == '\r' || m_rx_buffer[0] == '\n')
            {
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm, "\r\n", 2);
                COMMAND current_command = parse_command();
                counter = 0;
                execute_command(current_command);
            }
            else
            {
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                             m_rx_buffer,
                                             READ_SIZE);
                m_command_buffer[counter] = m_rx_buffer[0];
                counter++;
            }
            
            // while(!echo_command_done)
            // {
            //     // while(app_usbd_event_queue_process())
            //     // {}
            // }
            /* Fetch data until internal buffer is empty */
            ret = app_usbd_cdc_acm_read(&usb_cdc_acm,
                                        m_rx_buffer,
                                        READ_SIZE);
        } while (ret == NRF_SUCCESS);

        break;
    }
    default:
        break;
    }
}