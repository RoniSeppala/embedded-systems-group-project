#include <avr/io.h>
#include <stdint.h>

#include "bit_ops.h"
#include "i2c_slave.h"

/*
 * TWI/I2C register notes:
 *
 * TWAR  = TWI Address Register.
 *         Stores this device's own slave address.
 *
 * TWSR  = TWI Status Register.
 *         Contains the current TWI status code. The status code is masked with 0xF8 because
 *         the lowest bits are prescaler bits, not part of the status code.
 *
 * TWCR  = TWI Control Register.
 *         Used to enable TWI, clear the interrupt flag, send ACKs, and control TWI operation.
 *
 * TWDR  = TWI Data Register.
 *         Holds the received or transmitted data byte.
 *
 * TWINT = TWI Interrupt Flag bit in TWCR.
 *         Set by hardware when the current TWI operation has completed. Writing 1 clears it.
 *
 * TWEA  = TWI Enable Acknowledge bit in TWCR.
 *         Makes the slave send ACK after receiving its address or a data byte.
 *
 * TWEN  = TWI Enable bit in TWCR.
 *         Enables the TWI hardware module.
 */

 // Initializes the TWI hardware as a slave using the given 7-bit slave address.
void i2c_slave_init(uint8_t slave_address)
{
    TWAR = (slave_address << 1);
    TWSR = 0x00;
    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
}

// Polls the TWI hardware and returns 1 when a new byte has been received.
uint8_t i2c_slave_receive_byte(uint8_t *data)
{
    uint8_t status;

    if (!READ_BIT(TWCR, TWINT))
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