#Set pins for HC-SR04
from machine import Pin

TrigPin = Pin('P0', mode=Pin.OUT, pull=None, alt=-1)
EchoPin = Pin('P4', mode=Pin.IN, pull=None, alt=-1)


#TrigPin = const(0), EchoPin = const(4)
