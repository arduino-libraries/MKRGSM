#ifndef _MODEM_INCLUDED_H
#define _MODEM_INCLUDED_H

#include <Arduino.h>

class ModemClass {
public:
  ModemClass(Uart& uart, unsigned long baud);

  int begin(bool restart = true);
  void end();

  int autosense(int timeout = 10000);

  int noop();
  int reset();

  void send(const char* command);
  void send(const String& command) { send(command.c_str()); }
  int waitForResponse(unsigned long timeout = 100, String* responseDataStorage = NULL);
  int ready();
  void poll();
  void setResponseDataStorage(String* responseDataStorage);

private:
  Uart* _uart;
  unsigned long _baud;

  enum {
    AT_COMMAND_IDLE,
    AT_COMMAND_ECHOING,
    AT_RECEIVING_RESPONSE
  } _atCommandState;
  int _ready;
  String _buffer;
  String* _responseDataStorage;
};

extern ModemClass MODEM;

#endif
