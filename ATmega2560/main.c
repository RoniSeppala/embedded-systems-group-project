#include "mcu.h"
#include "uart.h"
#include "board_config.h"
#include "bit_ops.h"

// elevator controller
#include "elevator_controller.h"

// TODO: add error handling




int main(void)
{
    // run in the beginning
    elevator_controller_init();

    while (1)
    {
        // loop
        elevator_controller_run();
    }
 
    return 0;
}