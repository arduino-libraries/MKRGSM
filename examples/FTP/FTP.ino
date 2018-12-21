/*
  FTP client

 This sketch connects to a FTP server through a MKR GSM 1400 board. 

 Circuit:
 * MKR GSM 1400 board
 * Antenna
 * SIM card with a data plan

 created 21 Dec 2018
 by Tryhus
*/

// libraries
#include <MKRGSM.h>
#include <GSMFTP.h>
#include <GSMFileSystem.h>
#include "arduino_secrets.h" 

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[]     = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[]      = SECRET_GPRS_APN;
const char GPRS_LOGIN[]    = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

// initialize the library instance
GSMFileSytem fileSystem;
GSMFTP ftp;
GPRS gprs;
GSM gsmAccess;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Starting Arduino FTP client.");
  // connection state
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
}

void loop() {

	Serial.println("Connect to FTP server.");
	if (ftp.connect(SECRET_FTP_HOST, SECRET_FTP_USER, SECRET_FTP_PASSWORD) == false) {
		Serial.println("Failed to Connect to FTP server.");
	}

	Serial.println("Change of directory");
	if (ftp.cd(SECRET_FTP_REMOTE_DIR) == false) {
		Serial.println("Failed to change of directory.");
	}

	Serial.print("Free space ");
	Serial.println(fileSystem.freeSpace());

	Serial.println("Create remote file : test");
	if (ftp.mkdir("test") == false) {
		Serial.println("Failed to create the file.");
	}

	Serial.println("Rename remote file : test to test2");
	if (ftp.rename("test", "test2") == false) {
		Serial.println("Failed to rename the file.");
	}

	Serial.println("Write a binary file in local memory");
	double valueWR = -12.5789876;
	double valueRD = 0;
	if (fileSystem.write("myFile", &valueWR, sizeof(valueWR)) == false) {
		Serial.println("Failed to write file");
	}

	Serial.println("Send the file to the server");
	if (ftp.upload("myFile", "myFileToServer") == false) {
		Serial.println("Failed to upload the file.");
	}

	Serial.println("Retreive the file from the server to local memory");
	if (ftp.download("myFileToServer", "myFileToLocalMemory") == false) {
		Serial.println("Failed to download the file.");
	}

	Serial.println("Check that the original file is identical to the one that was received");
	if (fileSystem.read("myFileToLocalMemory", &valueRD, sizeof(valueRD)) == false) {
		Serial.println("Failed to read file");
	}
	else if (valueWR != valueRD) {
		Serial.println("Failed to read file, value is corrupted");
	}

	Serial.print("Free space ");
	Serial.println(fileSystem.freeSpace());

	Serial.println("Display local files");
	if (fileSystem.ls(true) == false) {
		Serial.println("Failed to display local files");
	}

	Serial.println("Remove local files");
	for (int i = 0; i < fileSystem.fileCount(); ++i) {
		fileSystem.remove(fileSystem.file(i).name);
	}

	Serial.println("Display local files");
	if (fileSystem.ls(true) == false) {
		Serial.println("Failed to display local files");
	}

	Serial.println("Display remote files");
	if (ftp.ls(true) == false) {
		Serial.println("Failed to display files.");
	}

	Serial.println("Delete the created files");
	if (ftp.remove("test2") == false) {
		Serial.println("Failed to remove files : test2.");
	}
	if (ftp.remove("myFileToServer") == false) {
		Serial.println("Failed to remove files : myFileToServer.");
	}

	Serial.println("Display remote files");
	if (ftp.ls(true) == false) {
		Serial.println("Failed to display files.");
	}

	Serial.println("Disconnect to FTP server");
	if (ftp.disconnect() == false) {
		Serial.println("Failed to disconnect.");
	}

	for (;;)
		;
}

