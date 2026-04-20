# BL40A1812 Introduction to Embedded Systems

## design
- modular code within files
# used split
- communication via I2C
## Arduino Mega (ATmega2560) - Master
- main logic
- keypad
- lcd
## Arduino Uno (ATmega328p) - Slave
- buzzer
- led

## pre made, copied files (might include edits if permited by licence)
- keypad h and c
- lcd h and c
- stdutils.h
- delay h and c

# modules to do:
## to both
- critical handling + write error handling
- maybe uart for debuging
- board configs and pin mapping
- deep sleep (maybe timer0)
## Arduino Mega
- design mcu.h
- main switch case
- keypad input
- lcd

## Arduino Uno
- design mcu.h
- buzzer, play one melody 
  - **redesign timer1 to function with ATmega328p**
- activate led

### clear todo list
- state machine (arduino mega, switch case)
- keypad functionality (arduino mega)
- lcd feedback (Arduino mega)
- microcontroller communication
- led indicators (arduino uno)
- buzzer (arduino uno)

#### done

# to test
- breaking of melodies into its own file as well as the if not defined in pbm, might cause issues

# modules
## both master and slave
| module name | description | notes |
|-|-|-|
| main.c | houses inits and loop | |
| mcu.h | houses mcu related definitions | |
| board_config.h | does pin mapping | |
| bit_ops | provides functions for bit operations | |
| uart | allows uart communication | mainly for debuging, allows printf, uart.c premade |
## master (ATmega2560, Arduino Mega)
| module name | description | notes |
|-|-|-|
| delay | provides delay functions | premade |
| keypad | provides fuctions for using the keypad | premade |
| lcd | provides functions for using the lcd | premade |
| stdutils.h | neede by keypad, provides definitions | premade |

## slave (ATmega328p, Arduino Uno)
| module name | description | notes |
|-|-|-|
| tune.h | holds the definitions for notes and playing them | |
| timer1 | provides a timer to deal with playing the notes | |
| melodies.h | holds the melody lisist | |