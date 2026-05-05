#include "mcu.h"

#include <stdint.h>

#include "uart.h"
#include "elevator_protocol.h"
#include "i2c_slave.h"
#include "outputs.h"
#include "delay.h"

#define UNO_OUTPUT_UPDATE_DELAY_MS (25u)

// maps i2c input from mega to actions on this device
static void elevator_handle_command(uint8_t command)
{
    switch (command)
    {
        case ELEVATOR_CMD_ALL_OFF:
            outputs_all_off();
            break;

        case ELEVATOR_CMD_MOVING:
            outputs_set_movement();
            break;

        case ELEVATOR_CMD_DOOR_OPENING:
            outputs_set_door_opening();
            break;

        case ELEVATOR_CMD_DOOR_CLOSING:
            outputs_set_door_closing();
            break;

        case ELEVATOR_CMD_OBSTACLE:
            outputs_set_obstacle();
            break;

        case ELEVATOR_CMD_BUZZER_STOP:
            outputs_stop_buzzer();
            break;

        case ELEVATOR_CMD_JINGLE_START:
            outputs_start_jingle();
            break;

        case ELEVATOR_CMD_JINGLE_STOP:
            outputs_stop_jingle();
            break;

        default:
            outputs_all_off();
            break;
    }
}

int main(void)
{
    // run in the beginning
    // i2c init
    uint8_t command;
    uint8_t output_update_ticks = 0u;

    setup_uart_io(); // setup uart for debuging

    outputs_init();
    i2c_slave_init(ELEVATOR_I2C_ADDRESS);

    while (1)
    {
        // Poll I2C frequently to avoid missing back-to-back transactions.
        if (i2c_slave_receive_byte(&command))
        {
            elevator_handle_command(command);
        }

        DELAY_ms(1);
        output_update_ticks++;

        if (output_update_ticks >= UNO_OUTPUT_UPDATE_DELAY_MS)
        {
            output_update_ticks = 0u;
            outputs_update();
        }
    }
 
    return 0;
}