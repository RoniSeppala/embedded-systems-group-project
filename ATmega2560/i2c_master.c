#include <avr/io.h>
#include "i2c_master.h"

void i2c_master_init(void)
{
    TWBR = 0x03;
    TWSR = 0x00;
    TWCR |= (1 << TWEN);
}

void i2c_master_send_byte(uint8_t slave_address, uint8_t data)
{
    uint8_t write_address = (slave_address << 1) | 0;

    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    while (!(TWCR & (1 << TWINT)))
    {
        ;
    }

    TWDR = write_address;
    TWCR = (1 << TWINT) | (1 << TWEN);

    while (!(TWCR & (1 << TWINT)))
    {
        ;
    }

    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);

    while (!(TWCR & (1 << TWINT)))
    {
        ;
    }

    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}
