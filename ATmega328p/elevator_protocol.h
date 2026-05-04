#ifndef ELEVATOR_PROTOCOL_H
#define ELEVATOR_PROTOCOL_H

/*
 * Shared one-byte I2C command protocol.
 *
 * The Arduino Mega is the I2C master and sends one command byte at a time.
 * The Arduino Uno is the I2C slave and reacts to the received command by changing
 * LED and buzzer outputs.
 *
 * The Uno does not know the current floor, target floor, or elevator state.
 * Those are controlled only by the Mega.
 */

// I2C slave address used by the Arduino Uno.
#define ELEVATOR_I2C_ADDRESS (0b1010111)

// Output commands sent from Mega to Uno.
#define ELEVATOR_CMD_ALL_OFF        '0'    // Turn off all LEDs and buzzer.
#define ELEVATOR_CMD_MOVING         'M'    // Turn on movement LED.
#define ELEVATOR_CMD_DOOR_OPENING   'O'    // Turn on door-opening LED.
#define ELEVATOR_CMD_DOOR_CLOSING   'C'    // Turn on door-closing LED.
#define ELEVATOR_CMD_OBSTACLE       'X'    // Start obstacle LED blinking and buzzer melody.
#define ELEVATOR_CMD_BUZZER_STOP    'S'    // Stop buzzer melody.

#endif