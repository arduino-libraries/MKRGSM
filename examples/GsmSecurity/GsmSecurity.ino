/*
  AWS IoT client

 This sketch connects to AWS IoT using TLS 1.2 through a MKR GSM 1400 board.

 Circuit:
 * MKR GSM 1400 board
 * Antenna
 * SIM card with a data plan

 created 11 April 2018
 by Darren Jeacocke
*/

// libraries
#include <MKRGSM.h>
#include <PubSubClient.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[]     = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[]      = SECRET_GPRS_APN;
const char GPRS_LOGIN[]    = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

const char server[] = "abb7o0rtitl09.iot.ap-southeast-2.amazonaws.com";
const char topic[] = "$aws/things/myThing/update";
const char clientId[] = "myThing";

GSMSSLClient gsmClient;
GSMSecurity profile;
GPRS gprs;
GSM gsmAccess;

PubSubClient mqttClient(server, 8883, gsmClient);

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Starting Arduino MQTT client...");
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

  Serial.println("Importing certificates...");

  profile.setRootCertificate(SECRET_ROOT_CERT);
  profile.setClientCertificate(SECRET_CLIENT_CERT);
  profile.setPrivateKey(SECRET_PRIVATE_KEY);
  profile.setValidation(SSL_VALIDATION_ROOT_CERT);
  profile.setVersion(SSL_VERSION_TLS_1_2);
  profile.setCipher(SSL_CIPHER_AUTO);
  gsmClient.setSecurityProfile(profile);

  Serial.println("Connecting...");

  while (!mqttClient.connect(clientId)) {
    Serial.print(".");
    delay(500);
  }
}

unsigned long prevNow = millis();

void loop() {
  unsigned long now = millis();
  if (now - prevNow >= 30000) {
    prevNow = now;
    if (mqttClient.publish(topic, "{d: { status: \"connected!\"}")) {
      Serial.println("Publish ok");
    } else {
      Serial.println("Publish failed");
    }
  }

  mqttClient.loop();
}
