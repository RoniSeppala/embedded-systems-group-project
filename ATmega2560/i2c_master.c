#include <avr/io.h>
#include <stdio.h>

#include "bit_ops.h"
#include "i2c_master.h"

/*
 * TWI/I2C register notes:
 *
 * TWBR  = TWI Bit Rate Register.
 *         Controls the SCL clock frequency together with the prescaler bits in TWSR.
 *
 * TWSR  = TWI Status Register.
 *         Used here to set the prescaler bits to 0. During debugging, it can also be
 *         read to check the current TWI status code.
 *
 * TWCR  = TWI Control Register.
 *         Used to enable TWI and control START, STOP, and data transfer operations.
 *
 * TWDR  = TWI Data Register.
 *         Holds the address or data byte being transmitted.
 *
 * TWINT = TWI Interrupt Flag bit in TWCR.
 *         Set by hardware when the current TWI operation has completed. Writing 1 clears it.
 *
 * TWSTA = TWI START Condition bit in TWCR.
 *         Requests a START condition on the I2C bus.
 *
 * TWSTO = TWI STOP Condition bit in TWCR.
 *         Requests a STOP condition on the I2C bus.
 *
 * TWEN  = TWI Enable bit in TWCR.
 *         Enables the TWI hardware module.
 */
void i2c_master_init(void)
{
    TWBR = 0x03;
    TWSR = 0x00;
    SET_BIT(TWCR, TWEN);
}

void i2c_master_send_byte(uint8_t slave_address, uint8_t data)
{
    uint8_t write_address = (slave_address << 1) | 0;

    printf("TX 0x%02X '%c'\r\n", data, data);

    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    while (!READ_BIT(TWCR, TWINT))
    {
        ;
    }

    TWDR = write_address;
    TWCR = (1 << TWINT) | (1 << TWEN);

    while (!READ_BIT(TWCR, TWINT))
    {
        ;
    }

    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);

    while (!READ_BIT(TWCR, TWINT))
    {
        ;
    }

    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}