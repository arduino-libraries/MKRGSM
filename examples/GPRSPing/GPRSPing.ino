/*

 This uses an MKR GSM 1400 to continuously pings given host specified by IP Address or name.

Circuit:
* MKR GSM 1400 board
* Antenna
* SIM card with a data plan

 created 06 Dec 2017
 by Arturo Guadalupi
*/
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
GSMSSLClient client;
GPRS gprs;
GSM gsmAccess;

// Specify IP address or hostname
String hostName = "www.google.com";
int pingResult;

void setup() {
 // Initialize serial and wait for port to open:
 Serial.begin(9600);
 while (!Serial) {
   ; // wait for serial port to connect. Needed for native USB port only
 }

 Serial.println("Starting Arduino GPRS ping.");
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
}

void loop() {
 Serial.print("Pinging ");
 Serial.print(hostName);
 Serial.print(": ");

 pingResult = gprs.ping(hostName);

 if (pingResult >= 0) {
   Serial.print("SUCCESS! RTT = ");
   Serial.print(pingResult);
   Serial.println(" ms");
 } else {
   Serial.print("FAILED! Error code: ");
   Serial.println(pingResult);
 }

 delay(5000);
}