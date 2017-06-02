from machine import UART
import os
uart = UART(0, 115200)
os.dupterm(uart)

wlan = WLAN(mode=WLAN.STA)
wlan.scan()

wlan.connect(ssid='StartGarden-Members', auth=(WLAN.WPA2, 'GeorgesDoriot'))

while not wlan.isconnected():
    pass

print(wlan.ifconfig())
