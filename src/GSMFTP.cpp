/*
  This file is part of the MKR GSM library.
  Copyright (C) 2017  Arduino AG (http://www.arduino.cc/)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <GSMFTP.h>
#include <GSMFileSystem.h>

GSMFTP::GSMFTP() :
  _connected(-1),
  _dirCreated(-1),
  _dirChanged(-1),
  _fileRemoved(-1),
  _fileDownloaded(-1),
  _fileUploaded(-1),
  _fileDirectUploaded(-1),
  _fileDirectDownloaded(-1),
  _downloadDisplayTimeRef(0),
  _downloadRemoteFileSize(0),
  _fileInfo(nullptr),
  _uploadRemainingBytes(0)
{
  MODEM.addUrcHandler(this);
}

GSMFTP::~GSMFTP()
{
  MODEM.removeUrcHandler(this);
}

bool GSMFTP::connect(String hostname, String user, String password, uint16_t port, bool passiveMode)
{
  uint32_t start = millis();

  MODEM.send("AT+UFTP=1,\"" + hostname + "\"");
  if (MODEM.waitForResponse(100) != 1) {
    return false;
  }

  MODEM.send("AT+UFTP=2,\"" + user + "\"");
  if (MODEM.waitForResponse(100) != 1) {
    return false;
  }

  MODEM.send("AT+UFTP=3,\"" + password + "\"");
  if (MODEM.waitForResponse(100) != 1) {
    return false;
  }

  String command = (passiveMode == true) ? "AT+UFTP=6,1" : "AT+UFTP=6,0";
  MODEM.send(command);
  if (MODEM.waitForResponse(100) != 1) {
    return false;
  }

  MODEM.sendf("AT+UFTP=7,%d", port);
  if (MODEM.waitForResponse(100) != 1) {
    return false;
  }

  MODEM.send("AT+UDNSRN=0,\"" + hostname + "\"");
  if (MODEM.waitForResponse(10000) != 1) {
    return false;
  }

  _connected = -2;
  while ((_connected != 1) && (millis() - start) < c_connectionTimeout) {
    if (_connected == -2) {
      MODEM.send("AT+UFTPC=1");
      _connected = -1;
    }
    else if (_connected == 0) {
      _connected = -2;
    }
    MODEM.poll();
  }

  return (_connected == 1);
}

bool GSMFTP::disconnect()
{
  uint32_t start = millis();
  _connected = -1;

  MODEM.send("AT+UFTPC=0");

  while ((_connected == -1) && (millis() - start) < 10000) {
    MODEM.poll();
  }
  return (_connected == 0);
}

bool GSMFTP::ls(GSMFTPElem& file, bool show, uint32_t timeout)
{
  if (_connected == 1) {
    file.clear();
    _fileInfo = &file;
    uint32_t start = millis();

    MODEM.send("AT+UFTPC=13");

    while (_fileInfo != nullptr) {
      MODEM.poll();
      if ((millis() - start) > timeout) {
        _fileInfo = nullptr;
        return false;
      }
    }

    if (show == true) {
      for (int i = 0; i < file.count(); ++i) {
        file.show(i);
      }
    }
    return true;
  }
  else {
    return false;
  }
}

bool GSMFTP::ls(GSMFTPElem& file, const String name, bool show, uint32_t timeout)
{
  if (_connected == 1) {
    file.clear();
    _fileInfo = &file;
    uint32_t start = millis();
    MODEM.send("AT+UFTPC = 13,\"" + name + "\"");

    while (_fileInfo != nullptr) {
      MODEM.poll();
      if ((millis() - start) > timeout) {
        _fileInfo = nullptr;
        return false;
      }
    }

    if (show == true) {
      for (int i = 0; i < file.count(); ++i) {
        file.show(i);
      }
    }
    return true;
  }
  else {
    return false;
  }
}

bool GSMFTP::mkdir(const String& name, uint32_t timeout)
{
  uint32_t start = millis();
  if (_connected != 1) {
    return false;
  }
  _dirCreated = -1;
  MODEM.send("AT+UFTPC=10,\"" + name + "\"");

  while ((millis() - start) < timeout) {
    MODEM.poll();
    if (_dirCreated == 0) {
      return false;
    }
    else if (_dirCreated == 1) {
      return true;
    }
  }
  return false;
}

bool GSMFTP::removeFile(const String& name, uint32_t timeout)
{
  uint32_t start = millis();
  if (_connected != 1) {
    return false;
  }

  _fileRemoved = -1;
  MODEM.send("AT+UFTPC=2,\"" + name + "\"");
  if (MODEM.waitForResponse(100) != 1) {
    return false;
  }

  while ((millis() - start) < timeout) {
    MODEM.poll();
    if (_fileRemoved == 0) {
      _fileRemoved = -1;
      return false;
    }
    else if (_fileRemoved == 1) {
      return true;
    }
  }
  return false;
}

bool GSMFTP::removeDirectory(const String& name, uint32_t timeout)
{
  uint32_t start = millis();
  if (_connected != 1) {
    return false;
  }

  _dirRemoved = -1;
  MODEM.send("AT+UFTPC=11,\"" + name + "\"");


  while ((millis() - start) < timeout) {
    MODEM.poll();
    if (_dirRemoved == 0) {
      return false;
    }
    else if (_dirRemoved == 1) {
      return true;
    }
  }
  return false;
}

bool GSMFTP::rename(const String& oldName, const String& name, uint32_t timeout)
{
  uint32_t start = millis();
  if (_connected != 1) {
    return false;
  }

  _fileRenamed = -1;
  MODEM.send("AT+UFTPC=3,\"" + oldName + "\",\"" + name + "\"");

  while ((millis() - start) < timeout) {
    MODEM.poll();
    if (_fileRenamed == 0) {
      return false;
    }
    else if (_fileRenamed == 1) {
      return true;
    }
  }
  return false;
}

bool GSMFTP::download(const String& localFileName, const String& remoteFileName, int32_t timeout)
{
  uint32_t start = millis();
  if (_connected != 1) {
    return false;
  }
  _fileDownloaded = -1;
  MODEM.send("AT+UFTPC=4,\"" + remoteFileName + "\",\"" + localFileName + "\"");

  while ((timeout < 0) || (millis() - start) < timeout) {
    MODEM.poll();
    if (_fileDownloaded == 0) {
      return false;
    }
    else if (_fileDownloaded == 1) {
      return true;
    }
  }
  return false;
}

bool GSMFTP::downloadStart(const String& localFileName, const String& remoteFileName)
{
  if (_connected != 1) {
    return false;
  }
  //get remote file informations
  GSMFTPElem remoteFile;
  ls(remoteFile, remoteFileName);
  _downloadRemoteFileSize = remoteFile.elem(0).size;
  _fileDownloaded = -1;

  MODEM.send("AT+UFTPC=4,\"" + remoteFileName + "\",\"" + localFileName + "\"");
  if (MODEM.waitForResponse(100) != 1) {
    return false;
  }
  return true;
}

int GSMFTP::downloadReady(const String& localFileName, bool showProgression)
{
  if (_connected != 1) {
    return -1;
  }

  MODEM.poll();

  if (_fileDownloaded == 0) {
    if (showProgression == true) {
      Serial.println("Failed to download the file.");
    }
    return -1;
  }
  else if (_fileDownloaded == 1) {
    _fileDownloaded = -1;
    if (showProgression == true) {
      Serial.println("Download 100%");
    }
    return 1;
  }
  else if ((showProgression == true) &&
    ((millis() - _downloadDisplayTimeRef) > 5000)) {
    double progress = 0;
    if (_downloadRemoteFileSize > 0) {
      progress = 100.0*FILESYSTEM.size(localFileName) / (double)(_downloadRemoteFileSize);
    }
    Serial.print("Download ");
    Serial.print(progress);
    Serial.println(" %");
    _downloadDisplayTimeRef = millis();
  }
  return 0;
}

bool GSMFTP::upload(const String& localFileName, const String& remoteFileName, int32_t timeout)
{
  uint32_t start = millis();
  if (_connected != 1) {
    return false;
  }

  _fileUploaded = -1;
  MODEM.send("AT+UFTPC=5,\"" + localFileName + "\",\"" + remoteFileName + "\"");

  while ((timeout < 0) || (millis() - start) < timeout) {
    MODEM.poll();
    if (_fileUploaded == 0) {
      return false;
    }
    else if (_fileUploaded == 1) {
      return true;
    }
  }
  return false;
}

bool GSMFTP::uploadStart(const String& localFileName, const String& remoteFileName)
{
  if (_connected != 1) {
    return false;
  }

  _fileUploaded = -1;
  MODEM.send("AT+UFTPC=5,\"" + localFileName + "\",\"" + remoteFileName + "\"");
  if (MODEM.waitForResponse(100) != 1) {
    return false;
  }
  return true;
}

int GSMFTP::uploadReady()
{
  if (_connected != 1) {
    return -1;
  }

  MODEM.poll();

  if (_fileUploaded == 0) {
    return -1;
  }
  else if (_fileUploaded == 1) {
    _fileUploaded = -2;
    return 1;
  }
  return 0;
}

bool GSMFTP::cd(const String& name, uint32_t timeout)
{
  uint32_t start = millis();
  if (_connected != 1) {
    return false;
  }

  _dirChanged = -1;
  MODEM.send("AT+UFTPC=8,\"" + name + "\"");

  while ((millis() - start) < timeout) {
    MODEM.poll();
    if (_dirChanged == 0) {
      return false;
    }
    else if (_dirChanged == 1) {
      return true;
    }
  }
  return false;
}

void GSMFTP::printError()
{
  String res = "FTP last error : ";
  String response;

  MODEM.send("AT+UFTPER");

  if ((MODEM.waitForResponse(1000, &response) == 1) &&
    (response.startsWith("+UFTPER:"))) {
    res += response.substring(8);
  }
  else {
    res += "no response";
  }
  Serial.println(res);
}

bool GSMFTP::write(void* data, size_t size, const String& remoteFileName, int32_t timeout)
{
  uint32_t start = millis();
  if (_connected != 1) {
    return false;
  }
  _fileDirectUploaded = -1;

  MODEM.send("AT+UFTPC=7,\"" + remoteFileName + "\"");
  String resp;

  if (MODEM.waitForResponse(timeout, &resp) != 1) {
    return false;
  }

  MODEM.write((const uint8_t*)data, size);
  MODEM.escapeSequence(1500, 0);

  while ((timeout < 0) || ((millis() - start) < timeout)) {

    MODEM.poll();

    if (_fileDirectUploaded == 0) {
      return false;
    }
    else if (_fileDirectUploaded == 1) {
      return true;
    }
  }

  return false;
}

bool GSMFTP::read(void* data, size_t size, const String& remoteFileName, int32_t timeout)
{
  _fileDirectDownloaded = -1;
  uint32_t start = millis();
  if (_connected != 1) {
    return false;
  }

  GSMFTPElem remoteFile;
  ls(remoteFile, remoteFileName);
  if (remoteFile.elem(0).size == 0) {
    return false;
  }
  else if (size > remoteFile.elem(0).size) {
    size = remoteFile.elem(0).size;
  }

  MODEM.send("AT+UFTPC=6,\"" + remoteFileName + "\"");

  if (MODEM.waitForResponse(timeout) != 1) {
    return false;
  }
  uint32_t res = MODEM.read((uint8_t*)data, size, timeout);

  if (res < remoteFile.elem(0).size) {
    MODEM.escapeSequence(1000, 1000, true);
  }

  while (((timeout < 0) || (millis() - start) < timeout)) {

    MODEM.poll();

    if ((_fileDirectDownloaded == 0) || (_fileDirectDownloaded == 1)) {
      return (res == size);
    }
  }
  return false;
}

bool GSMFTP::streamOutStart(const String& remoteFileName)
{
  if (_connected != 1) {
    return false;
  }
  _fileDirectUploaded = -2;

  MODEM.send("AT+UFTPC=7,\"" + remoteFileName + "\"");

  if (MODEM.waitForResponse(10000) != 1) {
    return false;
  }
  return true;
}

bool GSMFTP::streamOut(void* data, size_t size)
{
  if (_connected != 1) {
    return false;
  }
  MODEM.write((const uint8_t*)data, size);
  return true;
}

int GSMFTP::streamOutReady()
{
  if (_connected != 1) {
    return -1;
  }

  if (_fileDirectUploaded == -2) {
    MODEM.escapeSequence(1500, 0);
    _fileDirectUploaded = -1;
  }

  MODEM.poll();

  if (_fileDirectUploaded == 0) {
    return -1;
  }
  else if (_fileDirectUploaded == 1) {
    return 1;
  }

  return 0;
}

bool GSMFTP::streamInStart(const String& remoteFileName)
{
  _fileDirectDownloaded = -2;
  uint32_t start = millis();
  if (_connected != 1) {
    return false;
  }

  GSMFTPElem remoteFile;
  ls(remoteFile, remoteFileName);
  _uploadRemainingBytes = remoteFile.elem(0).size;
  if (_uploadRemainingBytes == 0) {
    return false;
  }

  MODEM.send("AT+UFTPC=6,\"" + remoteFileName + "\"");

  if (MODEM.waitForResponse(10000) != 1) {
    return false;
  }

  return true;
}

int GSMFTP::streamIn(void* data, size_t size, int32_t timeout)
{
  if (_connected != 1) {
    return -1;
  }

  if (size > _uploadRemainingBytes) {
    size -= _uploadRemainingBytes;
  }

  uint32_t res = MODEM.read((uint8_t*)data, size, timeout);
  _uploadRemainingBytes -= res;

  return ((_uploadRemainingBytes == 0) ? 1 : 0);
}

int GSMFTP::streamInReady()
{
  if (_connected != 1) {
    return -1;
  }

  if ((_fileDirectDownloaded == -2) && (_uploadRemainingBytes > 0)) {
    MODEM.escapeSequence(1000, 1000, true);
    _fileDirectDownloaded = -1;
  }

  MODEM.poll();

  if ((_fileDirectDownloaded == 0) || (_fileDirectDownloaded == 1)) {
    return 1;
  }

  return 0;
}


//--- GSMFTPElem

GSMFTPElem::Elem GSMFTPElem::elem(uint16_t i)
{
  if (i < _count) {
    return _elem[i];
  }
  else {
    return Elem();
  }
}

void GSMFTPElem::append(const Elem elem)
{
  Elem* tmp = new Elem[_count + 1];
  for (int i = 0; i < _count; ++i) {
    tmp[i] = _elem[i];
  }
  tmp[_count] = elem;
  if (_elem != nullptr) {
    delete[] _elem;
  }
  _elem = tmp;
  _count++;
}

void GSMFTPElem::clear() {
  if (_elem != nullptr) {
    delete[] _elem;
    _elem = nullptr;
  }
  _count = 0;
}

void GSMFTPElem::show(int i)
{
  if (i >= _count) {
    return;
  }

  Serial.print(_elem[i].permissions);
  Serial.print(" ");
  Serial.print(_elem[i].number);
  Serial.print(" ");
  Serial.print(_elem[i].user);
  Serial.print(" ");
  Serial.print(_elem[i].group);
  Serial.print(" ");
  Serial.print(_elem[i].size);
  Serial.print(" ");
  Serial.print(_elem[i].lastModified);
  Serial.print(" ");
  Serial.print(_elem[i].name);
  Serial.println();
}

void GSMFTPElem::parse(const String& str)
{
  String res = str;

  if (res == "\"") {
    return;
  }
  else if (_count == 0) {
    int i = res.indexOf('"');
    if (i < 0) {
      return;
    }
    res = res.substring(i + 1);
  }
  Elem elem;
  for (int i = 0; i < 7; ++i) {
    String tmp = res;
    int j = res.indexOf(" ");

    while (res.charAt(j + 1) == ' ') {
      ++j;
    }
    if (i == 5) {
      for (int k = 1; k < 3; ++k) {
        j = res.indexOf(" ", j + 1);
        while (res.charAt(j + 1) == ' ') {
          ++j;
        }
      }
    }

    if (j > 0) {
      tmp = res.substring(0, j + 1);
      tmp.trim();
      res = res.substring(j + 1);
    }

    switch (i)
    {
    case 0:
      elem.permissions = tmp;
      break;
    case 1:
      elem.number = tmp.toInt();
      break;
    case 2:
      elem.user = tmp;
      break;
    case 3:
      elem.group = tmp;
      break;
    case 4:
      elem.size = tmp.toInt();
      break;
    case 5:
      elem.lastModified = tmp;
      break;
    case 6:
      elem.name = tmp;
      break;
    default:
      break;
    }
  }
  append(elem);
}

void GSMFTP::handleUrc(const String& urc)
{
  if (urc.startsWith("+UUFTPCR: 1,")) {
    _connected = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 1 : -1;
  }
  else if (urc.startsWith("+UUFTPCR: 0,")) {
    _connected = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 0 : -1;
  }
  else if (urc.startsWith("+UUFTPCD: 13,")) {
    if ((urc.charAt(urc.lastIndexOf(",") + 1) == '0')) {
      printError();
    }
  }
  else if (urc.startsWith("+UUFTPCR: 10,")) {
    _dirCreated = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 1 : 0;
  }
  else if (urc.startsWith("+UUFTPCR: 8,")) {
    _dirChanged = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 1 : 0;
  }
  else if (urc.startsWith("+UUFTPCR: 2,")) {
    _fileRemoved = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 1 : 0;
  }
  else if (urc.startsWith("+UUFTPCR: 11,")) {
    _dirRemoved = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 1 : 0;
  }
  else if (urc.startsWith("+UUFTPCR: 3,")) {
    _fileRenamed = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 1 : 0;
  }
  else if (urc.startsWith("+UUFTPCR: 4,")) {
    _fileDownloaded = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 1 : 0;
  }
  else if (urc.startsWith("+UUFTPCR: 5,")) {
    _fileUploaded = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 1 : 0;
  }
  else if (urc.startsWith("+UUFTPCR: 6,")) {
    _fileDirectDownloaded = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 1 : 0;
  }
  else if (urc.startsWith("+UUFTPCR: 7,")) {
    _fileDirectUploaded = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 1 : 0;
  }
  else if (urc.startsWith("+UUFTPCR: 13,")) {
    _fileInfo = nullptr;
  }

  if (_fileInfo != nullptr) {
    _fileInfo->parse(urc);
  }
}
