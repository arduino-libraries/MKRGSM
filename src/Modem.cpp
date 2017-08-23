#include "Modem.h"

ModemClass::ModemClass(Uart& uart, unsigned long baud) :
  _uart(&uart),
  _baud(baud),
  _atCommandState(AT_COMMAND_IDLE),
  _ready(1),
  _responseDataStorage(NULL)
{
  _buffer.reserve(64);
}

int ModemClass::begin(bool restart)
{
  _uart->begin(_baud);

  if (!autosense()) {
    return 0;
  }

  if (restart) {
    if (!reset()) {
      return 0;
    }

    if (!autosense()) {
      return 0;
    }
  }

  return 1;
}

void ModemClass::end()
{
  _uart->end();
}

int ModemClass::autosense(int timeout)
{
  for (unsigned long start = millis(); millis() < (start + timeout);) {
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

void ModemClass::send(const char* command)
{
  _uart->println(command);
  _uart->flush();
  _atCommandState = AT_COMMAND_IDLE;
  _ready = 0;
}

int ModemClass::waitForResponse(unsigned long timeout, String* responseDataStorage)
{
  _responseDataStorage = responseDataStorage;
  for (unsigned long start = millis(); millis() < (start + timeout);) {
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

Serial.write(c);

    _buffer += c;

    switch (_atCommandState) {
      case AT_COMMAND_IDLE:
      default: {
        int urcIndex = _buffer.lastIndexOf("AT");
        if (urcIndex != -1) {
          _atCommandState = AT_COMMAND_ECHOING;
        } else {
          urcIndex = _buffer.indexOf("\r\n\r\n");
        }

        if (urcIndex != -1) {
          _buffer.remove(urcIndex);
          _buffer.trim();

          if (_buffer.length()) {
Serial.println("UCR>>> ");
Serial.print("UCR>>> ");
Serial.println(_buffer);
Serial.println("UCR>>> ");
          }

          _buffer = "";
        }
        break;
      }

      case AT_COMMAND_ECHOING: {
        if (c == '\n') {
          _buffer = "";
          _atCommandState = AT_RECEIVING_RESPONSE;
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
            }
          }

          if (_ready != 0) {
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

ModemClass MODEM(SerialGSM, 115200);
