/*
 GSM Web Server

 A simple web server that shows the value of the analog input pins.
 using a MKR GSM 1400 board.

 Circuit:
 * MKR GSM 1400 board
 * Antenna
 * Analog inputs attached to pins A0 through A5 (optional)

 created 8 Mar 2012
 by Tom Igoe
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
GPRS gprs;
GSM gsmAccess;     // include a 'true' parameter for debug enabled
GSMServer server(80); // port 80 (http default)

// timeout
const unsigned long __TIMEOUT__ = 10 * 1000;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // connection state
  bool connected = false;

  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (!connected) {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("Connected to GPRS network");

  // start server
  server.begin();

  //Get IP.
  IPAddress LocalIP = gprs.getIPAddress();
  Serial.println("Server IP address=");
  Serial.println(LocalIP);
}

void loop() {


  // listen for incoming clients
  GSMClient client = server.available();



  if (client) {
    while (client.connected()) {
      if (client.available()) {
        Serial.println("Receiving request!");
        bool sendResponse = false;
        while (int c = client.read()) {
          if (c == -1) {
            break;
          } else if (c == '\n') {
            sendResponse = true;
          }
        }

        // if you've gotten to the end of the line (received a newline
        // character)
        if (sendResponse) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(analogRead(analogChannel));
            client.println("<br />");
          }
          client.println("</html>");
          //necessary delay
          delay(1000);
          client.stop();
        }
      }
    }
  }
}

