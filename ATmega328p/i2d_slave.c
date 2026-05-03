#include <avr/io.h>
#include <stdint.h>
#include "i2c_slave.h"

void i2c_slave_init(uint8_t slave_address)
{
    TWAR = (slave_address << 1);
    TWSR = 0x00;
    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
}

uint8_t i2c_slave_receive_byte(uint8_t *data)
{
    uint8_t status;

    if (!(TWCR & (1 << TWINT)))
    {
        return 0;
    }

    status = TWSR & 0xF8;

    if ((status == 0x80) || (status == 0x90))
    {
        *data = TWDR;
        TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

        return 1;
    }

    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

    return 0;
}