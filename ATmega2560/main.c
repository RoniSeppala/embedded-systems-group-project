#include "mcu.h"
#include "uart.h"

// elevator controller
#include "elevator_controller.h"

int main(void)
{
    setup_uart_io(); //setup uart for debuging
    // run in the beginning
    elevator_controller_init();

    while (1)
    {
        // loop
        elevator_controller_run();
    }
 
    return 0;
}