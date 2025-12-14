#if ESTC_USB_CLI_ENABLED == 0
void init_usb_cli(Command_Executor executor) {}
#else 

#include "cli_utils.h"

static char m_command_buffer[MAX_COMMAND_SIZE];
static char m_rx_buffer[READ_SIZE];
static volatile bool m_tx_done = true;
Command_Executor cmd_executor;

APP_TIMER_DEF(parse_command_timer_id);

APP_USBD_CDC_ACM_GLOBAL_DEF(usb_cdc_acm,
                        usb_ev_handler,
                        CDC_ACM_COMM_INTERFACE,
                        CDC_ACM_DATA_INTERFACE,
                        CDC_ACM_COMM_EPIN,
                        CDC_ACM_DATA_EPIN,
                        CDC_ACM_DATA_EPOUT,                   
                        APP_USBD_CDC_COMM_PROTOCOL_NONE);


static void parse_command_timer_handler(void * p_context);
void init_usb_cli(Command_Executor executor)
{                       
    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&usb_cdc_acm);
    ret_code_t ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);
    
    ret = app_timer_create(&parse_command_timer_id,
                            APP_TIMER_MODE_SINGLE_SHOT,
                            parse_command_timer_handler);
    APP_ERROR_CHECK(ret);
    cmd_executor = executor;
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



void usb_serial_dumb_print(char const * p_buffer, size_t len)
{

        ret_code_t ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                                p_buffer,
                                                len);
        APP_ERROR_CHECK(ret);
}

bool try_parse_arg(uint16_t * arg, int * pos)
{
    uint16_t parsed_arg = 0;
    int max_pos = strlen(m_command_buffer);
    while((*pos < max_pos) && (m_command_buffer[*pos] == ' '))
        *pos = *pos + 1;

    int digits = 0;
    while(*pos < max_pos && m_command_buffer[*pos] != ' ')
    {
        if (m_command_buffer[*pos] < '0' || m_command_buffer[*pos] > '9')
            return false; // not a digit
        if (++digits > 3)
            return false; // too big number
        parsed_arg = parsed_arg * 10 + (uint16_t)(m_command_buffer[*pos] - '0');
        *pos = *pos + 1;
    }

    *arg = parsed_arg;
    return digits > 0;
}

bool try_parse_args(COMMAND * cmd)
{
    int cur_pos = 4;
    if (cmd->command_type == CMD_HELP)
    {
        if (strlen(m_command_buffer) == 4 || m_command_buffer[cur_pos] == ' ')
            return true;
    }
    else if (cmd->command_type == CMD_SET_RGB)
    {
        uint16_t parsed_arg = 0;
        if(!try_parse_arg(&parsed_arg, &cur_pos) || parsed_arg > 255){
            return false;
        }
        cmd->arg1 = parsed_arg;
        if(!try_parse_arg(&parsed_arg, &cur_pos) || parsed_arg > 255){
            return false;
        }
        cmd->arg2 = parsed_arg;
        
        if(!try_parse_arg(&parsed_arg, &cur_pos) || parsed_arg > 255){
            return false;
        }
        cmd->arg3 = parsed_arg;
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
    if (strlen(m_command_buffer) < 4) {
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

static void parse_command_timer_handler(void * p_context)
{
    // NRF_LOG_INFO("We will try to parse, m_counter = %d", m_counter);
    COMMAND current_command = parse_command();

    m_command_buffer[0] = '\0';
    // execute_command(current_command);
    cmd_executor(current_command);
}

void usb_ev_handler(app_usbd_class_inst_t const * p_inst,
                           app_usbd_cdc_acm_user_event_t event)
{
    switch (event)
    {
    case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
    {
        m_command_buffer[0] = '\0';
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
        m_tx_done = true;
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
            UNUSED_VARIABLE(size);
            NRF_LOG_INFO("rx: %d", m_rx_buffer[0]);
            /* It's the simple version of an echo. Note that writing doesn't
             * block execution, and if we have a lot of characters to read and
             * write, some characters can be missed.
             */
            if (m_rx_buffer[0] == '\r' || m_rx_buffer[0] == '\n')
            {
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm, "\r\n", 2);
                m_command_buffer[strlen(m_command_buffer)] = '\0';
                app_timer_start(parse_command_timer_id, 5, NULL);
            }
            else
            {
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                             m_rx_buffer,
                                             READ_SIZE);

                int cur_len = strlen(m_command_buffer);
                if (cur_len < MAX_COMMAND_SIZE - 1)
                {
                    m_command_buffer[cur_len] = m_rx_buffer[0];
                    m_command_buffer[cur_len + 1] = '\0';
                }                         
            }

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

#endif