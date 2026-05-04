// general includes
#include <stdint.h>
#include "mcu.h"
#include "delay.h"

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
#define ELEVATOR_REQUEST_QUEUE_SIZE (5u)
#define KEYPAD_CONFIRM_KEY '#'
#define KEYPAD_CLEAR_KEY '*'
#define KEYPAD_CLEAR_QUEUE_KEY 'D'
#define ELEVATOR_INITIAL_FLOOR (0u)
#define ELEVATOR_FLOOR_DELAY_MS (500u)
#define ELEVATOR_DOOR_OPEN_DELAY_MS (3000u)
#define ELEVATOR_DOOR_CLOSE_DELAY_MS (2000u)
#define ELEVATOR_FAULT_DELAY_MS (1500u)

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

static uint8_t floor_queue[ELEVATOR_REQUEST_QUEUE_SIZE];
static uint8_t floor_queue_head = 0u; // head  = next item to read
static uint8_t floor_queue_tail = 0u; // tail  = next slot to write
static uint8_t floor_queue_count = 0u; // count = number of queued floors
static uint8_t live_request_input_value = 0u;
static uint8_t live_request_digit_count = 0u;

// Function prototypes
static uint8_t is_digit_key(uint8_t key);

static void elevator_lcd_print_floor(uint8_t floor);
static void elevator_display_current_floor(void);
static void elevator_display_idle(void);
static void elevator_display_input(uint8_t input_value, uint8_t digit_count);
static void elevator_display_moving(const char *direction_text);

static void elevator_delay_ms(uint16_t milliseconds);
static void elevator_delay_ms_with_request_polling(uint16_t milliseconds, uint8_t allow_star_clear);

static void elevator_set_fault_same_floor(void);
static void elevator_select_target_floor(uint8_t selected_floor);
static void elevator_read_floor_input(void);

static void elevator_clear_live_request_input(void);
static void elevator_queue_live_request(void);
static uint8_t elevator_get_new_keypress(uint8_t *key);
static void elevator_handle_live_request_key(uint8_t key, uint8_t allow_star_clear);
static void elevator_poll_for_queued_request(uint8_t allow_star_clear);

static uint8_t elevator_queue_is_empty(void);
static uint8_t elevator_queue_is_full(void);
static void elevator_queue_clear(void);
static uint8_t elevator_queue_push(uint8_t floor);
static uint8_t elevator_queue_pop(uint8_t *floor);
static uint8_t elevator_process_next_queued_floor(void);

static uint8_t elevator_wait_for_obstacle_trigger(uint16_t timeout_ms);

static void elevator_handle_idle(void);
static void elevator_handle_going_up(void);
static void elevator_handle_going_down(void);
static void elevator_handle_door_opening(void);
static void elevator_handle_door_closing(void);
static void elevator_handle_obstacle_detection(void);
static void elevator_handle_fault(void);

// helper functions
static uint8_t is_digit_key(uint8_t key)
{
    return ((key >= '0') && (key <= '9'));
}

static void elevator_delay_ms(uint16_t milliseconds)
{
    while (milliseconds > 0u)
    {
        DELAY_ms(1);
        milliseconds--;
    }
}

static void elevator_lcd_print_floor(uint8_t floor)
{
    lcd_putc((floor / 10u) + '0');
    lcd_putc((floor % 10u) + '0');
}

static void elevator_display_current_floor(void)
{
    lcd_gotoxy(0, 1);
    lcd_puts("Current: ");

    elevator_lcd_print_floor(current_floor);
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
        elevator_lcd_print_floor(input_value);
    }

    lcd_gotoxy(0, 1);
    lcd_puts("#=OK *=Clear");
}

static void elevator_set_fault_same_floor(void)
{
    current_state = ELEVATOR_STATE_FAULT;
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
        else if (key == KEYPAD_CLEAR_QUEUE_KEY)
        {
            elevator_queue_clear();
            elevator_clear_live_request_input();
            input_value = 0u;
            digit_count = 0u;
            elevator_display_input(input_value, digit_count);
        }
    }
}

static void elevator_poll_for_queued_request(uint8_t allow_star_clear)
{
    uint8_t key;

    if (elevator_get_new_keypress(&key))
    {
        elevator_handle_live_request_key(key, allow_star_clear);
    }
}

static void elevator_handle_live_request_key(uint8_t key, uint8_t allow_star_clear)
{
    if (is_digit_key(key))
    {
        if (live_request_digit_count < ELEVATOR_INPUT_MAX_DIGITS)
        {
            live_request_input_value = (live_request_input_value * 10u) + (key - '0');
            live_request_digit_count++;

            if (live_request_input_value > ELEVATOR_MAX_FLOOR)
            {
                elevator_clear_live_request_input();
            }
        }
    }
    else if (key == KEYPAD_CONFIRM_KEY)
    {
        elevator_queue_live_request();
    }
    else if ((key == KEYPAD_CLEAR_KEY) && allow_star_clear)
    {
        elevator_clear_live_request_input();
    }
    else if (key == KEYPAD_CLEAR_QUEUE_KEY)
    {
        elevator_queue_clear();
        elevator_clear_live_request_input();
    }
}

static void elevator_display_moving(const char *direction_text)
{
    lcd_clrscr();

    lcd_gotoxy(0, 0);
    lcd_puts(direction_text);

    lcd_gotoxy(0, 1);
    lcd_puts("Current: ");
    elevator_lcd_print_floor(current_floor);
    lcd_puts(" Q:");
    lcd_putc(floor_queue_count + '0');
}

static void elevator_delay_ms_with_request_polling(uint16_t milliseconds, uint8_t allow_star_clear)
{
    while (milliseconds > 0u)
    {
        elevator_poll_for_queued_request(allow_star_clear);
        DELAY_ms(1);
        milliseconds--;
    }
}

static void elevator_clear_live_request_input(void)
{
    live_request_input_value = 0u;
    live_request_digit_count = 0u;
}

static void elevator_queue_live_request(void)
{
    if (live_request_digit_count == 0u)
    {
        return;
    }

    if (elevator_queue_push(live_request_input_value))
    {
        elevator_clear_live_request_input();
    }
}

static uint8_t elevator_get_new_keypress(uint8_t *key)
{
    static uint8_t previous_key = 'z';
    uint8_t current_key;

    current_key = KEYPAD_GetCurrentKey();

    if (current_key == 'z')
    {
        previous_key = 'z';
        return 0u;
    }

    if (current_key == previous_key)
    {
        return 0u;
    }

    previous_key = current_key;
    *key = current_key;

    return 1u;
}

static uint8_t elevator_queue_is_empty(void)
{
    return (floor_queue_count == 0u);
}

static uint8_t elevator_queue_is_full(void)
{
    return (floor_queue_count >= ELEVATOR_REQUEST_QUEUE_SIZE);
}

static void elevator_queue_clear(void)
{
    floor_queue_head = 0u;
    floor_queue_tail = 0u;
    floor_queue_count = 0u;
}

static uint8_t elevator_queue_push(uint8_t floor)
{
    if (elevator_queue_is_full())
    {
        return 0u;
    }

    floor_queue[floor_queue_tail] = floor;
    floor_queue_tail++;

    if (floor_queue_tail >= ELEVATOR_REQUEST_QUEUE_SIZE)
    {
        floor_queue_tail = 0u;
    }

    floor_queue_count++;

    return 1u;
}

static uint8_t elevator_queue_pop(uint8_t *floor)
{
    if (elevator_queue_is_empty())
    {
        return 0u;
    }

    *floor = floor_queue[floor_queue_head];
    floor_queue_head++;

    if (floor_queue_head >= ELEVATOR_REQUEST_QUEUE_SIZE)
    {
        floor_queue_head = 0u;
    }

    floor_queue_count--;

    return 1u;
}

static uint8_t elevator_process_next_queued_floor(void)
{
    uint8_t next_floor;

    if (!elevator_queue_pop(&next_floor))
    {
        return 0u;
    }

    elevator_select_target_floor(next_floor);

    return 1u;
}

static uint8_t elevator_wait_for_obstacle_trigger(uint16_t timeout_ms)
{
    uint8_t key;

    while (timeout_ms > 0u)
    {
        if (elevator_get_new_keypress(&key))
        {
            if (key == KEYPAD_CLEAR_KEY)
            {
                return 1u;
            }

            elevator_handle_live_request_key(key, 0u);
        }

        DELAY_ms(1);
        timeout_ms--;
    }

    return 0u;
}

static void elevator_handle_idle(void)
{
    elevator_read_floor_input();
}

static void elevator_handle_going_up(void)
{
    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_MOVING);

    while (current_floor < target_floor)
    {
        current_floor++;
        elevator_display_moving("Going up");
        elevator_delay_ms_with_request_polling(ELEVATOR_FLOOR_DELAY_MS, 1u);
    }

    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_ALL_OFF);

    current_state = ELEVATOR_STATE_DOOR_OPENING;
}

static void elevator_handle_going_down(void)
{
    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_MOVING);

    while (current_floor > target_floor)
    {
        current_floor--;
        elevator_display_moving("Going down");
        elevator_delay_ms_with_request_polling(ELEVATOR_FLOOR_DELAY_MS, 1u);
    }

    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_ALL_OFF);

    current_state = ELEVATOR_STATE_DOOR_OPENING;
}

static void elevator_handle_door_opening(void)
{
    uint8_t obstacle_detected;

    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_DOOR_OPENING);

    lcd_clrscr();
    lcd_gotoxy(0, 0);
    lcd_puts("Door open");
    lcd_gotoxy(0, 1);
    lcd_puts("* = obstacle");

    obstacle_detected = elevator_wait_for_obstacle_trigger(ELEVATOR_DOOR_OPEN_DELAY_MS);

    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_ALL_OFF);

    if (obstacle_detected)
    {
        current_state = ELEVATOR_STATE_OBSTACLE_DETECTION;
    }
    else
    {
        current_state = ELEVATOR_STATE_DOOR_CLOSING;
    }
}

static void elevator_handle_door_closing(void)
{
    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_DOOR_CLOSING);

    lcd_clrscr();
    lcd_gotoxy(0, 0);
    lcd_puts("Door closing");

    elevator_delay_ms_with_request_polling(ELEVATOR_DOOR_CLOSE_DELAY_MS, 1u);

    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_ALL_OFF);

    if (!elevator_process_next_queued_floor())
    {
        current_state = ELEVATOR_STATE_IDLE;
        elevator_display_idle();
    }
}

static void elevator_handle_obstacle_detection(void)
{
    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_OBSTACLE);

    lcd_clrscr();
    lcd_gotoxy(0, 0);
    lcd_puts("Obstacle");
    lcd_gotoxy(0, 1);
    lcd_puts("Press any key");

    KEYPAD_GetKey();

    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_BUZZER_STOP);
    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_ALL_OFF);

    current_state = ELEVATOR_STATE_DOOR_CLOSING;
}

static void elevator_handle_fault(void)
{
    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_ALL_OFF);

    lcd_clrscr();
    lcd_gotoxy(0, 0);
    lcd_puts("Same floor");

    elevator_delay_ms(ELEVATOR_FAULT_DELAY_MS);

    if (!elevator_process_next_queued_floor())
    {
        current_state = ELEVATOR_STATE_IDLE;
        elevator_display_idle();
    }
}

void elevator_controller_init(void)
{
    current_state = ELEVATOR_STATE_IDLE;
    current_floor = ELEVATOR_INITIAL_FLOOR;
    target_floor = ELEVATOR_INITIAL_FLOOR;
    elevator_queue_clear();
    elevator_clear_live_request_input();

    i2c_master_init();
    i2c_master_send_byte(ELEVATOR_I2C_ADDRESS, ELEVATOR_CMD_ALL_OFF);

    KEYPAD_Init();

    lcd_init(LCD_DISP_ON);
    elevator_display_idle();
}

// elevator logic
void elevator_controller_run(void)
{
    switch (current_state)
    {
        case ELEVATOR_STATE_IDLE:
            elevator_handle_idle();
            break;

        case ELEVATOR_STATE_GOING_UP:
            elevator_handle_going_up();
            break;

        case ELEVATOR_STATE_GOING_DOWN:
            elevator_handle_going_down();
            break;

        case ELEVATOR_STATE_DOOR_OPENING:
            elevator_handle_door_opening();
            break;

        case ELEVATOR_STATE_DOOR_CLOSING:
            elevator_handle_door_closing();
            break;

        case ELEVATOR_STATE_OBSTACLE_DETECTION:
            elevator_handle_obstacle_detection();
            break;

        case ELEVATOR_STATE_FAULT:
            elevator_handle_fault();
            break;

        default:
            current_state = ELEVATOR_STATE_IDLE;
            elevator_display_idle();
            break;
    }
}