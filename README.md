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
  - **maybe add pin_config c and h??**

### clear todo list
- state machine (arduino mega, switch case)
- keypad functionality (arduino mega)
- lcd feedback (Arduino mega)
- microcontroller communication
- led indicators (arduino uno, maybe add pin_config c and h??)
- buzzer (arduino uno)
- queued floor requests??
- sleep??

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

# AI notice
ChatGPT-5.5 Thinking Extended was used in project planning, error checking and debuging