from network import LoRa
import socket
import time
import binascii


#initialize LoRa in LoRaWAN mode
lora = LoRa(mode=LoRa.LORAWAN)

#Dev EUI
#70 B3 D5 49 9D E9 17 33 

app_eui = binascii.unhexlify('70 B3 D5 7E F0 00 3D D7'.replace(' ',''))
app_key = binascii.unhexlify('9E 36 38 FD D0 C0 C9 B4 D7 A2 B1 95 19 1C FA DC'.replace(' ',''))

# join a network using OTAA(Over the Air Activation)
lora.join(activation=LoRa.OTAA, auth=(app_eui, app_key), timeout=0)

#wait until the moduel has joined the network
while not lora.has_joined():
    time.sleep(2.5)
    print('Not yet joined...')

#create a LoRA socket
s = socket.socket(socket.AF_LORA, socket.SOCK_RAW)

#set the LoRaWAN data rate
s.setsockopt(socket.SOL_LORA, socket.SO_DR, 5)


#make the socket blocked
# (waits for the data to be send and for the 2 receive windows to expire)
s.setblocking(True)

#send some data
s.send(bytes([0x01, 0x02, 0x03]))

#make the socket non-locking
# (because if there is no data received it will block forever....)
s.setblocking(False)

# get any data received (if any...)
data = s.recv(64)
print(data)
