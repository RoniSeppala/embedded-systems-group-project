// general includes
#include <stdint.h>
#include "mcu.h"

// hw libraries
#include "keypad.h"
#include "lcd.h"

// elevator stuff
#include "elevator_controller.h"
#include "elevator_protocol.h"

// i2c init
#include "i2c_master.h"

// constants and types
#define ELEVATOR_INITIAL_FLOOR (0u)

typedef enum
{
    ELEVATOR_STATE_IDLE,
    ELEVATOR_STATE_GOING_UP,
    ELEVATOR_STATE_GOING_DOWN,
    ELEVATOR_STATE_DOOR_OPENING,
    ELEVATOR_STATE_DOOR_CLOSING,
    ELEVATOR_STATE_OBSTACLE_DETECTION,
    ELEVATOR_STATE_FAULT
} elevator_state_t;

// variable inits
static elevator_state_t current_state = ELEVATOR_STATE_IDLE;
static uint8_t current_floor = ELEVATOR_INITIAL_FLOOR;
static uint8_t target_floor = ELEVATOR_INITIAL_FLOOR;

// lcd idle helper
static void elevator_display_idle(void)
{
    lcd_clrscr();
    lcd_gotoxy(0, 0);
    lcd_puts("Choose floor");
    lcd_gotoxy(0, 1);
    lcd_puts("Current: 0");
}

// elevator code
void elevator_controller_init(void)
{
    current_state = ELEVATOR_STATE_IDLE;
    current_floor = ELEVATOR_INITIAL_FLOOR;
    target_floor = ELEVATOR_INITIAL_FLOOR;

    i2c_master_init();
    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_ALL_OFF);

    KEYPAD_Init();

    lcd_init(LCD_DISP_ON);
    elevator_display_idle();
}

// elevator logic
void elevator_controller_run(void)
{
}