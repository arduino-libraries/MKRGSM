/*
   SerialGSMPassthrough sketch

   This sketch allows you to send AT commands from the USB CDC serial port
   of the MKR GSM 1400 board to the onboard ublox SARA-U201 celluar module.

   For a list of supported AT commands see:
   https://www.u-blox.com/sites/default/files/u-blox-CEL_ATCommands_%28UBX-13002752%29.pdf

   Circuit:
   - MKR GSM 1400 board
   - Antenna
   - 1500 mAh or higher lipo battery connected
   - SIM card

   Make sure the Serial Monitor's line ending is set to "Both NL and CR"

   create 11 December 2017
   Sandeep Mistry
*/

// baud rate used for both Serial ports
unsigned long baud = 115200;

void setup() {
  // reset the ublox module
  pinMode(GSM_RESETN, OUTPUT);
  digitalWrite(GSM_RESETN, HIGH);
  delay(100);
  digitalWrite(GSM_RESETN, LOW);

  Serial.begin(baud);
  SerialGSM.begin(baud);
}

void loop() {
  if (Serial.available()) {
    SerialGSM.write(Serial.read());
  }

  if (SerialGSM.available()) {
    Serial.write(SerialGSM.read());
  }
}

