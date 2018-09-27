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

#include "Modem.h"

bool ModemClass::_debug = false;
ModemUrcHandler* ModemClass::_urcHandlers[MAX_URC_HANDLERS] = { NULL };

ModemClass::ModemClass(Uart& uart, unsigned long baud, int resetPin, int dtrPin) :
  _uart(&uart),
  _baud(baud),
  _resetPin(resetPin),
  _dtrPin(dtrPin),
  _lowPowerMode(false),
  _atCommandState(AT_COMMAND_IDLE),
  _ready(1),
  _responseDataStorage(NULL)
{
  _buffer.reserve(64);
}

int ModemClass::begin(bool restart)
{
  _uart->begin(_baud > 115200 ? 115200 : _baud);

  if (_resetPin > -1 && restart) {
    pinMode(_resetPin, OUTPUT);
    digitalWrite(_resetPin, HIGH);
    delay(100);
    digitalWrite(_resetPin, LOW);
  } else {
    if (!autosense()) {
      return 0;
    }

    if (!reset()) {
      return 0;
    }
  }

  if (!autosense()) {
    return 0;
  }

  if (_baud > 115200) {
    sendf("AT+IPR=%ld", _baud);
    if (waitForResponse() != 1) {
      return 0;
    }

    _uart->end();
    delay(100);
    _uart->begin(_baud);

    if (!autosense()) {
      return 0;
    }
  }

  if (_dtrPin > -1) {
    pinMode(_dtrPin, OUTPUT);
    noLowPowerMode();

    send("AT+UPSV=3");
    if (waitForResponse() != 1) {
      return 0;
    }
  }

  return 1;
}

void ModemClass::end()
{
  _uart->end();
  digitalWrite(_resetPin, HIGH);

  if (_dtrPin > -1) {
    digitalWrite(_dtrPin, LOW);
  }
}

void ModemClass::debug()
{
  _debug = true;
}

void ModemClass::noDebug()
{
  _debug = false;
}

int ModemClass::autosense(unsigned int timeout)
{
  for (unsigned long start = millis(); (millis() - start) < timeout;) {
    if (noop() == 1) {
      return 1;
    }

    delay(100);
  }

  return 0;
}

int ModemClass::noop()
{
  send("AT");

  return (waitForResponse() == 1);
}

int ModemClass::reset()
{
  send("AT+CFUN=16");

  return (waitForResponse(1000) == 1);
}

int ModemClass::lowPowerMode()
{
  if (_dtrPin > -1) {
    _lowPowerMode = true;

    digitalWrite(_dtrPin, HIGH);

    return 1;
  }

  return 0;
}

int ModemClass::noLowPowerMode()
{
  if (_dtrPin > -1) {
    _lowPowerMode = false;

    digitalWrite(_dtrPin, LOW);

    return 1;
  }

  return 0;
}

size_t ModemClass::write(uint8_t c)
{
  return _uart->write(c);
}

void ModemClass::send(const char* command)
{
  if (_lowPowerMode) {
    digitalWrite(_dtrPin, LOW);
    delay(5);
  }

  unsigned long dif=millis()-_uartMillis;
  if(dif<20) delay(20-dif);

  _uart->println(command);
  _uart->flush();
  _atCommandState = AT_COMMAND_IDLE;
  _ready = 0;
}

void ModemClass::sendf(const char *fmt, ...)
{
  char buf[BUFSIZ];

  va_list ap;
  va_start((ap), (fmt));
  vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
  va_end(ap);

  send(buf);
}

int ModemClass::waitForResponse(unsigned long timeout, String* responseDataStorage)
{
  _responseDataStorage = responseDataStorage;
  for (unsigned long start = millis(); (millis() - start) < timeout;) {
    int r = ready();

    if (r != 0) {
      _responseDataStorage = NULL;
      return r;
    }
  }

  _responseDataStorage = NULL;
  _buffer = "";
  return -1;
}

int ModemClass::ready()
{
  poll();

  return _ready;
}

void ModemClass::poll()
{
  while (_uart->available()) {
    char c = _uart->read();
    _uartMillis=millis();

    if (_debug) {
      Serial.write(c);
    }

    _buffer += c;

    switch (_atCommandState) {
      case AT_COMMAND_IDLE:
      default: {

        if (_buffer.startsWith("AT") && _buffer.endsWith("\r\n")) {
          _atCommandState = AT_RECEIVING_RESPONSE;
          _buffer = "";
        }  else if (_buffer.endsWith("\r\n")) {
          _buffer.trim();

          if (_buffer.length()) {
            for (int i = 0; i < MAX_URC_HANDLERS; i++) {
              if (_urcHandlers[i] != NULL) {
                _urcHandlers[i]->handleUrc(_buffer);
              }
            }
          }

          _buffer = "";
        }

        break;
      }

      case AT_RECEIVING_RESPONSE: {
        if (c == '\n') {
          int responseResultIndex = _buffer.lastIndexOf("OK\r\n");
          if (responseResultIndex != -1) {
            _ready = 1;
          } else {
            responseResultIndex = _buffer.lastIndexOf("ERROR\r\n");
            if (responseResultIndex != -1) {
              _ready = 2;
            } else {
              responseResultIndex = _buffer.lastIndexOf("NO CARRIER\r\n");
              if (responseResultIndex != -1) {
                _ready = 3;
              }
            }
          }

          if (_ready != 0) {
            if (_lowPowerMode) {
              digitalWrite(_dtrPin, HIGH);
            }

            if (_responseDataStorage != NULL) {
              _buffer.remove(responseResultIndex);
              _buffer.trim();

              *_responseDataStorage = _buffer;

              _responseDataStorage = NULL;
            }

            _atCommandState = AT_COMMAND_IDLE;
            _buffer = "";
            return;
          }
        }
        break;
      }
    }
  }
}

void ModemClass::setResponseDataStorage(String* responseDataStorage)
{
  _responseDataStorage = responseDataStorage;
}

void ModemClass::addUrcHandler(ModemUrcHandler* handler)
{
  for (int i = 0; i < MAX_URC_HANDLERS; i++) {
    if (_urcHandlers[i] == NULL) {
      _urcHandlers[i] = handler;
      break;
    }
  }
}

void ModemClass::removeUrcHandler(ModemUrcHandler* handler)
{
  for (int i = 0; i < MAX_URC_HANDLERS; i++) {
    if (_urcHandlers[i] == handler) {
      _urcHandlers[i] = NULL;
      break;
    }
  }
}

ModemClass MODEM(SerialGSM, 921600, GSM_RESETN, GSM_DTR);
