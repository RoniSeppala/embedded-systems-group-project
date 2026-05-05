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
    // Standard-mode 100 kHz at F_CPU=16 MHz: TWBR ~= 72, prescaler=1.
    TWBR = 72;
    TWSR = 0x00;
    SET_BIT(TWCR, TWEN);
}

void i2c_master_send_byte(uint8_t slave_address, uint8_t data)
{
    uint8_t write_address = (slave_address << 1) | 0;
    uint8_t attempt;
    uint8_t status;

    printf("TX 0x%02X '%c'\r\n", data, data);

    for (attempt = 0u; attempt < 3u; attempt++)
    {
        TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

        while (!READ_BIT(TWCR, TWINT)) { ; }
        status = TWSR & 0xF8;
        printf("MASTER TWSR after START: %02X\r\n", status);
        if ((status != 0x08) && (status != 0x10))
        {
            continue;
        }

        TWDR = write_address;
        TWCR = (1 << TWINT) | (1 << TWEN);

        while (!READ_BIT(TWCR, TWINT)) { ; }
        status = TWSR & 0xF8;
        printf("MASTER TWSR after ADDR:  %02X\r\n", status);
        if (status != 0x18)
        {
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
            continue;
        }

        TWDR = data;
        TWCR = (1 << TWINT) | (1 << TWEN);

        while (!READ_BIT(TWCR, TWINT)) { ; }
        status = TWSR & 0xF8;
        printf("MASTER TWSR after DATA:  %02X\r\n", status);
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

        if (status == 0x28)
        {
            return;
        }
    }

    printf("MASTER send failed after retries\r\n");
}