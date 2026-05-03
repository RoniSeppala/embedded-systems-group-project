#include "mcu.h"
#include "uart.h"
#include "board_config.h"
#include "bit_ops.h"

#include "keypad.h"
#include "lcd.h"


//i2c coms includes
#include "elevator_protocol.h"
#include "i2c_master.h"

i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_MOVING);

// TODO: add error handling




int main(void)
{
    // run in the beginning
    // i2c init
    i2c_master_init();

    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_ALL_OFF);

    while (1)
    {
        // loop
    }
 
    return 0;
}