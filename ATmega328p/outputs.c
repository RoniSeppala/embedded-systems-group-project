#include <avr/io.h>
#include <stdint.h>

#include "board_config.h"
#include "outputs.h"
#include "timer1.h"
#include "tune.h"

// melody handling
#define OUTPUTS_UPDATE_TICK_MS (25u)

#define OBSTACLE_BLINK_TOGGLE_TICKS (8u)
#define OBSTACLE_BLINK_TOGGLE_COUNT (6u)

#define OBSTACLE_MELODY_LENGTH (5u)

static const note_t obstacle_melody[OBSTACLE_MELODY_LENGTH] =
{
    { C3, QUARTER },
    { E3, QUARTER },
    { G3, QUARTER },
    { E3, QUARTER },
    { C3, QUARTER }
};

//variables
static uint8_t buzzer_active = 0u;
static uint8_t melody_index = 0u;
static uint16_t melody_ticks_remaining = 0u;

static uint8_t obstacle_blink_active = 0u;
static uint8_t obstacle_blink_toggle_count = 0u;
static uint8_t obstacle_blink_ticks_remaining = 0u;

// helpers
static uint16_t outputs_ms_to_ticks(uint16_t milliseconds)
{
    uint16_t ticks;

    ticks = milliseconds / OUTPUTS_UPDATE_TICK_MS;

    if (ticks == 0u)
    {
        ticks = 1u;
    }

    return ticks;
}

static void outputs_obstacle_led_off(void)
{
    OBSTACLE_LED_PORT &= ~(1 << OBSTACLE_LED_PIN);
}

static void outputs_obstacle_led_toggle(void)
{
    OBSTACLE_LED_PORT ^= (1 << OBSTACLE_LED_PIN);
}

static void outputs_start_melody(void)
{
    buzzer_active = 1u;
    melody_index = 0u;
    melody_ticks_remaining = outputs_ms_to_ticks(obstacle_melody[melody_index].duration_ms);

    timer1_set_frequency(obstacle_melody[melody_index].frequency_hz);
    timer1_channel_A_on();
}

static void outputs_start_obstacle_blink(void)
{
    obstacle_blink_active = 1u;
    obstacle_blink_toggle_count = 0u;
    obstacle_blink_ticks_remaining = OBSTACLE_BLINK_TOGGLE_TICKS;

    outputs_obstacle_led_off();
}

// functions
void outputs_init(void)
{
    MOVEMENT_LED_DDR |= (1 << MOVEMENT_LED_PIN);
    DOOR_OPENING_LED_DDR |= (1 << DOOR_OPENING_LED_PIN);
    DOOR_CLOSING_LED_DDR |= (1 << DOOR_CLOSING_LED_PIN);
    OBSTACLE_LED_DDR |= (1 << OBSTACLE_LED_PIN);
    BUZZER_DDR |= (1 << BUZZER_PIN);

    setup_timer1();

    outputs_all_off();
}

void outputs_all_off(void)
{
    MOVEMENT_LED_PORT &= ~(1 << MOVEMENT_LED_PIN);
    DOOR_OPENING_LED_PORT &= ~(1 << DOOR_OPENING_LED_PIN);
    DOOR_CLOSING_LED_PORT &= ~(1 << DOOR_CLOSING_LED_PIN);
    OBSTACLE_LED_PORT &= ~(1 << OBSTACLE_LED_PIN);
    BUZZER_PORT &= ~(1 << BUZZER_PIN);

    buzzer_active = 0u;
    melody_index = 0u;
    melody_ticks_remaining = 0u;

    obstacle_blink_active = 0u;
    obstacle_blink_toggle_count = 0u;
    obstacle_blink_ticks_remaining = 0u;

    timer1_channel_A_off();
    timer1_set_frequency(0u);
}

void outputs_set_movement(void)
{
    outputs_all_off();
    MOVEMENT_LED_PORT |= (1 << MOVEMENT_LED_PIN);
}

void outputs_set_door_opening(void)
{
    outputs_all_off();
    DOOR_OPENING_LED_PORT |= (1 << DOOR_OPENING_LED_PIN);
}

void outputs_set_door_closing(void)
{
    outputs_all_off();
    DOOR_CLOSING_LED_PORT |= (1 << DOOR_CLOSING_LED_PIN);
}

void outputs_set_obstacle(void)
{
    outputs_all_off();
    outputs_start_obstacle_blink();
    outputs_start_melody();
}

void outputs_stop_buzzer(void)
{
    buzzer_active = 0u;
    melody_index = 0u;
    melody_ticks_remaining = 0u;

    timer1_channel_A_off();
    timer1_set_frequency(0u);
}

void outputs_update(void)
{
    if (obstacle_blink_active)
    {
        if (obstacle_blink_ticks_remaining > 0u)
        {
            obstacle_blink_ticks_remaining--;
        }
        else
        {
            outputs_obstacle_led_toggle();
            obstacle_blink_toggle_count++;

            if (obstacle_blink_toggle_count >= OBSTACLE_BLINK_TOGGLE_COUNT)
            {
                obstacle_blink_active = 0u;
                outputs_obstacle_led_off();
            }
            else
            {
                obstacle_blink_ticks_remaining = OBSTACLE_BLINK_TOGGLE_TICKS;
            }
        }
    }

    if (buzzer_active)
    {
        if (melody_ticks_remaining > 0u)
        {
            melody_ticks_remaining--;
        }
        else
        {
            melody_index++;

            if (melody_index >= OBSTACLE_MELODY_LENGTH)
            {
                melody_index = 0u;
            }

            melody_ticks_remaining = outputs_ms_to_ticks(obstacle_melody[melody_index].duration_ms);

            timer1_set_frequency(obstacle_melody[melody_index].frequency_hz);
            timer1_channel_A_on();
        }
    }
}