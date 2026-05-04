#include <avr/io.h>
#include <stdint.h>

#include "board_config.h"
#include "bit_ops.h"
#include "outputs.h"
#include "timer1.h"
#include "tune.h"

// melody handling
#define OUTPUTS_UPDATE_TICK_MS (25u)

#define OBSTACLE_BLINK_TOGGLE_TICKS (8u)
#define OBSTACLE_BLINK_TOGGLE_COUNT (6u)

static const note_t obstacle_melody[] =
{
    { C3, QUARTER },
    { E3, QUARTER },
    { G3, QUARTER },
    { E3, QUARTER },
    { C3, QUARTER }
};

#define OBSTACLE_MELODY_LENGTH ((uint8_t)(sizeof(obstacle_melody) / sizeof(obstacle_melody[0])))

static const note_t background_jingle[] =
{
    { C3, QUARTER },
    { E3, QUARTER },
    { G3, QUARTER },
    { C4, QUARTER }
};

#define BACKGROUND_JINGLE_LENGTH ((uint8_t)(sizeof(background_jingle) / sizeof(background_jingle[0])))

//variables
static uint8_t obstacle_melody_active = 0u;
static uint8_t obstacle_melody_index = 0u;
static uint16_t obstacle_melody_ticks_remaining = 0u;

static uint8_t jingle_active = 0u;
static uint8_t jingle_index = 0u;
static uint16_t jingle_ticks_remaining = 0u;

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
    CLEAR_BIT(OBSTACLE_LED_PORT, OBSTACLE_LED_PIN);
}

static void outputs_obstacle_led_toggle(void)
{
    TOGGLE_BIT(OBSTACLE_LED_PORT, OBSTACLE_LED_PIN);
}

static void outputs_start_obstacle_melody(void)
{
    obstacle_melody_active = 1u;
    obstacle_melody_index = 0u;
    obstacle_melody_ticks_remaining = outputs_ms_to_ticks(obstacle_melody[obstacle_melody_index].duration_ms);

    timer1_set_frequency(obstacle_melody[obstacle_melody_index].frequency_hz);
    timer1_channel_A_on();
}

static void outputs_start_obstacle_blink(void)
{
    obstacle_blink_active = 1u;
    obstacle_blink_toggle_count = 0u;
    obstacle_blink_ticks_remaining = OBSTACLE_BLINK_TOGGLE_TICKS;

    outputs_obstacle_led_off();
}

static void outputs_start_current_jingle_note(void)
{
    if (jingle_index >= BACKGROUND_JINGLE_LENGTH)
    {
        jingle_index = 0u;
    }

    jingle_ticks_remaining = outputs_ms_to_ticks(background_jingle[jingle_index].duration_ms);

    timer1_set_frequency(background_jingle[jingle_index].frequency_hz);
    timer1_channel_A_on();
}

static void outputs_advance_obstacle_melody(void)
{
    if (obstacle_melody_ticks_remaining > 0u)
    {
        obstacle_melody_ticks_remaining--;
    }
    else
    {
        obstacle_melody_index++;

        if (obstacle_melody_index >= OBSTACLE_MELODY_LENGTH)
        {
            obstacle_melody_index = 0u;
        }

        obstacle_melody_ticks_remaining = outputs_ms_to_ticks(obstacle_melody[obstacle_melody_index].duration_ms);

        timer1_set_frequency(obstacle_melody[obstacle_melody_index].frequency_hz);
        timer1_channel_A_on();
    }
}

static void outputs_advance_jingle(void)
{
    if (jingle_ticks_remaining > 0u)
    {
        jingle_ticks_remaining--;
    }
    else
    {
        jingle_index++;

        if (jingle_index >= BACKGROUND_JINGLE_LENGTH)
        {
            jingle_index = 0u;
        }

        outputs_start_current_jingle_note();
    }
}

// functions
void outputs_init(void)
{
    SET_BIT(MOVEMENT_LED_DDR, MOVEMENT_LED_PIN);
    SET_BIT(DOOR_OPENING_LED_DDR, DOOR_OPENING_LED_PIN);
    SET_BIT(DOOR_CLOSING_LED_DDR, DOOR_CLOSING_LED_PIN);
    SET_BIT(OBSTACLE_LED_DDR, OBSTACLE_LED_PIN);
    SET_BIT(BUZZER_DDR, BUZZER_PIN);

    setup_timer1();

    outputs_all_off();
    outputs_start_jingle(); //comment to remove jingle
}

void outputs_all_off(void)
{
    uint8_t obstacle_melody_was_active = obstacle_melody_active;

    CLEAR_BIT(MOVEMENT_LED_PORT, MOVEMENT_LED_PIN);
    CLEAR_BIT(DOOR_OPENING_LED_PORT, DOOR_OPENING_LED_PIN);
    CLEAR_BIT(DOOR_CLOSING_LED_PORT, DOOR_CLOSING_LED_PIN);
    CLEAR_BIT(OBSTACLE_LED_PORT, OBSTACLE_LED_PIN);
    CLEAR_BIT(BUZZER_PORT, BUZZER_PIN);

    obstacle_melody_active = 0u;
    obstacle_melody_index = 0u;
    obstacle_melody_ticks_remaining = 0u;

    obstacle_blink_active = 0u;
    obstacle_blink_toggle_count = 0u;
    obstacle_blink_ticks_remaining = 0u;

    if (jingle_active)
    {
        if (obstacle_melody_was_active)
        {
            outputs_start_current_jingle_note();
        }
    }
    else
    {
        timer1_channel_A_off();
        timer1_set_frequency(0u);
    }
}

void outputs_set_movement(void)
{
    outputs_all_off();
    SET_BIT(MOVEMENT_LED_PORT, MOVEMENT_LED_PIN);
}

void outputs_set_door_opening(void)
{
    outputs_all_off();
    SET_BIT(DOOR_OPENING_LED_PORT, DOOR_OPENING_LED_PIN);
}

void outputs_set_door_closing(void)
{
    outputs_all_off();
    SET_BIT(DOOR_CLOSING_LED_PORT, DOOR_CLOSING_LED_PIN);
}

void outputs_set_obstacle(void)
{
    outputs_all_off();
    outputs_start_obstacle_blink();
    outputs_start_obstacle_melody();
}

void outputs_stop_buzzer(void)
{
    obstacle_melody_active = 0u;
    obstacle_melody_index = 0u;
    obstacle_melody_ticks_remaining = 0u;

    if (jingle_active)
    {
        outputs_start_current_jingle_note();
    }
    else
    {
        timer1_channel_A_off();
        timer1_set_frequency(0u);
    }
}

void outputs_start_jingle(void)
{
    jingle_active = 1u;
    jingle_index = 0u;
    jingle_ticks_remaining = 0u;

    if (!obstacle_melody_active)
    {
        outputs_start_current_jingle_note();
    }
}

void outputs_stop_jingle(void)
{
    jingle_active = 0u;
    jingle_index = 0u;
    jingle_ticks_remaining = 0u;

    if (!obstacle_melody_active)
    {
        timer1_channel_A_off();
        timer1_set_frequency(0u);
    }
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

    if (obstacle_melody_active)
    {
        outputs_advance_obstacle_melody();
    }
    else if (jingle_active)
    {
        outputs_advance_jingle();
    }
}