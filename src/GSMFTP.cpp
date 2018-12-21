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

GSMFTP::GSMFTP(): 
	_connected(false),
	_dirCreated(-1),
	_dirChanged(-1),
	_fileRemoved(-1)
{
	MODEM.addUrcHandler(this);
}

GSMFTP::~GSMFTP()
{
  MODEM.removeUrcHandler(this);
	_file.clear();
}

bool GSMFTP::connect(String hostname, String user, String password)
{
	uint32_t start = millis();
	String command;

	command = "AT+UFTP=1,\"" + hostname + "\"";
	MODEM.send(command);
	if (MODEM.waitForResponse(100) != 1){
		return false;
	}

	command = "AT+UFTP=2,\"" + user + "\"";
	MODEM.send(command);
	if (MODEM.waitForResponse(100) != 1) {
		return false;
	}

	command = "AT+UFTP=3,\"" + password + "\"";
	MODEM.send(command);
	if (MODEM.waitForResponse(100) != 1) {
		return false;
	}

	MODEM.send("AT+UFTP=6,0");
	if (MODEM.waitForResponse(100) != 1) {
		return false;
	}
	
	command = "AT+UDNSRN=0,\"" + hostname + "\"";
	MODEM.send(command);
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

bool GSMFTP::ls(bool show, uint32_t timeout)
{
	if (_connected == true) {
		_file.ready = false;
		_file.clear();
		uint32_t start = millis();

		MODEM.send("AT+UFTPC=13");

		while (_file.ready == false) {
			MODEM.poll();
			if ((millis() - start) > timeout) {
				_file.ready = true;
				return false;
			}
		}

		if (show == true) {
			for (int i = 0; i < _file.count; ++i) {
				_file.show(i);
			}
		}
		return true;
	}
	else{
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
	String command = "AT+UFTPC=10,\"" + name + "\"";

	while ((millis() - start) < timeout) {
		if (_dirCreated == -2) {
			_dirCreated = -1;
			MODEM.send(command);
			if (MODEM.waitForResponse(100) != 1) {
				return false;
			}
		}
		MODEM.poll();
		if (_dirCreated == 0) {
			_dirCreated = -2;
			MODEM.send(command);
		}
		else if (_dirCreated == 1) {
			return true;
		}
	}
	return false;
}

bool GSMFTP::remove(const String& name, uint32_t timeout)
{
	uint32_t start = millis();
	if (_connected == false) {
		return false;
	}

	_fileRemoved = -2;
	String command = "AT+UFTPC=2,\"" + name + "\"";

	while ((millis() - start) < timeout) {
		if (_fileRemoved == -2) {
			_fileRemoved = -1;
			MODEM.send(command);
			if (MODEM.waitForResponse(100) != 1) {
				return false;
			}
		}
		MODEM.poll();
		if (_fileRemoved == 0) {
			_fileRemoved = -2;
			MODEM.send(command);
		}else if (_fileRemoved == 1) {
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
	String command = "AT+UFTPC=3,\"" + oldName + "\",\"" + name + "\"";
	
	while ((millis() - start) < timeout) {
		if (_fileRenamed == -2) {
			_fileRenamed = -1;
			MODEM.send(command);
			if (MODEM.waitForResponse(100) != 1) {
				return false;
			}
		}
		MODEM.poll();
		if (_fileRenamed == 0) {
			_fileRenamed = -2;
			MODEM.send(command);
		}
		else if (_fileRenamed == 1) {
			return true;
		}
	}
	return false;
}

bool GSMFTP::download(const String& remoteFileName, const String& localFileName, uint32_t timeout)
{
	uint32_t start = millis();
	if (_connected == false) {
		return false;
	}
	_fileDownloaded = -2;
	String command = "AT+UFTPC=4,\"" + remoteFileName + "\",\"" + localFileName + "\"";

	while ((millis() - start) < timeout) {
		if (_fileDownloaded == -2) {
			_fileDownloaded = -1;
			MODEM.send(command);
			if (MODEM.waitForResponse(100) != 1) {
				return false;
			}
		}
		MODEM.poll();
		if (_fileDownloaded == 0) {
			_fileDownloaded = -2;
			MODEM.send(command);
		}
		else if (_fileDownloaded == 1) {
			return true;
		}
	}
	return false;
}

bool GSMFTP::upload(const String& localFileName, const String& remoteFileName, uint32_t timeout)
{
	uint32_t start = millis();
	if (_connected == false) {
		return false;
	}
	_fileUploaded = -2;
	String command = "AT+UFTPC=5,\"" + localFileName + "\",\"" + remoteFileName + "\"";

	while ((millis() - start) < timeout) {
		if (_fileUploaded == -2) {
			_fileUploaded = -1;
			MODEM.send(command);
			if (MODEM.waitForResponse(100) != 1) {
				return false;
			}
		}
		MODEM.poll();
		if (_fileUploaded == 0) {
			_fileUploaded = -2;
			MODEM.send(command);
		}
		else if (_fileUploaded == 1) {
			return true;
		}
	}
	return false;
}

bool GSMFTP::cd(const String& name, uint32_t timeout)
{
	uint32_t start = millis();
	if (_connected == false) {
		return false;
	}
	_dirChanged = -2;
	String command = "AT+UFTPC=8,\"" + name + "\"";

	while ((millis() - start) < timeout) {
		if (_dirChanged == -2) {
			_dirChanged = -1;
			MODEM.send(command);
			if (MODEM.waitForResponse(100) != 1) {
				return false;
			}
		}
		MODEM.poll();
		if (_dirChanged == 0) {
			_dirChanged = -2;
			MODEM.send(command);
		}
		else if (_dirChanged == 1) {
			return true;
		}
	}
	return false;
}

GSMFTP::FTPFileElem GSMFTP::file(uint16_t i)
{
	if (i < _file.count) {
		return _file.e[i];
	}
	else {
		return FTPFileElem();
	}
}

void GSMFTP::FTPFile::append(const FTPFileElem& elem)
{
	FTPFileElem* tmp = new FTPFileElem[count + 1];
	for (int i = 0; i < count; ++i) {
		tmp[i] = e[i];
	}
	tmp[count] = elem;
	if (e != nullptr) {
		delete[] e;
	}
	e = tmp;
	count++;
}

void GSMFTP::FTPFile::clear() {
	if (e != nullptr) {
		delete[] e;
		e = nullptr;
	}
	count = 0;
}

void GSMFTP::FTPFile::show(int i)
{
	if (i >= count) {
		return;
	}
	
	Serial.print(e[i].permissions);
	Serial.print(" ");
	Serial.print(e[i].number);
	Serial.print(" ");
	Serial.print(e[i].user);
	Serial.print(" ");
	Serial.print(e[i].group);
	Serial.print(" ");
	Serial.print(e[i].size);
	Serial.print(" ");
	Serial.print(e[i].lastModified);
	Serial.print(" ");
	Serial.print(e[i].name);
	Serial.println();
}

void GSMFTP::FTPFile::parse(const String& str)
{
	if (str == "\"") {
		ready = true;
	}
	else {
		String res = str;
		if (count == 0) {
			int i = res.indexOf('"');
			if (i < 0) {
				ready = true;
				return;
			}
			res = res.substring(i + 1);
		}
		FTPFileElem elem;
		for (int i = 0; i < 7; ++i)
		{
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
		;
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
	else if (urc.startsWith("+UUFTPCR: 3,")) {
		_fileRenamed = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 1 : 0;
	}
	else if (urc.startsWith("+UUFTPCR: 4,")) {
		_fileDownloaded = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 1 : 0;
	}
	else if (urc.startsWith("+UUFTPCR: 5,")) {
		_fileUploaded = (urc.charAt(urc.lastIndexOf(",") + 1) == '1') ? 1 : 0;
	}

	if (_file.ready == false) {
		_file.parse(urc);
	}
}
