/*
  Manage JSON configurations via GSM module filesystem.

  This sketch demonstrates how to use the internal filesystem
  of the GSM module to store and retrieve data and configurations
  both as JSON objects or C++ structs.

  Circuit:
  * MKR GSM 1400 board

 created 19 June 2020
 by Giampaolo Mancini
*/

#include <Arduino_JSON.h>
#include <MKRGSM.h>

#include "Config.h"

GSMFileUtils fileUtils;

void setup()
{
    Serial.begin(9600);

    while (!Serial)
        ;

    Serial.println("Store and Retrieve JSON data to GSM module storage.");
    Serial.println();

    fileUtils.begin();

    simpleDemo();
    structDemo();

    while (true)
        ;
}

void loop()
{
}

void simpleDemo()
{

    Serial.println();
    Serial.println("========================");
    Serial.println("Running simple JSON demo");
    Serial.println();

    JSONVar myObject;

    myObject["hello"] = "world";
    myObject["true"] = true;
    myObject["x"] = 42;

    String jsonString = JSON.stringify(myObject);

    Serial.println("Saving JSON file (test.json): ");
    Serial.println(jsonString);
    Serial.println();
    fileUtils.downloadFile("test.json", jsonString);

    printFiles(fileUtils);
    Serial.println();

    String jsonData;
    Serial.println("Reading JSON file (test.json): ");
    fileUtils.readFile("test.json", &jsonData);

    Serial.println("File contents:");
    Serial.println(jsonData);
    Serial.println();

    Serial.println("Parsing JSON contents:");
    JSONVar myConf = JSON.parse(jsonData);
    Serial.print("myConf[\"hello\"]: ");
    Serial.println(myConf["hello"]);
    Serial.print("myConf[\"true\"]: ");
    Serial.println(myConf["true"]);
    Serial.print("myConf[\"x\"]: ");
    Serial.println(myConf["x"]);
}

void structDemo()
{
    GSMModem modem;

    Serial.println();
    Serial.println("========================");
    Serial.println("Running Configuration via struct and JSON demo");
    Serial.println();
    Serial.println("Creating configuration struct:");

    Config conf;
    conf.deviceId = modem.getICCID();
    conf.timestamp = millis();

    Serial.print(conf);

    fileUtils.downloadFile("conf.json", conf.toJson());

    Serial.println();
    printFiles(fileUtils);
    Serial.println();

    Serial.println("Reading configuration file:");

    String jsonConf;
    fileUtils.readFile("conf.json", &jsonConf);
    Serial.println(jsonConf);
    Serial.println();

    Serial.println("Reading configuration struct:");
    Config newConf;
    newConf.fromJSON(jsonConf);

    Serial.print(newConf);
}