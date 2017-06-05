import machine
from machine import Timer
import pyb

class Ultrasonic:
    def __init__(self, tPin, ePin):
        self.triggerPin = tPin
        self.echoPin = ePin

        # initializing trigger output pin
        self.trigger = pyb.Pin(self.triggerPin)
        self.trigger.init(pyb.Pin.OUT_PP, pyb.Pin.PULL_NONE)
        self.trigger.low()

        #Initializing echo input pin
        self.echo = pyb.Pin(self.echoPin)
        self.echo.init(pyb.Pin.IN, pyb.Pin.PULL_NONE)

    def distance_in_inches(self):
        return (self.distance_in_cm() * 0.3937)

    def distance_in_cm(self):
        start = 0
        end = 0

        #creating the microsecond counter
        micros = pyb.Timer(2, prescaler = 83, period = 0x3fffffff)
        micros.counter(0)

        #where things go wrong
        #sending the 10us pulse
        self.trigger.high()
        pyb.udelay(10)
        self.trigger.low()

        #wait until the pulse starts
        while self.echo.value() == 0:
            start = micros.counter()

        #wait till pulse ends
        while self.echo.value() == 0:
            end = micros.counter()

        micros.deinit()

        #calculating the duration of pulse and get the distance in centimeters
        distance_in_cm = ((end - start) / 2) / 29

        return distance_in_cm
