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
#define ELEVATOR_MAX_FLOOR (99u)
#define ELEVATOR_INPUT_MAX_DIGITS (2u)
#define KEYPAD_CONFIRM_KEY '#'
#define KEYPAD_CLEAR_KEY '*'
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

// helper functions
static uint8_t is_digit_key(uint8_t key)
{
    return ((key >= '0') && (key <= '9'));
}

static void elevator_display_current_floor(void)
{
    lcd_gotoxy(0, 1);
    lcd_puts("Current: ");

    lcd_putc((current_floor / 10u) + '0');
    lcd_putc((current_floor % 10u) + '0');
    lcd_puts("      ");
}

static void elevator_display_idle(void)
{
    lcd_clrscr();
    lcd_gotoxy(0, 0);
    lcd_puts("Choose floor");
    elevator_display_current_floor();
}

static void elevator_display_input(uint8_t input_value, uint8_t digit_count)
{
    lcd_clrscr();
    lcd_gotoxy(0, 0);
    lcd_puts("Floor: ");

    if (digit_count == 0u)
    {
        lcd_puts("__");
    }
    else if (digit_count == 1u)
    {
        lcd_putc(input_value + '0');
    }
    else
    {
        lcd_putc((input_value / 10u) + '0');
        lcd_putc((input_value % 10u) + '0');
    }

    lcd_gotoxy(0, 1);
    lcd_puts("#=OK *=Clear");
}

static void elevator_set_fault_same_floor(void)
{
    current_state = ELEVATOR_STATE_FAULT;

    lcd_clrscr();
    lcd_gotoxy(0, 0);
    lcd_puts("Same floor");
}

static void elevator_select_target_floor(uint8_t selected_floor)
{
    target_floor = selected_floor;

    if (target_floor == current_floor)
    {
        elevator_set_fault_same_floor();
    }
    else if (target_floor > current_floor)
    {
        current_state = ELEVATOR_STATE_GOING_UP;
    }
    else
    {
        current_state = ELEVATOR_STATE_GOING_DOWN;
    }
}

static void elevator_read_floor_input(void)
{
    uint8_t key;
    uint8_t input_value = 0u;
    uint8_t digit_count = 0u;

    elevator_display_input(input_value, digit_count);

    while (current_state == ELEVATOR_STATE_IDLE)
    {
        key = KEYPAD_GetKey();

        if (is_digit_key(key))
        {
            if (digit_count < ELEVATOR_INPUT_MAX_DIGITS)
            {
                input_value = (input_value * 10u) + (key - '0');
                digit_count++;

                if (input_value > ELEVATOR_MAX_FLOOR)
                {
                    input_value = 0u;
                    digit_count = 0u;
                }

                elevator_display_input(input_value, digit_count);
            }
        }
        else if (key == KEYPAD_CLEAR_KEY)
        {
            input_value = 0u;
            digit_count = 0u;
            elevator_display_input(input_value, digit_count);
        }
        else if (key == KEYPAD_CONFIRM_KEY)
        {
            if (digit_count > 0u)
            {
                elevator_select_target_floor(input_value);
            }
        }
    }
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
    if (current_state == ELEVATOR_STATE_IDLE)
    {
        elevator_read_floor_input();
    }
}