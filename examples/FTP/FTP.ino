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

//this file must be present in the remote directory SECRET_FTP_REMOTE_DIR
const String c_downloadFileName = "downloadFile"; 

// initialize the library instance
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
  GSMFileSystemElem localFile;
	GSMFTPElem remoteFile;

	Serial.println("Connect to FTP server.");
	if (ftp.connect(SECRET_FTP_HOST, SECRET_FTP_USER, SECRET_FTP_PASSWORD, SECRET_FTP_PORT) == false) {
		Serial.println("Failed to Connect to FTP server.");
    ftp.printError();
	}
  
	Serial.println("Change of directory");
	if (ftp.cd(SECRET_FTP_REMOTE_DIR) == false) {
		Serial.println("Failed to change of directory.");
	}
	
	Serial.print("Free space ");
	Serial.println(FILESYSTEM.freeSpace());

	Serial.println("Create remote directory : test");
	if (ftp.mkdir("test") == false) {
		Serial.println("Failed to create the directory.");
	}

	Serial.println("Rename remote directory : test to test2");
	if (ftp.rename("test", "test2") == false) {
		Serial.println("Failed to rename the directory.");
	}

	Serial.println("Write a binary file in local memory");
	double valueWR = -12.5789876;
	double valueRD = 0;
	if (FILESYSTEM.write("myFile", &valueWR, sizeof(valueWR)) == false) {
		Serial.println("Failed to write file");
	}
	
	Serial.println("Send the file to the server");
	if (ftp.upload("myFile", "myFileToServer") == false) {
		Serial.println("Failed to upload the file.");
		ftp.printError();
	}
	
	Serial.println("Retreive the file from the server to local memory");
	if (ftp.download("myFileToLocalMemory", "myFileToServer") == false) {
		Serial.println("Failed to download the file.");
		ftp.printError();
	}

	Serial.println("Check that the original file is identical to the one that was received");
	if (FILESYSTEM.read("myFileToLocalMemory", &valueRD, sizeof(valueRD)) == false) {
		Serial.println("Failed to read file");
	}
	else if (valueWR != valueRD) {
		Serial.println("Failed to read file, value is corrupted");
	}

	Serial.print("Free space ");
	Serial.println(FILESYSTEM.freeSpace());

	Serial.println("Display local files");
	if (FILESYSTEM.ls(localFile, true) == false) {
		Serial.println("Failed to display local files");
	}

	Serial.println("Remove local files");
	if (FILESYSTEM.remove(localFile) == false) {
		Serial.println("Failed to remove file");
	}
	
	Serial.println("Display local files");
	if (FILESYSTEM.ls(localFile, true) == false) {
		Serial.println("Failed to display local files");
	}

	Serial.println("Display remote files");
	if (ftp.ls(remoteFile, true) == false) {
		Serial.println("Failed to display files.");
	}

	Serial.println("Delete the created file and directory");
	if (ftp.removeDirectory("test2") == false) {
		Serial.println("Failed to remove files : test2.");
	}
	if (ftp.removeFile("myFileToServer") == false) {
		Serial.println("Failed to remove files : myFileToServer.");
	}

	Serial.println("Display remote files");
	if (ftp.ls(remoteFile, true) == false) {
		Serial.println("Failed to display files.");
	}
	
	//--- Test download/upload a large file with non blocking function ---

	Serial.println();
	Serial.println("Download a file with non blocking function");
	downloadFileNonBlocking("downloadedFile", c_downloadFileName);

	Serial.println("Display local files");
	if (FILESYSTEM.ls(localFile, true) == false) {
		Serial.println("Failed to display local files");
	}

	Serial.println("Upload a file with non blocking function");
	uploadFileNonBlocking("downloadedFile", "uploadFile");

	Serial.println("Display remote files");
	if (ftp.ls(remoteFile, true) == false) {
		Serial.println("Failed to display files.");
	}

	Serial.println("Remove local and remote files");
	if (FILESYSTEM.remove("downloadedFile") == false) {
		Serial.println("Failed to remove file");
	}
	if (ftp.removeFile("uploadFile") == false) {
		Serial.println("Failed to remove files : myFileToServer.");
	}

	Serial.println("Disconnect to FTP server");
	if (ftp.disconnect() == false) {
		Serial.println("Failed to disconnect.");
	}
  
	for (;;)
		;
}

//Example of non blocking download functions
void downloadFileNonBlocking(const String localFileName, const String remoteFileName) {

	Serial.println("Retreive the file from the server to local memory");
	//Start download
	if (ftp.downloadStart(localFileName, remoteFileName) == false) {
		Serial.println("Failed to start download.");
		ftp.printError();
	}
	
	//update download
	while (ftp.downloadReady(localFileName, true) == 0)
	{
		//do some job
	}
}

//Example of non blocking upload functions
void uploadFileNonBlocking(const String localFileName, const String remoteFileName) {

	Serial.println("Send the file to the server from local memory");
	if (ftp.uploadStart(localFileName, remoteFileName) == false) {
		Serial.println("Failed to start upload.");
		ftp.printError();
	}

	int res = 0;
	while (res == 0){
		res = ftp.uploadReady();
		if (res == 1) {
			Serial.println("Upload finished.");
		}
		else if (res < 0) {
			Serial.println("Upload error.");
			ftp.printError();
		}
		//do some job
	}
}
