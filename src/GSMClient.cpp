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

#include "GSMClient.h"

enum {
  CLIENT_STATE_IDLE,
  CLIENT_STATE_CREATE_SOCKET,
  CLIENT_STATE_WAIT_CREATE_SOCKET_RESPONSE,
  CLIENT_STATE_ENABLE_SSL,
  CLIENT_STATE_WAIT_ENABLE_SSL_RESPONSE,
  CLIENT_STATE_CONNECT,
  CLIENT_STATE_WAIT_CONNECT_RESPONSE,
  CLIENT_STATE_CLOSE_SOCKET,
  CLIENT_STATE_WAIT_CLOSE_SOCKET
};

GSMClient::GSMClient(bool synch) :
  GSMClient(-1, synch)
{
}

GSMClient::GSMClient(int socket, bool synch) :
  _synch(synch),
  _socket(socket),
  _state(CLIENT_STATE_IDLE),
  _ip((uint32_t)0),
  _host(NULL),
  _port(0),
  _ssl(false),
  _writeSync(true),
  _peek(-1),
  _available(0)
{
  MODEM.addUrcHandler(this);
}

GSMClient::~GSMClient()
{
  MODEM.removeUrcHandler(this);
}

int GSMClient::ready()
{
  int ready = MODEM.ready();

  if (ready == 0) {
    return 0;
  }

  switch (_state) {
    case CLIENT_STATE_IDLE:
    default: {
      break;
    }

    case CLIENT_STATE_CREATE_SOCKET: {
      MODEM.setResponseDataStorage(&_response);
      MODEM.send("AT+USOCR=6");

      _state = CLIENT_STATE_WAIT_CREATE_SOCKET_RESPONSE;
      ready = 0;
      break;
    }

    case CLIENT_STATE_WAIT_CREATE_SOCKET_RESPONSE: {
      if (ready > 1 || !_response.startsWith("+USOCR: ")) {
        _state = CLIENT_STATE_IDLE;
      } else {
        _socket = _response.charAt(_response.length() - 1) - '0';

        if (_ssl) {
          _state = CLIENT_STATE_ENABLE_SSL;
        } else {
          _state = CLIENT_STATE_CONNECT;
        }

        ready = 0;
      }
      break;
    }

    case CLIENT_STATE_ENABLE_SSL: {
      MODEM.sendf("AT+USOSEC=%d,1", _socket);

      _state = CLIENT_STATE_WAIT_ENABLE_SSL_RESPONSE;
      ready = 0;
      break;
    }

    case CLIENT_STATE_WAIT_ENABLE_SSL_RESPONSE: {
      if (ready > 1) {
        _state = CLIENT_STATE_CLOSE_SOCKET;
      } else {
        _state = CLIENT_STATE_CONNECT;
      }

      ready = 0;
      break;
    }

    case CLIENT_STATE_CONNECT: {
      if (_host != NULL) {
        MODEM.sendf("AT+USOCO=%d,\"%s\",%d", _socket, _host, _port);
      } else {
        MODEM.sendf("AT+USOCO=%d,\"%d.%d.%d.%d\",%d", _socket, _ip[0], _ip[1], _ip[2], _ip[3], _port);
      }

      _state = CLIENT_STATE_WAIT_CONNECT_RESPONSE;
      ready = 0;
      break;
    }

    case CLIENT_STATE_WAIT_CONNECT_RESPONSE: {
      if (ready > 1) {
        _state = CLIENT_STATE_CLOSE_SOCKET;

        ready = 0;
      } else {
        _state = CLIENT_STATE_IDLE;
      }
      break;
    }

    case CLIENT_STATE_CLOSE_SOCKET: {

      MODEM.sendf("AT+USOCL=%d", _socket);

      _state = CLIENT_STATE_WAIT_CLOSE_SOCKET;
      ready = 0;
      break;
    }

    case CLIENT_STATE_WAIT_CLOSE_SOCKET: {
      _state = CLIENT_STATE_IDLE;
      _socket = -1;
      break;
    }
  }

  return ready;
}

int GSMClient::connect(IPAddress ip, uint16_t port)
{
  _ip = ip;
  _host = NULL;
  _port = port;
  _ssl = false;

  return connect();
}

int GSMClient::connectSSL(IPAddress ip, uint16_t port)
{
  _ip = ip;
  _host = NULL;
  _port = port;
  _ssl = true;

  return connect();
}

int GSMClient::connect(const char *host, uint16_t port)
{
  _ip = (uint32_t)0;
  _host = host;
  _port = port;
  _ssl = false;

  return connect();
}

int GSMClient::connectSSL(const char *host, uint16_t port)
{
  _ip = (uint32_t)0;
  _host = host;
  _port = port;
  _ssl = true;

  return connect();
}

int GSMClient::connect()
{
  if (_synch) {
    while (ready() == 0);
  } else if (ready() == 0) {
    return 0;
  }

  _state = CLIENT_STATE_CREATE_SOCKET;

  if (_synch) {
    while (ready() == 0) {
      delay(100);
    }

    if (_socket == -1) {
      return 2;
    }
  }

  return 1;
}

void GSMClient::beginWrite(bool sync)
{
  _writeSync = sync;
}

size_t GSMClient::write(uint8_t c)
{
  return write(&c, 1);
}

size_t GSMClient::write(const uint8_t *buf)
{
  return write(buf, strlen((const char*)buf));
}

size_t GSMClient::write(const uint8_t* buf, size_t size)
{
  if (_writeSync) {
    while (ready() == 0);
  } else if (ready() == 0) {
    return 0;
  }

  if (_socket == -1) {
    return 0;
  }

  size_t written = 0;
  String command;


  while (size) {
    size_t chunkSize = size;

    if (chunkSize > 256) {
      chunkSize = 256;
    }

    command.reserve(19 + chunkSize * 2);

    command += "AT+USOWR=";
    command += _socket;
    command += ",";
    command += chunkSize;
    command += ",\"";

    for (size_t i = 0; i < chunkSize; i++) {
      byte b = buf[i + written];

      byte n1 = (b >> 4) & 0x0f;
      byte n2 = (b & 0x0f);

      command += (char)(n1 > 9 ? 'A' + n1 - 10 : '0' + n1);
      command += (char)(n2 > 9 ? 'A' + n2 - 10 : '0' + n2);
    }

    command += "\"";

    MODEM.send(command);
    if (_writeSync) {
      if (MODEM.waitForResponse(10000) != 1) {
        break;
      }
    }

    written += chunkSize;
    size -= chunkSize;
  }

  return written;
}

void GSMClient::endWrite(bool /*sync*/)
{
  _writeSync = true;
}

uint8_t GSMClient::connected()
{
  MODEM.poll();

  if (_socket == -1) {
    return 0;
  }

  return 1;
}

GSMClient::operator bool()
{
  return (_socket != -1);
}

int GSMClient::read(uint8_t *buf, size_t size)
{
  if (_socket == -1) {
    return 0;
  }

  if (size == 0) {
    return 0;
  }

  if (size > 512) {
    size = 512;
  }

  String response;

  MODEM.sendf("AT+USORD=%d,%d", _socket, size);
  if (MODEM.waitForResponse(10000, &response) != 1) {
    return 0;
  }

  if (!response.startsWith("+USORD: ")) {
    return 0;
  }

  int firstQuoteIndex = response.indexOf("\"");

  response.remove(0, firstQuoteIndex + 1);
  response.remove(response.length() - 1);

  size = response.length() / 2;

  for (size_t i = 0; i < size; i++) {
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

    buf[i] = (n1 << 4) | n2;
  }

  _available = 0;
  MODEM.poll();

  return size;
}

int GSMClient::read()
{
  byte b;

  if (_peek != -1) {
    b = _peek;
    _peek = -1;
    return b;
  }

  if (read(&b, 1) == 1) {
    return b;
  }

  return -1;
}

int GSMClient::available()
{
  if (_synch) {
    while (ready() == 0);
  } else if (ready() == 0) {
    return 0;
  }

  if (_socket == -1) {
    return 0;
  }

  MODEM.poll();

  return _available;
}

int GSMClient::peek()
{
  if (_peek == -1) {
    _peek = read();
  }

  return _peek;
}

void GSMClient::flush()
{
}

void GSMClient::stop()
{
  if (_socket < 0) {
    return;
  }

  MODEM.sendf("AT+USOCL=%d", _socket);
  MODEM.waitForResponse(10000);

  _socket = -1;
}

void GSMClient::handleUrc(const String& urc)
{
  if (urc.startsWith("+UUSOCL: ")) {
    int socket = urc.charAt(urc.length() - 1) - '0';

    if (socket == _socket) {
      // this socket closed
      _socket = -1;
      _available = 0;
    }
  } else if (urc.startsWith("+UUSORD: ")) {
    int socket = urc.charAt(9) - '0';

    if (socket == _socket) {
      if (urc.endsWith(",4294967295")) {
        // SSL disconnect
        // this socket closed
        _socket = -1;
        _available = 0;
      } else {
        int commaIndex = urc.indexOf(',');
        if (commaIndex != -1) {
          _available = urc.substring(commaIndex + 1).toInt();
        }
      }
    }
  }
}
