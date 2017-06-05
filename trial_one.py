#Set pins for HC-SR04
import pyb
from machine import Pin

TrigPin = Pin('P0', mode=Pin.OUT, pull=None, alt=-1)
EchoPin = Pin('P4', mode=Pin.IN, pull=None, alt=-1)

#intializing the trigger and echo pins
TrigPin = Pin(0, Pin.OUT)
EchoPin = Pin(4, Pin.IN)

#seeing if we can see the pin number -- testing purposes
print(TrigPin.pin())

#declaring variables for distance and duration
float distance, duration

#trigger pin turn off
TrigPin.value(0)
pyb.udelay(2)

#trigger pin On
TrigPin.value(1)
pyb.udelay(10)

#trigger pin off
TrigPin.value(0)







"""PseudoCode

1. initialize the pins
2. declare variables for distance and duration
3. set the trigger pin off
4. delay for 2 microseconds
5. set the trigger pin on
6. pause for 10 microseconds
7. set the trigger pin off again
8. read in the duration (something similar to pulseIN)
9. find the distance
10. messages for different distances, pause program after every
    message loop
11. display the data with print statements
12. delay(2000) - pauses the program for specified amount of time
