# BL40A1812 Introduction to Embedded Systems
## functionality
Simulates an elevator which can move between floors 0 and 99.
The systems supports queueing floors like a normal elevator. # submits the current key to queue, and if elevator is not occupied, it will start moving there. If it is moving currently, or in some other action, the floor will be queued and moved to in the queue order.
Handles target floor entry through keypad, has logic based on a state machine, provides output using lcd, leds and buzzer.
keypad actions:
- enter target floor with numbers
  - confirm and add to queue with #
- use * for clearing currently typed floor input
- use * during open door to trigger obstacle detection
- D clears the full queue
action leds
- movement led
- obstacle led
- door opening led
- door closing led
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
- leds

## pre made, copied files (might include edits if permited by licence)
- keypad h and c
- lcd h and c
- stdutils.h
- delay h and c

# modules to do:
## to both
- deep sleep (maybe timer0)
  - critical handling + write error handling

### clear todo list
- sleep??
- make some bigger files into multiple smaller files ????
  - break melodies into own files

# modules
## both master and slave
| module name | description | notes |
|-|-|-|
| main.c | houses inits and loop | |
| mcu.h | houses mcu related definitions | |
| board_config.h | does pin mapping | |
| bit_ops | provides functions for bit operations | |
| uart | allows uart communication | mainly for debuging, allows printf, uart.c premade |
| elevator_protocol.h | defines the i2c communication between the master and slave | needs to be same on both |
| stdutils.h | neede by keypad and delay, provides definitions | premade |
| delay | provides delay functions | premade |

## master (ATmega2560, Arduino Mega)
| module name | description | notes |
|-|-|-|
| keypad | provides fuctions for using the keypad | premade |
| lcd | provides functions for using the lcd | premade |
| elevator_controller | houses main logic for elevator statemachine, lcd and keypad | |
| i2c_master | handles sending out i2c coms | |


## slave (ATmega328p, Arduino Uno)
| module name | description | notes |
|-|-|-|
| tune.h | holds the definitions for notes and playing them | |
| timer1 | provides a timer to deal with playing the notes | |
| i2c_slave | handles recieving i2c coms | |
| outputs | handles logic regarding actions after recieving i2c command from master | |

# pin mapping
## mega
### lcd
| mega pin | description | 
|-|-|
| D3 | data0/D4 |
| D4 | data1/D5 |
| D5 | data2/D6 |
| D6 | data3/D7 |
| D9 | RS |
| D10 | RW |
| D11 | E |

### keypad
| mega pin | description | 
|-|-|
| A8 | C0 |
| A9 | C1 |
| A10 | C2 |
| A11 | C3 |
| A12 | R0 |
| A13 | R1 |
| A14 | R2 |
| A15 | R3 |
### I2C
| mega pin | description(uno pin) | 
|-|-|
| D20/SDA | A4/SDA |
| D21/SCL | A5/SCL |
| GND | GND |

## uno
### LEDS
| uno pin | description | 
|-|-|
| D2 | Movement Led |
| D3 | Door Opening Led |
| D4 | Door Closing Led |
| D5 | Obstacle Led |

### Other
| uno pin | description | 
|-|-|
| D9 | Buzzer |

### I2C
| uno pin | description(mega pin) | 
|-|-|
| A4/SDA | D20/SDA | 
| A5/SCL | D21/SCL | 
| GND | GND | 

# I2C command protocol
| Command | Meaning |
|-|-|
| `'0'` | Turn off all LEDs and buzzer |
| `'M'` | Turn on movement LED |
| `'O'` | Turn on door opening LED |
| `'C'` | Turn on door closing LED |
| `'X'` | Start obstacle LED blinking and buzzer melody |
| `'S'` | Stop buzzer melody |

# AI notice
ChatGPT-5.5 Thinking Extended was used in project planning, error checking, debuging, and commenting