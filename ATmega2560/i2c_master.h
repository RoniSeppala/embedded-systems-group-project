#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include <stdint.h>

void i2c_master_init(void);
void i2c_master_send_byte(uint8_t slave_address, uint8_t data);

#endif