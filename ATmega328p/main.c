#include "mcu.h"
#include "uart.h"
#include "board_config.h"
#include "bit_ops.h"

//elevator init
#include <stdint.h>
#include "elevator_protocol.h"
#include "i2c_slave.h"

// mega output handling
#include "outputs.h"


// TODO: add error handling

static void elevator_handle_command(uint8_t command)
{
    switch (command)
    {
        case ELEVATOR_CMD_ALL_OFF:
            break;

        case ELEVATOR_CMD_MOVING:
            break;

        case ELEVATOR_CMD_DOOR_OPENING:
            break;

        case ELEVATOR_CMD_DOOR_CLOSING:
            break;

        case ELEVATOR_CMD_OBSTACLE:
            break;

        case ELEVATOR_CMD_BUZZER_STOP:
            break;

        default:
            break;
    }
}

int main(void)
{
    // run in the beginning
    // i2c init
    uint8_t command;

    i2c_slave_init(ELEVATOR_I2C_ADDRESS);
    outputs_init();

    while (1)
    {
        // loop
        if (i2c_slave_receive_byte(&command))
        {
            elevator_handle_command(command);
        }
    }
 
    return 0;
}