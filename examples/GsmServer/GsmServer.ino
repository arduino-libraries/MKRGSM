/*
    GSM Server
    A simple server that receives a command reply a response with support of http or plain data.
    using a MKR GSM 1400 board.

 Circuit:
 * MKR GSM 1400 board
 * Antenna
 * Led attached to pin D1 blinking (optional)
 * Led attached to pin D5 to indicate state (optional);
 
 created 06 Feb 2018
 by Franc Muñoz
*/

// libraries
#include <MKRGSM.h>
#include "arduino_secrets.h" 

// Please enter your sensitive data in the Secret tab or arduino_secrets.h

// PIN Number
const char GSM_PIN[]       = SECRET_GSM_PIN;

// APN data
const char GPRS_APN[]      = SECRET_GPRS_APN;
const char GPRS_LOGIN[]    = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

// Server listening port
const int SERVER_PORT      = 80;

// Pin
const int PIN_BLINK=1;
const int PIN_STATE=5;

// GSM
GPRS gprs;
GSM gsm(false); // Set to true to enable Debugging
GSMModem modem;
GSMServer server(SERVER_PORT);
IPAddress localIP;
String imei;
bool gsmStarted;

// Other Stuff
int timerLed=0;
int blinkState=0;

void connectGSM() {
    int retries=0;
    gsmStarted=false;
    while (!gsmStarted && retries<10) {
        Serial.write("Initializing GSM\n");
        if (gsm.begin(GSM_PIN) == GSM_READY) {
            Serial.write("Initializing GPRS\n");
            if(gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY) {
                gsmStarted = true;
            }
        } else {
            Serial.println("Not connected retrying.");
            digitalWrite(PIN_BLINK, 1);
            digitalWrite(PIN_STATE, 1);
            delay(1000);
        }
        retries++;
    }
    if(!gsmStarted) {
        // Reboot
        Serial.println("ERROR: Can't Initialize, should reboot.");
        
        delay(1000);
    } else {
        // Get IMEI
        imei = modem.getIMEI();
        Serial.print("Server IMEI is ");
        Serial.print(imei);
        Serial.println(".");

        // Starting Server
        server.begin();
        Serial.print("Initializing Server at port ");
        Serial.println(SERVER_PORT);
        
        localIP = gprs.getIPAddress();
        Serial.print("IP Address: ");
        Serial.println(localIP);

        // Reaching Server
        Serial.println("You can reach me at:");
        Serial.print("http://");
        Serial.print(localIP);
        Serial.print(":");
        Serial.print(SERVER_PORT);
        Serial.println("/hello");

        Serial.print("http://");
        Serial.print(localIP);
        Serial.print(":");
        Serial.print(SERVER_PORT);
        Serial.println("/bye");
    }    
}

void checkServer() {
    GSMClient client;
    bool reply = false;
    String data = "";
    int received = 0; 
    int ready;
    String response = "";
    uint8_t buffer[256];
    int avail, read;
    bool isHTTP = false;
    int s;
    
    client=server.available();
    if(client) {
        Serial.println("Request Received");
    
        // Browsers will try to retain connection open even with connection close... so we'll close it when we have the data.
        // Chrome sends up to four requests for maitain connection open.
        if(client.connected()) {           
            while(client.available()>0) {
                // Read max buffer data Leaving last byte to convert the buffer to String NULLing last byte.
                avail=client.read(&buffer[0], 255); 
    
                // Limit received data to 1024 bytes this will avoid flooding and collapsing arduino. 
                // Enough for HTTP header and command or data but limited.
                // It can be removed safely.
                while(avail>0 && received<1024) {
                    received+=avail;
                    buffer[avail]=0; // Set last char to NULL to terminate String and concat.
                    data.concat(String((char *)buffer));
                    avail=client.read(&buffer[0], 255);
                }
            }

            if(data.length()>0) {
                Serial.print("Received: ");
                Serial.print(received);
                Serial.println(" bytes.");

                if(data.startsWith("GET /")) {
                    isHTTP=true;
                    s=data.indexOf(' ', 5);
                    if(s>-1) {
                        data=data.substring(5, s);
                    }                    
                } 
                
                // Parse message
                if(data=="hello") {
                    digitalWrite(PIN_STATE, 1);
                    response="Hello!";
                } else if(data=="bye") {
                    digitalWrite(PIN_STATE, 0);
                    response="Good Bye!";
                } else {
                    response="Can't understand "+data;
                }
            }

            if(isHTTP) {
                // Send HTTP Header
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/html");
                client.println("Connection: close");
                client.println();

                // Send HTTP Data
                client.println("<html><body>");
                client.println("<h3>MKRGSM 1400 Server v0.1</h3>\n");
                if(response.length()>0) {
                    client.print("<p>");
                    client.print(response);
                    client.println("</p>");
                }
                client.println("</body></html>");
            } else {
                // Send plain response, so we can use non HTTP like fsockopen from PHP.
                client.println(response);
            }
            delay(1000); // Let data reach its destination.
            Serial.println("Response Send.");
        }
        Serial.println("Close remote client.");
        client.stop();
    }
}

void setup() {
    // Setup Serial
    Serial.begin(115200);
    delay(1000);
    Serial.write("GSM Server v0.1\n"); 

    // Setup Pins and stop leds.
    pinMode(PIN_BLINK, OUTPUT);
    pinMode(PIN_STATE, OUTPUT);

    digitalWrite(PIN_BLINK, 0);
    digitalWrite(PIN_STATE, 0);

    connectGSM();
}

void loop() {
    int cMillis=millis();
    
    if(gsmStarted) {  
        checkServer();

        // Blink Led
        if(cMillis-timerLed>=250) {
            timerLed=cMillis; // Wait next blink ...
            blinkState=!blinkState;
            digitalWrite(PIN_BLINK, blinkState);
        }
    }
}
