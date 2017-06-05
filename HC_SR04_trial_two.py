import pyb
import ultrasonic

#settign pins to accomodate Ultrasonic Senosr
sensor1_trigPin = pyb.Pin.board.X3
sensor1_echoPin = pyb.Pin.board.X4
#sensor2_trigPin = pyb.Pin.board.Y3
#sensor2_echoPin = pyb.Pin.board.Y4
#sensor2_trigPin = Pin("G16")

#sensor needs 5V and ground to be conected to pyboard's ground

#creatign two Ultrsonic Objects using the above pin configuration
sensor1 = ultrasonic.Ultrasonic(sensor1_trigPin, sensor1_echoPin)
#sensor2 = ultrasonic.Ultrasonic(sensor2_trigPin, sensor2_echoPin)

#using USR switch to print the sensor's values when pressed
switch = pyb.Switch()

#print each sensor's distance
def print_sensor_values():
    #get sensor1's distance in cm
    distance_cm = sensor1.distance_in_cm()

    #get sensor2's distance in inches
    distance_inches = sensor2.distance_in_inches()

    print("Sensor 1 (Metric System)", distance_cm, "cm")
    print("Sensor 2 (Imperial System)", distance_inches, "inches")

    #prints values every second
    while True:
        print_sensor_values()
        pyb.delay(1000)
