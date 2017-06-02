""" LoPy LoRaWAN Nano Gateway configuration options """

GATEWAY_ID = '11aa334455bb7788' #Specified n when registering gateway

SERVER = 'router.eu.thethings.network' #server address and port to forward and receive data
PORT = 1700

NTP = "pool.ntp.org" #NTP server for getting/setting time
NTP_PERIOD_S = 3600 #NTP server polling interval

WIFI_SSID = 'StartGarden-Members'
WIFI_PASS = 'GeorgesDoriot'

LORA_FREQUENCY = 868100000 #our specific regions LORA_FREQUENCY
LORA_DR = "SF7BW125"   # our specific regions LORA_DR (datarate)
