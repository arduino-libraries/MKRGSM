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

#include <Modem.h>

#include "GSMUdp.h"

GSMUDP::GSMUDP() :
  _socket(-1),
  _packetReceived(false),
  _txIp((uint32_t)0),
  _txHost(NULL),
  _txPort(0),
  _txSize(0),
  _rxIp((uint32_t)0),
  _rxPort(0),
  _rxSize(0),
  _rxIndex(0)
{
  MODEM.addUrcHandler(this);
}

GSMUDP::~GSMUDP()
{
  MODEM.removeUrcHandler(this);
}

uint8_t GSMUDP::begin(uint16_t port)
{
  String response;

  MODEM.send("AT+USOCR=17");

  if (MODEM.waitForResponse(100, &response) != 1) {
    return 0;
  }

  _socket = response.charAt(response.length() - 1) - '0';

  MODEM.sendf("AT+USOLI=%d,%d", _socket, port);
  if (MODEM.waitForResponse(10000) != 1) {
    stop();
    return 0;
  }

  return 1;
}

void GSMUDP::stop()
{
  if (_socket < 0) {
    return;
  }

  MODEM.sendf("AT+USOCL=%d", _socket);
  MODEM.waitForResponse(10000);

  _socket = -1;
}

int GSMUDP::beginPacket(IPAddress ip, uint16_t port)
{
  if (_socket < 0) {
    return 0;
  }

  _txIp = ip;
  _txHost = NULL;
  _txPort = port;
  _txSize = 0;

  return 1;
}

int GSMUDP::beginPacket(const char *host, uint16_t port)
{
  if (_socket < 0) {
    return 0;
  }
  
  _txIp = (uint32_t)0;
  _txHost = host;
  _txPort = port;
  _txSize = 0;

  return 1;
}

int GSMUDP::endPacket()
{
  String command;

  if (_txHost != NULL) {
    command.reserve(26 + strlen(_txHost) + _txSize * 2);
  } else {
    command.reserve(41 + _txSize * 2);
  }

  command += "AT+USOST=";
  command += _socket;
  command += ",\"";

  if (_txHost != NULL) {
    command += _txHost;
  } else {
    command += _txIp[0];
    command += '.';
    command += _txIp[1];
    command += '.';
    command += _txIp[2];
    command += '.';
    command += _txIp[3];
  }

  command += "\",";
  command += _txPort;
  command += ",",
  command += _txSize;
  command += ",\"";

  for (size_t i = 0; i < _txSize; i++) {
    byte b = _txBuffer[i];

    byte n1 = (b >> 4) & 0x0f;
    byte n2 = (b & 0x0f);

    command += (char)(n1 > 9 ? 'A' + n1 - 10 : '0' + n1);
    command += (char)(n2 > 9 ? 'A' + n2 - 10 : '0' + n2);
  }

  command += "\"";

  MODEM.send(command);

  if (MODEM.waitForResponse() == 1) {
    return 1;
  } else {
    return 0;
  }
}

size_t GSMUDP::write(uint8_t b)
{
  return write(&b, sizeof(b));
}

size_t GSMUDP::write(const uint8_t *buffer, size_t size)
{
  if (_socket < 0) {
    return 0;
  }

  size_t spaceAvailable = sizeof(_txBuffer) - _txSize;

  if (size > spaceAvailable) {
    size = spaceAvailable;
  }

  memcpy(&_txBuffer[_txSize], buffer, size);
  _txSize += size;

  return size;
}

int GSMUDP::parsePacket()
{
  MODEM.poll();

  if (_socket < 0) {
    return 0;
  }

  if (!_packetReceived) {
    return 0;
  }
  _packetReceived = false;

  String response;

  MODEM.sendf("AT+USORF=%d,%d", _socket, sizeof(_rxBuffer));
  if (MODEM.waitForResponse(10000, &response) != 1) {
    return 0;
  }

  if (!response.startsWith("+USORF: ")) {
    return 0;
  }

  response.remove(0, 11);

  int firstQuoteIndex = response.indexOf('"');
  if (firstQuoteIndex == -1) {
    return 0;
  }

  String ip = response.substring(0, firstQuoteIndex);
  _rxIp.fromString(ip);

  response.remove(0, firstQuoteIndex + 2);

  int firstCommaIndex = response.indexOf(',');
  if (firstCommaIndex == -1) {
    return 0;
  }

  String port = response.substring(0, firstCommaIndex);
  _rxPort = port.toInt();
  firstQuoteIndex = response.indexOf("\"");

  response.remove(0, firstQuoteIndex + 1);
  response.remove(response.length() - 1);

  _rxIndex = 0;
  _rxSize = response.length() / 2;

  for (size_t i = 0; i < _rxSize; i++) {
    byte n1 = response[i * 2];
    byte n2 = response[i * 2 + 1];

    if (n1 > '9') {
      n1 = (n1 - 'A') + 10;
    } else {
      n1 = (n1 - '0');
    }

    if (n2 > '9') {
      n2 = (n2 - 'A') + 10;
    } else {
      n2 = (n2 - '0');
    }

    _rxBuffer[i] = (n1 << 4) | n2;
  }

  MODEM.poll();

  return _rxSize;
}

int GSMUDP::available()
{
  if (_socket < 0) {
    return 0;
  }

  return (_rxSize - _rxIndex);
}

int GSMUDP::read()
{
  byte b;

  if (read(&b, sizeof(b)) == 1) {
    return b;
  }

  return -1;
}

int GSMUDP::read(unsigned char* buffer, size_t len)
{
  size_t readMax = available();

  if (len > readMax) {
    len = readMax;
  }

  memcpy(buffer, &_rxBuffer[_rxIndex], len);

  _rxIndex += len;

  return len;
}

int GSMUDP::peek()
{
  if (available() > 1) {
    return _rxBuffer[_rxIndex];
  }

  return -1;
}

void GSMUDP::flush()
{
}

IPAddress GSMUDP::remoteIP()
{
  return _rxIp;
}

uint16_t GSMUDP::remotePort()
{
  return _rxPort;
}

void GSMUDP::handleUrc(const String& urc)
{
  if (urc.startsWith("+UUSORF: ")) {
    int socket = urc.charAt(9) - '0';

    if (socket == _socket) {
      _packetReceived = true;
    }
  } else if (urc.startsWith("+UUSOCL: ")) {
    int socket = urc.charAt(urc.length() - 1) - '0';

    if (socket == _socket) {
      // this socket closed
      _socket = -1;
      _rxIndex = 0;
      _rxSize = 0;
    }
  }
}
