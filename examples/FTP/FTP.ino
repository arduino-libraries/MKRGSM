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
const char PINNUMBER[] = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[] = SECRET_GPRS_APN;
const char GPRS_LOGIN[] = SECRET_GPRS_LOGIN;
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
    }
    else {
      Serial.println("Not connected");
      delay(1000);
    }
  }
}

void loop() {
  GSMFileSystemElem localFile;
  GSMFTPElem remoteFile;
  String fileName;

  test("Connect to FTP server",
    ftp.connect(SECRET_FTP_HOST, SECRET_FTP_USER, SECRET_FTP_PASSWORD, SECRET_FTP_PORT));

  test("Change current remote directory",
    ftp.cd(SECRET_FTP_REMOTE_DIR));

  test("Create remote directory",
    ftp.mkdir("test"));

  test("Rename remote directory",
    ftp.rename("test", "test2"));

  double valueWR = -12.5789876;
  double valueRD = 0;

  test("Local file system write",
    FILESYSTEM.write("myFile", &valueWR, sizeof(valueWR)));

  test("Local file system free space",
    (FILESYSTEM.freeSpace() > 0));

  test("Upload from local file system to FTP server",
    ftp.upload("myFile", "myFileToServer", 10000));

  test("Download from FTP server to local file system",
    ftp.download("myFileToLocalMemory", "myFileToServer", 10000));

  test("Local file system read",
    FILESYSTEM.read("myFileToLocalMemory", &valueRD, sizeof(valueRD)));

  test("Check local file consistency after upload, download then read local file system",
    (valueRD == valueWR));

  test("Display local files",
    FILESYSTEM.ls(localFile, true));

  test("Remove local files",
    FILESYSTEM.remove(localFile));

  test("Display local files",
    FILESYSTEM.ls(localFile, true));

  test("Display remote files",
    ftp.ls(remoteFile, true));

  test("Delete remote directory",
    ftp.removeDirectory("test2"));

  test("Delete remote file",
    ftp.removeFile("myFileToServer"));

  test("Display remote files",
    ftp.ls(remoteFile, true));

  //--- Test download/upload a large file with non blocking function ---

  test("Non blocking download from FTP server to local file system",
    downloadFileNonBlocking("downloadedFile", c_downloadFileName));

  test("Display local files",
    FILESYSTEM.ls(localFile, true));

  test("Non blocking upload from local file system to FTP server",
    uploadFileNonBlocking("downloadedFile", "uploadFile"));

  test("Display local files",
    FILESYSTEM.remove("downloadedFile"));

  test("Delete remote file",
    ftp.removeFile("uploadFile"));

  //--- Test direct upload/download ---
  //direct transfer doesn't use local file system but volatile memory 
  //upload volatile memory => FTP server
  //download FTP server => volatile memory

  fileName = "myFile.txt";
  char bufferWR[128];
  char bufferRD[128];
  for (int i = 0; i < 128; ++i) {
    bufferWR[i] = 33 + i;
    bufferRD[i] = 0;
  }

  test("Direct upload from volatile memory to FTP server",
    ftp.write(&bufferWR[0], sizeof(bufferWR), fileName, 10000));

  test("Direct download from FTP server to volatile memory",
    ftp.read(&bufferRD[0], sizeof(bufferRD), fileName, 10000));

  test("Direct upload/download tranferred data consistency",
    (memcmp(bufferRD, bufferWR, 128) == 0));

  test("Delete remote file",
    ftp.removeFile(fileName));

  //--- Test Stream data ---

  fileName = "FileToStream";
  test("Stream data to server",
    StreamOut(fileName));

  test("Stream data from server",
    StreamIn(fileName));

  test("Delete remote file",
    ftp.removeFile(fileName));

  test("Disconnect to FTP server",
    ftp.disconnect());

  for (;;)
    ;
}

//Example of non blocking download functions
bool downloadFileNonBlocking(const String localFileName, const String remoteFileName)
{
  int res = 0;

  //Start download
  if (ftp.downloadStart(localFileName, remoteFileName) == false) {
    return false;
  }

  //update download
  while (res == 0) {
    res = ftp.downloadReady(localFileName, true);
    //do something
  }

  return (res == 1);
}

//Example of non blocking upload functions
bool uploadFileNonBlocking(const String localFileName, const String remoteFileName)
{
  int res = 0;

  if (ftp.uploadStart(localFileName, remoteFileName) == false) {
    return false;
  }

  while (res == 0) {
    res = ftp.uploadReady();
    //do something
  }

  return (res == 1);
}

bool StreamOut(const String& remoteFileName)
{
  int res = 0;

  //Start upload
  if (ftp.streamOutStart(remoteFileName) == false) {
    return false;
  }

  //send data by packets
  for (int i = 0; i < 1000; ++i) {
    char buffer[128];
    snprintf(buffer, 128, "Line number %d\n", i);
    ftp.streamOut(buffer, 128);
    //do something
  }

  while (res == 0) {
    res = ftp.streamOutReady();
    //do something
  }

  return (res == 1);
}

bool StreamIn(const String& remoteFileName)
{
  int res = 0;
  bool dataConsistency = true;

  //Start download
  if (ftp.streamInStart(remoteFileName) == false) {
    return false;
  }

  //receive data by packets
  for (int i = 0; i < 1000; ++i) {
    char buffer[128];
    ftp.streamIn(buffer, 128);
    String test = String(buffer);
    dataConsistency &= (test.indexOf(String("Line number " + String(i) + "\n")) >= 0);
    //do something
  }

  while (res == 0) {
    res = ftp.streamInReady();
    //do something
  }

  return ((res == 1) && (dataConsistency == true));
}

bool test(const String& msg, bool function)
{
  if (function == true) {
    Serial.print("OK - ");
  }
  else {
    Serial.print("ERROR - ");
  }
  Serial.println(msg);

  return function;
}
