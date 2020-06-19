/*
  Download large file and store it into the GSM module filesystem.

 This sketch connects to a website through a MKR GSM 1400 board and
 downloads a large file and stores it into the filesystem of the GSM
 module.

 The file is processed in blocks of 512 bytes in order to save RAM.
 A block of data is read from the GSM module and the appended to a
 file created by the sketch.

 Circuit:
 * MKR GSM 1400 board
 * Antenna
 * SIM card with a data plan

 created 19 June 2020
 by Giampaolo Mancini
*/

// libraries
#include <MKRGSM.h>

GSMFileUtils fileUtils(false);

#include "Helpers.h"

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[] = SECRET_GPRS_APN;
const char GPRS_LOGIN[] = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;

// URL, path and port (for example: example.org)

void setup()
{
    // initialize serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    Serial.println("Starting Arduino web client.");

    fileUtils.begin();

    // List files on the GSM module's filesystem
    auto numberOfFiles = fileUtils.fileCount();
    Serial.print("Number of Files: ");
    Serial.println(numberOfFiles);
    Serial.println();

    printFiles(fileUtils);

    auto server = promptAndReadLine("Please, enter server name:", "arduino.cc");
    auto port = promptAndReadInt("Please, enter server port:", 80);
    auto filename = promptAndReadLine("Please, enter file name:", "asciilogo.txt");
    auto filesize = promptAndReadInt("Please, enter file size:", 2263);
    Serial.println("Connecting...");

    // connection state
    bool connected = false;

    // After starting the modem with GSM.begin()
    // attach the shield to the GPRS network with the APN, login and password
    while (!connected) {
        if ((gsmAccess.begin(PINNUMBER) == GSM_READY) && (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
            connected = true;
        } else {
            Serial.println("Not connected");
            delay(1000);
        }
    }

    // if you get a connection, report back via serial:
    if (client.connect(server.c_str(), port)) {
        Serial.println("connected");
        // Make a HTTP request:
        client.print("GET /");
        client.print(filename);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(server);
        client.println("Connection: close");
        client.println();
    } else {
        // if you didn't get a connection to the server:
        Serial.println("Connection failed");
    }

    // Download and store block-by-block
    storeFileBuffered(filename, filesize);

    auto updateBinSize = fileUtils.listFile(filename);
    Serial.print(filename);
    Serial.print(" downloaded size: ");
    Serial.println(updateBinSize);

    numberOfFiles = fileUtils.fileCount();
    Serial.print("Number of Files: ");
    Serial.println(numberOfFiles);
    Serial.println();

    printFiles(fileUtils);

}

void loop()
{
    // if there are incoming bytes available
    // from the server, read them and print them:
    if (client.available()) {
        char r = client.read();
        if (r < 16)
            Serial.print(0);
        Serial.print(r, HEX);

    }

    // if the server's disconnected, stop the client:
    if (!client.available() && !client.connected()) {
        Serial.println();
        Serial.println("disconnecting.");
        client.stop();

        // do nothing forevermore:
        for (;;)
            ;
    }
}

void storeFileBuffered(String filename, uint32_t totalLen)
{
    Serial.print("Ready to download \"");
    Serial.print(filename);
    Serial.print("\" - len: ");
    Serial.print(totalLen);
    Serial.println(" bytes.");

    constexpr uint32_t len { 512 };

    uint32_t cycles = totalLen / len;
    uint32_t spares = totalLen % len;

    int totalRead { 0 };

    fileUtils.deleteFile(filename);

    Serial.print("Saving file in ");
    Serial.print(cycles + 1);
    Serial.print(" blocks. [");
    Serial.print(cycles);
    Serial.print(' ');
    Serial.print(len);
    Serial.print(" -bytes blocks and ");
    Serial.print(spares);
    Serial.println(" bytes].");

    bool is_header_complete = false;
    String http_header;

    // Skip the HTTP header
    while (!is_header_complete) {
        while (client.available()) {
            const char c = client.read();
            http_header += c;
            if (http_header.endsWith("\r\n\r\n")) {
                Serial.println("Header Complete");
                is_header_complete = true;
                break;
            }
        }
    }

    // Define download and save lambda
    auto downloadAndSaveTrunk = [filename](uint32_t len) {
        char buf[len] { 0 };
        uint32_t written { 0 };

        if (client.available())
            written = client.readBytes(buf, len);

        fileUtils.appendFile(filename, buf, written);
        return written;
    };

    // Define wrapper function
    auto saveTrunk = [&totalRead, downloadAndSaveTrunk](size_t iter, uint32_t len) {
        Serial.print("Block ");
        if (iter < 10) Serial.print(' '); if (iter < 100) Serial.print(' '); 
        Serial.print(iter);

        totalRead += downloadAndSaveTrunk(len);

        Serial.print(": ");
        Serial.print(len);
        Serial.print(" - ");
        Serial.print(totalRead);
        Serial.println();
    };

    // Download and save complete trunks + spares
    for (auto c = 0; c <= cycles; c++)
        saveTrunk(c, len);

    Serial.println();        

}
