/*
  ArduinoMqttClient - SSLCertificateManagement_Example

  This example shows how to upload a self signed certificate 
  by GSMSSLClient's APIs and connects to a MQTT broker 
  and publishes a message to a topic once a second  .

  The circuit:
  - Arduino MKRGSM1400

  This example code is in the public domain.
*/

#include <ArduinoMqttClient.h>
#include <MKRGSM.h>
#include "arduino_secrets.h"


const char PINNUMBER[]     = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[]      = SECRET_GPRS_APN;
const char GPRS_LOGIN[]    = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

// initialize the library instance
GSMSSLClient client;
GPRS gprs;
GSM gsmAccess(true);
MqttClient mqttClient(client);

// replace with your brooker, port and topic 
const char broker[] = "";
int        port     = 8883;
const char topic[]  = "";

const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

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

  Serial.println("You're connected to the network");
  Serial.println();

  // eraseTrustedRoot erase from the module all the actual Trusted Root
  // pointed by the GSM object, call this API after the set will try to erase
  // all the certificates present in SECRET_GSM_ROOT_CERTS
  client.eraseTrustedRoot();
  client.setUserRoots(SECRET_GSM_ROOT_CERTS, SECRET_GSM_ROOT_SIZE);
  
  client.setSignedCertificate(SECRET_CERT, "MKRGSM01", sizeof(SECRET_CERT));
  client.setPrivateKey(SECRET_KEY, "MKRGSMKEY01", sizeof(SECRET_KEY));
  client.useSignedCertificate("MKRGSM01");
  client.usePrivateKey("MKRGSMKEY01");
  client.setTrustedRoot("Let_s_Encrypt_Authority_X3");

  
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {

    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();


}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.print("hello ");
    Serial.println(count);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    mqttClient.print("hello ");
    mqttClient.print(count);
    if (!mqttClient.endMessage()) {
      Serial.println("transmission error");
    }

    Serial.println();

    count++;
  }
}
