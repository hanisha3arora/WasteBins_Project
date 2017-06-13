from machine import UART
from network import WLAN
import os
uart = UART(0,100)
os.dupterm(uart)
