/*******************************************************************************
   Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman

   Permission is hereby granted, free of charge, to anyone
   obtaining a copy of this document and accompanying files,
   to do whatever they want with them without any restriction,
   including, but not limited to, copying, modification and redistribution.
   NO WARRANTY OF ANY KIND IS PROVIDED.

   This example will send Temperature and Humidity
   using frequency and encryption settings matching those of
   the The Things Network. Application will 'sleep' 7x8 seconds (56 seconds)

   This uses OTAA (Over-the-air activation), where where a DevEUI and
   application key is configured, which are used in an over-the-air
   activation procedure where a DevAddr and session keys are
   assigned/generated for use with all further communication.

   Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
   g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
   violated by this sketch when left running for longer)!
   To use this sketch, first register your application and device with
   the things network, to set or generate an AppEUI, DevEUI and AppKey.
   Multiple devices can use the same AppEUI, but each device has its own
   DevEUI and AppKey.

   Do not forget to define the radio type correctly in config.h.

 *******************************************************************************/

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "LowPower.h"
#include "I2C.h"
#include <Arduino.h>
#include "QuickStats.h"


//CayenneLPP lpp(uint8_t size);
//TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);
int sleepcycles = 7;  // every sleepcycle will last 8 secs, total sleeptime will be sleepcycles * 8 sec
float distance = 0;
bool joined = false;
bool sleeping = false;
const int pwPin1 = 9;
#define LedPin 2     // pin 13 LED is not used, because it is connected to the SPI port
#define MAX_DISTANCE 450

long pulse1, sensor1, feet;
const int MAX_NUM = 40;
int current_num = 0;
float arr[MAX_NUM];
QuickStats stats;

static uint8_t mydata[] = "Hello";
// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.

static const u1_t DEVEUI[8]  = { 0xF4, 0x89, 0xA3, 0xAD, 0x38, 0x8A, 0x4E, 0x00};
static const u1_t APPEUI[8] = { 0x4D, 0x5B, 0x00, 0xF0, 0x7E, 0xD5, 0xB3, 0x70};

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
static const u1_t APPKEY[16] = {0xA9, 0x1E, 0xB5, 0x6D, 0xA0, 0xB3, 0x1C, 0xD0, 0xF0, 0x08, 0x47, 0xD7, 0xE3, 0xFE, 0x61, 0xBF};

void os_getArtEui (u1_t* buf) {
  memcpy(buf, APPEUI, 8);
}

// provide DEVEUI (8 bytes, LSBF)
void os_getDevEui (u1_t* buf) {
  memcpy(buf, DEVEUI, 8);
}

// provide APPKEY key (16 bytes)
void os_getDevKey (u1_t* buf) {
  memcpy(buf, APPKEY, 16);
}

static osjob_t sendjob;
static osjob_t initjob;

// Pin mapping is hardware specific.
// Pin mapping Doug Larue PCB
const lmic_pinmap lmic_pins = {
  .nss = 8,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 4,
  .dio = {7, 6, LMIC_UNUSED_PIN},
};

void onEvent (ev_t ev) {
  int i, j;
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      // Disable link check validation (automatically enabled
      // during join, but not supported by TTN at this time).
      LMIC_setLinkCheckMode(0);
      digitalWrite(LedPin, LOW);
      // after Joining a job with the values will be sent.
      joined = true;
      break;
    case EV_RFU1:
      Serial.println(F("EV_RFU1"));
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      // Re-init
      os_setCallback(&initjob, initfunc);
      break;
    case EV_TXCOMPLETE:
      sleeping = true;
      if (LMIC.dataLen) {
        // data received in rx slot after tx
        // if any data received, a LED will blink
        // this number of times, with a maximum of 10
        Serial.print(F("Data Received: "));
        Serial.println(LMIC.frame[LMIC.dataBeg], HEX);
        i = (LMIC.frame[LMIC.dataBeg]);
        // i (0..255) can be used as data for any other application
        // like controlling a relay, showing a display message etc.
        if (i > 10) {
          i = 10;   // maximum number of BLINKs
        }
        for (j = 0; j < i; j++)
        {
          digitalWrite(LedPin, HIGH);
          delay(200);
          digitalWrite(LedPin, LOW);
          delay(400);
        }
      }
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      delay(50);  // delay to complete Serial Output before Sleeping

      // Schedule next transmission
      // next transmission will take place after next wake-up cycle in main loop
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
    default:
      Serial.println(F("Unknown event"));
      break;
  }
}


void do_send(osjob_t* j) {
  Serial.println("Entered Send Job");
  delay(10000);
  byte buffer[2];
  float distance;
  uint8_t d_value;
  distance = modeFunc();
  if (distance >= 200) {
    const char* message = "Empty";
    int framelength = strlen(message);
    for (int i = 0; i < framelength; ++i) {
      LMIC.frame[i] = message[i];
    }
    int result = LMIC_setTxData2(1, LMIC.frame, framelength, 0);
  }

  Serial.print("Distance: ");
  Serial.println(distance);
  d_value = int8_t(distance);
  buffer[0] = d_value & 0xFF; //high
  buffer[1] = d_value >> 8; //low

  //Check if there is a currecnt TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, (uint8_t*) buffer, 1 , 0);
    Serial.println(F("Sending: "));
  }

}
// initial job
static void initfunc (osjob_t* j) {
  // reset MAC state
  LMIC_reset();
  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);

  // start joining
  LMIC_startJoining();
  // init done - onEvent() callback will be invoked...
}

static const byte SLEEPCMD[19] = {
  0xAA, // head
  0xB4, // command id
  0x06, // data byte 1
  0x01, // data byte 2 (set mode)
  0x00, // data byte 3 (sleep)
  0x00, // data byte 4
  0x00, // data byte 5
  0x00, // data byte 6
  0x00, // data byte 7
  0x00, // data byte 8
  0x00, // data byte 9
  0x00, // data byte 10
  0x00, // data byte 11
  0x00, // data byte 12
  0x00, // data byte 13
  0xFF, // data byte 14 (device id byte 1)
  0xFF, // data byte 15 (device id byte 2)
  0x05, // checksum
  0xAB  // tail
};


void wakeup() {
  Serial.println(F("wake "));
  Serial1.write(0x01);
  Serial1.flush();
}

void sleep() {
  Serial.println(F("sleep "));
  for (uint8_t i = 0; i < 19; i++) {
    Serial1.write(SLEEPCMD[i]);
  }
  Serial1.flush();
  while (Serial1.available() > 0) {
    Serial1.read();
  }
}
void read_sensor() {
  pulse1 = pulseIn(pwPin1, HIGH); //inches
  sensor1 = pulse1 / 147; //total inches
  feet = sensor1 / 12;
  arr[current_num] = sensor1;
  current_num++;
}

float modeFunc() {
  float modeInches = stats.mode(arr, MAX_NUM, .000001);
  float finalInches = 0;
  int modeFeet = modeInches / 12;
  finalInches = fmod(modeInches, 12.0);
  /* Serial.print("Total Mode Inches: ");
    Serial.print(modeInches);
    Serial.print(' ');
    Serial.print("Feet- ");
    Serial.print(modeFeet);
    Serial.print(' ');
    Serial.print("Inches- ");
    Serial.println(finalInches);
  */
  return modeInches;
}


void setup() {
  delay(1000);
  Serial.begin(9600);
  pinMode(pwPin1, INPUT);
  delay(250);
  Serial.println(F("Starting"));
  os_init();
  os_setCallback(&initjob, initfunc);

  do_send(&sendjob);
  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);

}

unsigned long time;
void loop() {
  // start OTAA JOIN
  if (! joined) {
    os_runloop_once();
    return;
  }
  //wakeup();
  delay(1600);
  //sleep();
  Serial.println ("3");
  //lpp.measure_distance(1, 10);
  //ttn.sendBytes(lpp.getBuffer(), lpp.getSize());
  do_send(&sendjob);

  Serial.println("2");
  sleeping = false;
  while (sleeping == false) {
    os_runloop_once();
  }
  for (int i = 0; i < 3; i++) {
    //delay(8000);
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);    //sleep 8 seconds, no upload of software possible in this time
  }
}

