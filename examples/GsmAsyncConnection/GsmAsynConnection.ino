/*
  MKR GSM 1400 Asynchronous Connection
  This sketch connects asynchronously modem to internet GSM and GPRS 
  without blocking main loop while it blinks a led. It reconnects when 
  connection is lost... try removing the antenna.
  
  Circuit:
  * MKR GSM 1400 board
  * Antenna
  * SIM card with a data plan
  * Optionaly a Led and a resistor.
  
  Created 06 Jun 2020
  By Franc Muñoz, franc@zeroworks.com
  
*/

#include <MKRGSM.h>
#include "arduino_secrets.h" 

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
const char PINNUMBER[]     = SECRET_PINNUMBER;
const char GPRS_APN[]      = SECRET_GPRS_APN;
const char GPRS_LOGIN[]    = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

// Initialize instances
GPRS gprs;
GSM gsm;

// Some variables and constants
unsigned const int LED_PIN = 6;
bool ledState = false;
unsigned long ledMillis = 0;

unsigned long blinkMillis = 500;  // To change blink cadence 
unsigned long processMillis = 0;  // Last task execution
unsigned long taskTimer = 300000; // Milliseconds to repeat task

GSM3_NetworkStatus_t lastState = (GSM3_NetworkStatus_t)0;
bool connectingGSM = false;
bool connectingGPRS = false;

// Task related
GSMClient client;
char server[] = "www.arduino.cc";
char path[] = "/asciilogo.txt";
int port = 80;

void connectGSM() {
  if(!connectingGSM) {
    Serial.println("Connecting GSM...");
    gsm.begin(PINNUMBER, true, false);  
    connectingGSM=true;
    blinkMillis=250;
  }
}

void connectGPRS() {
  if(!connectingGPRS) {
    Serial.println("Connecting GPRS...");
    gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD, false);
    connectingGPRS=true;
    blinkMillis=100;
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Asynchronous GSM and GPRS connection");
  
  pinMode(LED_PIN, OUTPUT); 
  ledMillis=millis();
  processMillis=millis()-taskTimer;

  Serial.println("Let's Start");
  
  // Start the the async connection: needed here to initialize the MODEM.
  //MODEM.debug();
  connectGSM();
  Serial.println("End Init");
}

void loop() {

  // Asynchronous gsm connection 
  if(connectingGSM) {
    GSM3_NetworkStatus_t state=gsm.status();
    switch(state) {
      case ERROR: // not connected or error.
      case IDLE: // start
      case CONNECTING: // connecting...
        if(lastState!=state) {
          Serial.print("GSM State changed: ");
          Serial.println(state);
          lastState=state;
        }
        gsm.ready();
        break;
        
      case GSM_READY: // connected, let's wait to READY_STATE_DONE
        if(gsm.ready() == 1) {
          Serial.println("GSM Connected!");
          connectingGSM=false;
          connectGPRS();         
        }
        break;
        
    }    
  } else {
    if(connectingGPRS) {
      GSM3_NetworkStatus_t state=gprs.status();
      switch(state) {
        case ERROR: // not connected or error.
          blinkMillis = 500;
          
        case IDLE: // start
        case CONNECTING: // connecting...
          if(lastState!=state) {
            Serial.print("GPRS State changed: ");
            Serial.println(state);
            lastState=state;
          }
          gprs.ready();
          break;
          
        case GPRS_READY: // connected, let's wait to GPRS_STATE_IDLE
          
          //if(gprs.ready() == 1) {
            Serial.println("GPRS Connected!");
            connectingGPRS=false; 
            blinkMillis=1000;        
          //}
          break;
      } 
    } else {
      if(!gsm.isAccessAlive()) {
        // Reconnect ...
        Serial.println("GSM not ready...");
        // Let's SARA Breath and uptate its status
        delay(500);
        Serial.print("Reconnect GSM");
        connectGSM();
      } else {
        if(gprs.status()!=GPRS_READY) {
           Serial.println("GPRS not ready...");
           // Let's SARA Breath and uptate its status
           delay(500);
           if(!gsm.isAccessAlive()) {
              Serial.print("Reconnect GSM");
              connectGSM();
           } else {
              Serial.print("Reconnect GPRS");
              connectGPRS();
           }
        } else {
          // We are connected just do something each taskTimer milliseconds.
          if(millis()-processMillis>taskTimer) {
            Serial.println("Do Inernet Tasks...");
            processMillis=millis();
            doTask();
          }
        }
      }
    }
  }

  // Blink the led ...
  if(millis()-ledMillis>blinkMillis) {
    digitalWrite(LED_PIN, ledState?HIGH:LOW);
    ledState=!ledState;
    ledMillis=millis();
  }

  // Do whatever... SARA won't block main loop!
}

void doTask() {
  // Just do something if connected.
  Serial.print("Connecting ");
  Serial.print(server);
  Serial.println("...");

  // Modified from GSMWebClient example by Tom Igoe
  if(client.connect(server, port)) {
    Serial.println("Connected!");
    // Make a HTTP request:
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("Connection failed");
  }

  while(1) {
    // if there are incoming bytes available
    // from the server, read them and print them:
    if(client.available()) {
      char c = client.read();
      Serial.print(c);
    } else {
      if(!client.connected()) {
        Serial.println();
        Serial.println("Disconnecting.");
        client.stop();
        break;
      }
    }
  }
}
