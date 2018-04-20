/*
  Http client POST

  This sketch connects to a website through a MKR GSM 1400 board. Specifically,
  this example downloads the URL "http://www.arduino.cc/asciilogo.txt" and
  prints it to the Serial monitor.

  Circuit:
   MKR GSM 1400 board
   Antenna
   SIM card with a data plan
*/

// libraries
#include <MKRGSM.h>
#include <ArduinoHttpClient.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[]     = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[]      = SECRET_GPRS_APN;
const char GPRS_LOGIN[]    = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;

// URL, path and port (for example: arduino.cc)
char server[] = "httpbin.org";
char path[] = "/post";
int port = 80; // port 80 is the default for HTTP

HttpClient http = HttpClient(client, server, port);

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Starting Arduino web client.");
  // connection state
  boolean connected = false;

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
  Serial.println("Connected");

}

int maxLoop = 0;

void loop() {
  http.post(path, "application/json", "{\"test\": \"something\"}");

  // read the status code and body of the response
  int statusCode = http.responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);

  String response = http.responseBody();
  Serial.print("Response: ");
  Serial.println(response);

  Serial.println("Wait five seconds (this script runs max 5 times)");
  delay(5000);

  if (++maxLoop > 5)
    while (true) {}
}
