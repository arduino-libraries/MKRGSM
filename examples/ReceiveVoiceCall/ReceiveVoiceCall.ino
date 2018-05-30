/*
 Receive Voice Call

 This sketch, for the MKR GSM 1400 board, receives voice calls,
 displays the calling number, waits a few seconds then hangs up.

 Circuit:
 * MKR GSM 1400 board
 * Antenna
 * SIM card that can accept voice calls

 created Mar 2012
 by Javier Zorzano
*/

// Include the GSM library
#include <MKRGSM.h>

#include "arduino_secrets.h" 
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instance
GSM gsmAccess;
GSMVoiceCall vcs;

// Array to hold the number for the incoming call
char numtel[20];

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Receive Voice Call");

  // connection state
  bool connected = false;

  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (!connected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  // This makes sure the modem correctly reports incoming events
  vcs.hangCall();

  Serial.println("Waiting for a call");
}

void loop() {
  // Check the status of the voice call
  switch (vcs.getvoiceCallStatus()) {
    case IDLE_CALL: // Nothing is happening

      break;

    case RECEIVINGCALL: // Yes! Someone is calling us

      Serial.println("RECEIVING CALL");

      // Retrieve the calling number
      vcs.retrieveCallingNumber(numtel, 20);

      // Print the calling number
      Serial.print("Number:");
      Serial.println(numtel);

      // Answer the call, establish the call
      vcs.answerCall();
      break;

    case TALKING:  // In this case the call would be established

      Serial.println("TALKING. Press enter to hang up.");
      while (Serial.read() != '\n') {
        delay(100);
      }
      vcs.hangCall();
      Serial.println("Hanging up and waiting for the next call.");
      break;
  }
  delay(1000);
}


