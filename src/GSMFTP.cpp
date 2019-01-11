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

GSMFTP::GSMFTP(): 
	_connected(false),
	_dirCreated(-1),
	_dirChanged(-1),
	_fileRemoved(-1),
	_downloadDisplayTimeRef(0),
	_downloadRemoteFileSize(0),
	_fileInfo(nullptr)
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
	if (MODEM.waitForResponse(100) != 1){
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

	while ((_connected == false) && (millis() - start) < c_connectionTimeout) {
		MODEM.send("AT+UFTPC=1");
		if (MODEM.waitForResponse(100) != 1) {
			continue;
		}
		for (int i = 0; i < 30; ++i) {
			delay(100);
			MODEM.ready();
			if (_connected == true){
				break;
			}
		}
	}

	return _connected;
}

bool GSMFTP::disconnect()
{
	uint32_t start = millis();
	while ((_connected == true) && (millis() - start) < 10000) {
		MODEM.send("AT+UFTPC=0");
		if (MODEM.waitForResponse(100) != 1) {
			continue;
		}
		for (int i = 0; i < 30; ++i) {
			delay(100);
			MODEM.ready();
			if (_connected == false) {
				break;
			}
		}
	}

	return !_connected;
}

bool GSMFTP::ls(GSMFTPElem& file, bool show, uint32_t timeout)
{
	if (_connected == true) {
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
	else{
		return false;
	}
}

bool GSMFTP::ls(GSMFTPElem& file, const String name, bool show, uint32_t timeout)
{
  if (_connected == true) {
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
	if (_connected == false){
		return false;
	}
	_dirCreated = -2;

	while ((millis() - start) < timeout) {
		if (_dirCreated == -2) {
			_dirCreated = -1;
			MODEM.send("AT+UFTPC=10,\"" + name + "\"");
			if (MODEM.waitForResponse(100) != 1) {
				return false;
			}
		}
		MODEM.poll();
		if (_dirCreated == 0) {
			_dirCreated = -2;
		}else if (_dirCreated == 1) {
			return true;
		}
	}
	return false;
}

bool GSMFTP::removeFile(const String& name, uint32_t timeout)
{
	uint32_t start = millis();
	if (_connected == false) {
		return false;
	}

	_fileRemoved = -2;

	while ((millis() - start) < timeout) {
		if (_fileRemoved == -2) {
			_fileRemoved = -1;
			MODEM.send("AT+UFTPC=2,\"" + name + "\"");
			if (MODEM.waitForResponse(100) != 1) {
				return false;
			}
		}
		MODEM.poll();
		if (_fileRemoved == 0) {
			_fileRemoved = -2;
		}else if (_fileRemoved == 1) {
			return true;
		}
	}
	return false;
}

bool GSMFTP::removeDirectory(const String& name, uint32_t timeout)
{
	uint32_t start = millis();
	if (_connected == false) {
		return false;
	}

	_dirRemoved = -2;

	while ((millis() - start) < timeout) {
		if (_dirRemoved == -2) {
			_dirRemoved = -1;
			MODEM.send("AT+UFTPC=11,\"" + name + "\"");
			if (MODEM.waitForResponse(100) != 1) {
				return false;
			}
		}
		MODEM.poll();
		if (_dirRemoved == 0) {
			_dirRemoved = -2;
		}else if (_dirRemoved == 1) {
			return true;
		}
	}
	return false;
}

bool GSMFTP::rename(const String& oldName, const String& name, uint32_t timeout)
{
	uint32_t start = millis();
	if (_connected == false) {
		return false;
	}
	_fileRenamed= -2;
	
	while ((millis() - start) < timeout) {
		if (_fileRenamed == -2) {
			_fileRenamed = -1;
			MODEM.send("AT+UFTPC=3,\"" + oldName + "\",\"" + name + "\"");
			if (MODEM.waitForResponse(100) != 1) {
				return false;
			}
		}
		MODEM.poll();
		if (_fileRenamed == 0) {
			_fileRenamed = -2;
		}else if (_fileRenamed == 1) {
			return true;
		}
	}
	return false;
}

bool GSMFTP::download(const String& localFileName, const String& remoteFileName, uint32_t timeout)
{
	uint32_t start = millis();
	if (_connected == false) {
		return false;
	}
	_fileDownloaded = -2;

	while ((millis() - start) < timeout) {
		if (_fileDownloaded == -2) {
			_fileDownloaded = -1;
			MODEM.send("AT+UFTPC=4,\"" + remoteFileName + "\",\"" + localFileName + "\"");
			if (MODEM.waitForResponse(100) != 1) {
				return false;
			}
		}
		MODEM.poll();
		if (_fileDownloaded == 0) {
			_fileDownloaded = -2;
		}else if (_fileDownloaded == 1) {
			return true;
		}
	}
	return false;
}

bool GSMFTP::downloadStart(const String& localFileName, const String& remoteFileName)
{
  if (_connected == false) {
    return false;
  }
  //get remote file informations
	GSMFTPElem remoteFile;
  ls(remoteFile, remoteFileName);
	_downloadRemoteFileSize = remoteFile.elem(0).size;
  _fileDownloaded = -2;

  MODEM.send("AT+UFTPC=4,\"" + remoteFileName + "\",\"" + localFileName + "\"");
  if (MODEM.waitForResponse(100) != 1) {
    return false;
  }
  return true;
}

int GSMFTP::downloadReady(const String& localFileName, bool showProgression)
{
  if (_connected == false) {
    return -1;
  }
  MODEM.poll();

  if (_fileDownloaded == 0) {
    _fileDownloaded = -2;
    if (showProgression == true) {
      Serial.println("Failed to download the file.");
    }
    return -1;
  }else if (_fileDownloaded == 1) {
    _fileDownloaded = -2;
    if (showProgression == true) {
      Serial.println("Download 100%");
    }
    return 1;
  }else if ((showProgression == true) && 
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

bool GSMFTP::upload(const String& localFileName, const String& remoteFileName, uint32_t timeout)
{
	uint32_t start = millis();
	if (_connected == false) {
		return false;
	}
	_fileUploaded = -2;

	while ((millis() - start) < timeout) {
		if (_fileUploaded == -2) {
			_fileUploaded = -1;
			MODEM.send("AT+UFTPC=5,\"" + localFileName + "\",\"" + remoteFileName + "\"");
			if (MODEM.waitForResponse(100) != 1) {
				return false;
			}
		}
		MODEM.poll();
		if (_fileUploaded == 0) {
			_fileUploaded = -2;
		}else if (_fileUploaded == 1) {
			return true;
		}
	}
	return false;
}

bool GSMFTP::uploadStart(const String& localFileName, const String& remoteFileName)
{
  if (_connected == false) {
    return false;
  }

  _fileUploaded = -2;
  MODEM.send("AT+UFTPC=5,\"" + localFileName + "\",\"" + remoteFileName + "\"");
  if (MODEM.waitForResponse(100) != 1) {
    return false;
  }
  return true;
}

int GSMFTP::uploadReady()
{
  if (_connected == false) {
    return -1;
  }
  MODEM.poll();

  if (_fileUploaded == 0) {
    _fileUploaded = -2;
    return -1;
  }else if (_fileUploaded == 1) {
		_fileUploaded = -2;
		return 1;
	}
  return 0;
}

bool GSMFTP::cd(const String& name, uint32_t timeout)
{
	uint32_t start = millis();
	if (_connected == false) {
		return false;
	}
	_dirChanged = -2;

	while ((millis() - start) < timeout) {
		if (_dirChanged == -2) {
			_dirChanged = -1;
			MODEM.send("AT+UFTPC=8,\"" + name + "\"");
			if (MODEM.waitForResponse(100) != 1) {
				return false;
			}
		}
		MODEM.poll();
		if (_dirChanged == 0) {
			_dirChanged = -2;
		}else if (_dirChanged == 1) {
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
	}else {
		res += "no response";
	}
	Serial.println(res);
}

//--- GSMFTPElem

GSMFTPElem::Elem GSMFTPElem::elem(uint16_t i)
{
	if (i < _count) {
		return _elem[i];
	}else {
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
	}else if (_count == 0) {
		int i = res.indexOf('"');
		if (i < 0) {
			return;
		}
		res = res.substring(i + 1);
	}
	Elem elem;
	for (int i = 0; i < 7; ++i){
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
	if (urc.startsWith("+UUFTPCR: 1,1")) {
		_connected = true;
	}
	else if (urc.startsWith("+UUFTPCR: 0,1")) {
		_connected = false;
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
  else if (urc.startsWith("+UUFTPCR: 13,")) {
    _fileInfo = nullptr;
  }

	if (_fileInfo != nullptr) {
		_fileInfo->parse(urc);
	}
}
