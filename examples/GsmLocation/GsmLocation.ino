/*
  Location

  This sketch uses the celluar network to determine the location of a MKR GSM 1400 board
  and prints it to the Serial monitor. The location is  based on the cellular towers in
  range, and requires a GPRS data connection to be enabled.

  Circuit:
   MKR GSM 1400 board
   Antenna
   SIM card with a data plan

  created 15 Dec 2017
  by Sandeep Mistry
*/

// libraries
#include <MKRGSM.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[]     = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[]      = SECRET_GPRS_APN;
const char GPRS_LOGIN[]    = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

// initialize the library instance
GSMLocation location;
GPRS gprs;
GSM gsmAccess;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Starting GSM location.");
  // connection state
  bool connected = false;

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (!connected) {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  location.begin();
}

void loop() {
  if (location.available()) {
    Serial.print("Location: ");
    Serial.print(location.latitude(), 7);
    Serial.print(", ");
    Serial.println(location.longitude(), 7);

    Serial.print("Altitude: ");
    Serial.print(location.altitude());
    Serial.println("m");

    Serial.print("Accuracy: +/- ");
    Serial.print(location.accuracy());
    Serial.println("m");

    Serial.println();
  }
}

