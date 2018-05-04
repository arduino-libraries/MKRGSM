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

#include "GSMServer.h"

enum {
  SERVER_STATE_IDLE,
  SERVER_STATE_CREATE_SOCKET,
  SERVER_STATE_WAIT_CREATE_SOCKET_RESPONSE,
  SERVER_STATE_LISTEN,
  SERVER_STATE_WAIT_LISTEN_RESPONSE,
  SERVER_STATE_CLOSE_SOCKET,
  SERVER_STATE_WAIT_CLOSE_SOCKET
};

GSMServer::GSMServer(uint16_t port, bool synch) :
  _port(port),
  _synch(synch),
  _socket(-1),
  _state(SERVER_STATE_IDLE)
{
  for (int i = 0; i < MAX_CHILD_SOCKETS; i++) {
    _childSockets[i].socket = -1;
    _childSockets[i].accepted = false;
    _childSockets[i].available = 0;
  }

  MODEM.addUrcHandler(this);
}

GSMServer::~GSMServer()
{
  MODEM.removeUrcHandler(this);
}

int GSMServer::ready()
{
  int ready = MODEM.ready();

  if (ready == 0) {
    return 0;
  }

  switch (_state) {
    case SERVER_STATE_IDLE:
    default: {
      break;
    }

    case SERVER_STATE_CREATE_SOCKET: {
      MODEM.setResponseDataStorage(&_response);
      MODEM.send("AT+USOCR=6");

      _state = SERVER_STATE_WAIT_CREATE_SOCKET_RESPONSE;
      ready = 0;
      break;
    }

    case SERVER_STATE_WAIT_CREATE_SOCKET_RESPONSE: {
      if (ready > 1 || !_response.startsWith("+USOCR: ")) {
        _state = SERVER_STATE_IDLE;
      } else {
        _socket = _response.charAt(_response.length() - 1) - '0';

        _state = SERVER_STATE_LISTEN;
        ready = 0;
      }
      break;
    }

    case SERVER_STATE_LISTEN: {
      MODEM.sendf("AT+USOLI=%d,%d", _socket, _port);

      _state = SERVER_STATE_WAIT_LISTEN_RESPONSE;
      ready = 0;
      break;
    }

    case SERVER_STATE_WAIT_LISTEN_RESPONSE: {
      if (ready > 1) {
        _state = SERVER_STATE_CLOSE_SOCKET;
        ready = 0;
      } else {
        _state = SERVER_STATE_IDLE;
      }
      break;
    }

    case SERVER_STATE_CLOSE_SOCKET: {
      MODEM.sendf("AT+USOCL=%d", _socket);

      _state = SERVER_STATE_WAIT_CLOSE_SOCKET;
      ready = 0;
      break;
    }

    case SERVER_STATE_WAIT_CLOSE_SOCKET: {
      _state = SERVER_STATE_IDLE;
      _socket = -1;
      break;
    }
  }

  return ready;
}

void GSMServer::begin()
{
  _state = SERVER_STATE_CREATE_SOCKET;

  if (_synch) {
    while (ready() == 0);
  }
}

GSMClient GSMServer::available(bool synch)
{
  MODEM.poll();

  int socket = -1;

  if (_socket != -1) {
    // search for new accepted sockets first
    for (int i = 0; i < MAX_CHILD_SOCKETS; i++) {
      if (_childSockets[i].socket != -1 && _childSockets[i].accepted) {
        _childSockets[i].accepted = false;
        socket = _childSockets[i].socket;

        break;
      }
    }

    if (socket == -1) {
      // no new accepted sockets, search for one with data to be read
      for (int i = 0; i < MAX_CHILD_SOCKETS; i++) {
        if (_childSockets[i].socket != -1) {
          // check if socket is still alive
          MODEM.sendf("AT+USORD=%d,0", _childSockets[i].socket);
          if (MODEM.waitForResponse(10000) != 1) {
            // closed
            _childSockets[i].socket = -1;
            _childSockets[i].accepted = false;
            _childSockets[i].available = 0;

            continue;
          }

          if (_childSockets[i].available) {
            _childSockets[i].available = 0;
            socket = _childSockets[i].socket;
            break;
          }
        }
      }
    }
  }

  return GSMClient(socket, synch);
}

void GSMServer::beginWrite()
{
}

size_t GSMServer::write(uint8_t c)
{
  return write(&c, sizeof(c));
}

size_t GSMServer::write(const uint8_t *buf)
{
  return write(buf, strlen((const char*)buf));
}

size_t GSMServer::write(const uint8_t *buf, size_t sz)
{
  size_t written = 0;

  MODEM.poll();

  if (_socket != -1) {
    for (int i = 0; i < MAX_CHILD_SOCKETS; i++) {
      if (_childSockets[i].socket != -1) {
        GSMClient client(_childSockets[i].socket, true);

        written += client.write(buf, sz);
      }
    }
  }

  return written;
}

void GSMServer::endWrite()
{
}

void GSMServer::stop()
{
  if (_socket < 0) {
    return;
  }

  MODEM.sendf("AT+USOCL=%d", _socket);
  MODEM.waitForResponse(10000);

  _socket = -1;
}

void GSMServer::handleUrc(const String& urc)
{
  if (urc.startsWith("+UUSOLI: ")) {
    int socket = urc.charAt(9) - '0';

    for (int i = 0; i < MAX_CHILD_SOCKETS; i++) {
      if (_childSockets[i].socket == -1) {
        _childSockets[i].socket = socket;
        _childSockets[i].accepted = true;
        _childSockets[i].available = 0;

        break;
      }
    }
  } else if (urc.startsWith("+UUSOCL: ")) {
    int socket = urc.charAt(urc.length() - 1) - '0';

    if (socket == _socket) {
      _socket = -1;
    } else {
      for (int i = 0; i < MAX_CHILD_SOCKETS; i++) {
        if (_childSockets[i].socket == socket) {
          _childSockets[i].socket = -1;
          _childSockets[i].accepted = false;
          _childSockets[i].available = 0;

          break;
        }
      }
    }
  } else if (urc.startsWith("+UUSORD: ")) {
    int socket = urc.charAt(9) - '0';

    for (int i = 0; i < MAX_CHILD_SOCKETS; i++) {
      if (_childSockets[i].socket == socket) {
        int commaIndex = urc.indexOf(',');
        if (commaIndex != -1) {
          _childSockets[i].available = urc.substring(commaIndex + 1).toInt();
        }

        break;
      }
    }
  }
}
