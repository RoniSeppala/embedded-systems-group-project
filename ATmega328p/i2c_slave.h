#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <stdint.h>

void i2c_slave_init(uint8_t slave_address);
uint8_t i2c_slave_receive_byte(uint8_t *data);

#endif