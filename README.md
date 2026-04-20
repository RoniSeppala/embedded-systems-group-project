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

# modules to do:
## to both
- critical handling + write error handling
- maybe uart for debuging
## Arduino Mega
- design mcu.h
- main switch case
- keypad input
- lcd

## Arduino Uno
- design mcu.h
- buzzer, play one melody 
- activate led

# to test
- breaking of melodies into its own file as well as the if not defined in pbm, might cause issues