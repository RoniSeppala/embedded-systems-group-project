#include <avr/io.h>

#include "board_config.h"
#include "outputs.h"

void outputs_init(void)
{
    MOVEMENT_LED_DDR |= (1 << MOVEMENT_LED_PIN);
    DOOR_OPENING_LED_DDR |= (1 << DOOR_OPENING_LED_PIN);
    DOOR_CLOSING_LED_DDR |= (1 << DOOR_CLOSING_LED_PIN);
    OBSTACLE_LED_DDR |= (1 << OBSTACLE_LED_PIN);
    BUZZER_DDR |= (1 << BUZZER_PIN);

    outputs_all_off();
}

void outputs_all_off(void)
{
    MOVEMENT_LED_PORT &= ~(1 << MOVEMENT_LED_PIN);
    DOOR_OPENING_LED_PORT &= ~(1 << DOOR_OPENING_LED_PIN);
    DOOR_CLOSING_LED_PORT &= ~(1 << DOOR_CLOSING_LED_PIN);
    OBSTACLE_LED_PORT &= ~(1 << OBSTACLE_LED_PIN);
    BUZZER_PORT &= ~(1 << BUZZER_PIN);
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
    OBSTACLE_LED_PORT |= (1 << OBSTACLE_LED_PIN);
    BUZZER_PORT |= (1 << BUZZER_PIN);
}

void outputs_stop_buzzer(void)
{
    BUZZER_PORT &= ~(1 << BUZZER_PIN);
}