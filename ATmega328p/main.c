#include "mcu.h"
#include "uart.h"

//elevator init
#include <stdint.h>
#include "elevator_protocol.h"
#include "i2c_slave.h"

// mega output handling
#include "outputs.h"
#include "delay.h"


#define UNO_OUTPUT_UPDATE_DELAY_MS (25u)


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

    setup_uart_io(); // setup uart for debuging

    i2c_slave_init(ELEVATOR_I2C_ADDRESS);
    outputs_init();

    while (1)
    {
        // loop
        if (i2c_slave_receive_byte(&command))
        {
            elevator_handle_command(command);
        }

        outputs_update();
        DELAY_ms(UNO_OUTPUT_UPDATE_DELAY_MS);
    }
 
    return 0;
}