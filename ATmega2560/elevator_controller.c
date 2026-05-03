#include "elevator_controller.h"

#include "elevator_protocol.h"

// i2c init
#include "i2c_master.h"

void elevator_controller_init(void)
{
    i2c_master_init();
    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_ALL_OFF);
}

// elevator logic
void elevator_controller_run(void)
{
}