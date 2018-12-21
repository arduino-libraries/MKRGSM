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

#include <GSMFileSystem.h>
#include <Modem.h>

GSMFileSytem::GSMFileSytem()
{
}

GSMFileSytem::~GSMFileSytem()
{
	_file.clear();
}

bool GSMFileSytem::ls(bool show, uint32_t timeout)
{
	uint32_t start = millis();
	_file.clear();
	String response;
	MODEM.send("AT+ULSTFILE=");

	if (MODEM.waitForResponse(timeout, &response) == 1) {
		_file.parse(response);
	}else {
		return false;
	}

	for (int i = 0; i < _file.count; ++i){
		if ((millis() - start) > timeout) {
			return false;
		}
		String command = "AT+ULSTFILE=2,\"" + _file.e[i].name + "\"";
		MODEM.send(command);
		
		if (MODEM.waitForResponse(1000, &response) == 1) {
			if (response.startsWith("+ULSTFILE: ")) {
				_file.e[i].size = response.substring(11).toInt();
			}
		}
		if (show == true) {
			_file.show(i);
		}
	}
		
	return true;
}

bool GSMFileSytem::remove(const String& name)
{
	String command = "AT+UDELFILE=\"" + name + "\"";
	MODEM.send(command);
	if (MODEM.waitForResponse(10000) == 1) {	
		return true;
	}else {
		return false;
	}
}

uint32_t GSMFileSytem::freeSpace()
{
	uint32_t res = 0;
	String response;
	MODEM.send("AT+ULSTFILE=1");

	if (MODEM.waitForResponse(1000, &response) == 1) {
		if (response.startsWith("+ULSTFILE: ")) {
			res = response.substring(11).toInt();
		}
	}
	return res;
}

bool GSMFileSytem::write(const String& fileName, void* data, size_t size)
{
	String command = "AT+UDWNFILE=\"" + fileName + "\"," + size;
	MODEM.send(command);

	if (MODEM.waitForPrompt() == 1) {
		MODEM.write((const uint8_t*)data, size);
		if (MODEM.waitForResponse(10000) == 1) {
			return true;
		}
	}
	return false;
}

bool GSMFileSytem::read(const String& fileName, void* data, size_t size)
{
	String response;
	String command = "AT+URDFILE=\"" + fileName + "\"";
	MODEM.send(command);

	if (MODEM.waitForResponse(10000, &response) == 1) {
		memcpy(data, response.c_str(), size);
		return true;
	}
	return false;
}

GSMFileSytem::FileElem GSMFileSytem::file(uint16_t i)
{
	if (i < _file.count) {
		return _file.e[i];
	}else {
		return FileElem();
	}
}

void GSMFileSytem::File::append(const FileElem& elem)
{
	FileElem* tmp = new FileElem[count + 1];
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

void GSMFileSytem::File::clear() {
	if (e != nullptr) {
		delete[] e;
		e = nullptr;
	}
	count = 0;
}

void GSMFileSytem::File::show(int i)
{
	if (i >= count) {
		return;
	}
	Serial.print(e[i].name);
	Serial.print(" ");
	Serial.println(e[i].size);
}

void GSMFileSytem::File::parse(const String& str)
{
	String res = str;
	int i = res.indexOf('"')+1;
	int j = res.indexOf('"', i);

	while ((i > 0) && (j > 0)){
		FileElem elem;
		elem.name = res.substring(i, j);
		res = res.substring(j + 1);
		append(elem);
		i = res.indexOf('"') + 1;
		j = res.indexOf('"', i);
	}
}
