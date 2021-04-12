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

bool GSMFileSystem::ls(GSMFileSystemElem& file, bool show, uint32_t timeout)
{
	uint32_t start = millis();
	String response;
  file.clear();
	MODEM.send("AT+ULSTFILE=");

	if (MODEM.waitForResponse(timeout, &response) == 1) {
		file.parse(response);
	}else {
		return false;
	}

	for (int i = 0; i < file.count(); ++i){
		if ((millis() - start) > timeout) {
      return false;
		}
    file.setSize(i, size(file.elem(i).name));
		
		if (show == true) {
			file.show(i);
		}
	}
	return true;
}

int32_t GSMFileSystem::size(const String& name) 
{
  String response;

  MODEM.send("AT+ULSTFILE=2,\"" + name + "\"");

  if (MODEM.waitForResponse(1000, &response) == 1) {
    if (response.startsWith("+ULSTFILE: ")) {
      return response.substring(11).toInt();
    }
  }
  return -1;
}

bool GSMFileSystem::remove(const String& name)
{
	MODEM.send("AT+UDELFILE=\"" + name + "\"");
	if (MODEM.waitForResponse(10000) == 1) {	
		return true;
	}else {
		return false;
	}
}

bool GSMFileSystem::remove(GSMFileSystemElem& file)
{
	bool ok = true;
	for (int i = 0; i < file.count(); ++i) {
		ok &= remove(file.elem(i).name);
	}
	return ok;
}

uint32_t GSMFileSystem::freeSpace()
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

bool GSMFileSystem::write(const String& fileName, void* data, size_t size)
{
	MODEM.send("AT+UDWNFILE=\"" + fileName + "\"," + size);

	if (MODEM.waitForPrompt() == 1) {
		MODEM.write((const uint8_t*)data, size);
		if (MODEM.waitForResponse(10000) == 1) {
			return true;
		}
	}
	return false;
}

bool GSMFileSystem::read(const String& fileName, void* data, size_t size)
{
	String response;
	MODEM.send("AT+URDFILE=\"" + fileName + "\"");

	if (MODEM.waitForResponse(10000, &response) == 1) {
		memcpy(data, response.c_str(), size);
		return true;
	}
	return false;
}

//--- GSMFileSystemElem ---

GSMFileSystemElem::Elem GSMFileSystemElem::elem(uint16_t i)
{
  if (i < _count) {
    return _elem[i];
  }
  else {
    return Elem();
  }
}

void GSMFileSystemElem::setSize(uint16_t i, uint32_t size)
{
  if (i < _count) {
    _elem[i].size = size;
  }
}

void GSMFileSystemElem::append(const Elem elem)
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

void GSMFileSystemElem::clear() {
	if (_elem != nullptr) {
		delete[] _elem;
    _elem = nullptr;
	}
	_count = 0;
}

void GSMFileSystemElem::show(int i)
{
	if (i >= _count) {
		return;
	}
	Serial.print(_elem[i].name);
	Serial.print(" ");
	Serial.println(_elem[i].size);
}

void GSMFileSystemElem::parse(const String& str)
{
	String res = str;
	int i = res.indexOf('"')+1;
	int j = res.indexOf('"', i);

	while ((i > 0) && (j > 0)){
		Elem elem;
		elem.name = res.substring(i, j);
		res = res.substring(j + 1);
		append(elem);
		i = res.indexOf('"') + 1;
		j = res.indexOf('"', i);
	}
}

GSMFileSystem FILESYSTEM;
