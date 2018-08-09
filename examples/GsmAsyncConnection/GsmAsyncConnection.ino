/*
  Asynchronous Connection
  This sketch connects asynchronously modem to internet GSM and GPRS 
  without blocking main loop while it blinks a led.

  Circuit:
  * MKR GSM 1400 board
  * Antenna
  * SIM card with a data plan
  * Optionaly a Led and a resistor.

  Created 8 Aug 2018
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

const int LED_PIN = 6;
bool ledState = false;
int ledMillis = 0;
int processMillis = 0;
GSM3_NetworkStatus_t lastState = (GSM3_NetworkStatus_t)0;
bool connectingGSM = false;
bool connectingGPRS = false;

void connectGSM() {
  if(!connectingGSM) {
    Serial.println("Connecting GSM...");
    gsm.begin(PINNUMBER, true, false);  
    connectingGSM=true;
  }
}

void connectGPRS() {
  if(!connectingGPRS) {
    Serial.println("Connecting GPRS...");
    gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD, false);
    connectingGPRS=true;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Asynchronous GSM and GPRS connection");
  
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT); 
  ledMillis=millis();
  processMillis=millis();
  
  // Start the the async connection: needed here to initialize the MODEM.
  //MODEM.debug();
  connectGSM();
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
          //Serial.print("State changed: ");
          //Serial.println(state);
          lastState=state;
        }
        gsm.ready();
        break;
      case GSM_READY: // connected, let's wait to READY_STATE_DONE
        if(gsm.getReadyState() == READY_STATE_DONE) {
          Serial.println("GSM Connected!");
          connectingGSM=false;
          connectGPRS();         
        } else {
          gsm.ready(); 
        }
        break;
    }    
  } else {
    if(connectingGPRS) {
      GSM3_NetworkStatus_t state=gprs.status();
      switch(state) {
        case ERROR: // not connected or error.
        case IDLE: // start
        case CONNECTING: // connecting...
          if(lastState!=state) {
            //Serial.print("State changed: ");
            //Serial.println(state);
            lastState=state;
          }
          gprs.ready();
          break;
        case GPRS_READY: // connected, let's wait to GPRS_STATE_IDLE
          if(gprs.getReadyState() == GPRS_STATE_IDLE) {
            Serial.println("GPRS Connected!");
            connectingGPRS=false;           
          } else {
            gprs.ready(); 
          }
          break;
      } 
    } else {
      if(!gsm.isAccessAlive()) {
        // reconectar...
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
          // We are connected just do something each minute.
          if(millis()-processMillis>30000) {
            Serial.println("Do internet tasks... every 30 seconds.");
            processMillis=millis();
          }
        }
      }
    }
  }

  // Blink the led ...
  if(millis()-ledMillis>500) {
    digitalWrite(LED_PIN, ledState?HIGH:LOW);
    ledState=!ledState;
    ledMillis=millis();
  }

  // Do whatever... SARA won't block main loop!
}
