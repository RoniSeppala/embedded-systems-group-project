#ifndef UART_H
#define UART_H

#include <stdio.h>

#ifndef UART_BAUD_RATE
#define UART_BAUD_RATE (9600)
#endif

extern FILE uart_output;
extern FILE uart_input;

uint8_t setup_uart_io(void);

#endif