#ifndef _MODEM_INCLUDED_H
#define _MODEM_INCLUDED_H

#include <stdarg.h>
#include <stdio.h>

#include <Arduino.h>

class ModemUrcHandler {
public:
  virtual void handleUrc(const String& urc) = 0;
};

class ModemClass {
public:
  ModemClass(Uart& uart, unsigned long baud, int resetPin, int rtsPin, int ctsPin);

  int begin(bool restart = true);
  void end();

  void debug();

  int autosense(int timeout = 10000);

  int noop();
  int reset();

  size_t write(uint8_t c);

  void send(const char* command);
  void send(const String& command) { send(command.c_str()); }
  void sendf(const char *fmt, ...);

  int waitForResponse(unsigned long timeout = 100, String* responseDataStorage = NULL);
  int ready();
  void poll();
  void setResponseDataStorage(String* responseDataStorage);
  
  void addUrcHandler(ModemUrcHandler* handler);
  void removeUrcHandler(ModemUrcHandler* handler);

private:
  Uart* _uart;
  unsigned long _baud;
  int _resetPin;
  int _rtsPin;
  int _ctsPin;

  enum {
    AT_COMMAND_IDLE,
    AT_RECEIVING_RESPONSE
  } _atCommandState;
  int _ready;
  String _buffer;
  String* _responseDataStorage;

  #define MAX_URC_HANDLERS 10
  static bool _debug;
  static ModemUrcHandler* _urcHandlers[MAX_URC_HANDLERS];
};

extern ModemClass MODEM;

#endif
