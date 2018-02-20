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

#ifndef _GSM_SERVER_H_INCLUDED
#define _GSM_SERVER_H_INCLUDED

#include <Server.h>
#include "GSMClient.h"

#include "Modem.h"

class GSMServer : public Server, public ModemUrcHandler {

public:

  /** Constructor
      @param port     Port
      @param synch    True if the server acts synchronously
   */
  GSMServer(uint16_t port, bool synch = true);

  ~GSMServer();

  /** Get last command status
      @return returns 0 if last command is still executing, 1 success, >1 error
   */
  int ready();

  /** Initialize server
   */
  void begin();

  /** Check if there is an incoming client request
      @param synch      If true, the returned client is synchronous or
                        blocking.
      @return Client if successful, else error
  */
  GSMClient available(bool synch = true);

  // Just to keep in line with Ethernet.
  // Write to every open socket...
  //void write(uint8_t);
  //void write(const uint8_t *buf, size_t size);

  /** Begin write in socket
   */
  void beginWrite();

  /** Write character in socket
      @param c     Character
      @return size
   */
  size_t write(uint8_t c);

  /** Write buffer in socket
      @param buf   Buffer
      @return size
   */
  size_t write(const uint8_t *buf);

  /** Write buffer in socket with size
      @param buf      Buffer
      @param sz       Buffer size
      @return size
   */
  size_t write(const uint8_t *buf, size_t sz);

  /** End write in socket
   */
  void endWrite();

  /** Stop server
   */
  void stop();

  virtual void handleUrc(const String& urc);

private:
  uint16_t _port;
  bool _synch;

  int _socket;
  int _state;
  String _response;

  #define MAX_CHILD_SOCKETS 6
  struct {
    int socket;
    bool accepted;
    int available;
  } _childSockets[MAX_CHILD_SOCKETS];
};

#endif
