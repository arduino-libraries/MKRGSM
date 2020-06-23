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

#ifndef _GSM_CLIENT_H_INCLUDED
#define _GSM_CLIENT_H_INCLUDED

#include "Modem.h"

#include <Client.h>

class GSMClient : public Client, public ModemUrcHandler {

public:

  /** Constructor
      @param synch    Sync mode
   */
  GSMClient(bool synch = true);

  /** Constructor
      @param socket   Socket
      @param synch    Sync mode
   */
  GSMClient(int socket, bool synch);

  virtual ~GSMClient();

  /** Get last command status
      @return returns 0 if last command is still executing, 1 success, >1 error
  */
  virtual int ready();

  /** Connect to server by IP address
      @param (IPAddress)
      @param (uint16_t)
      @return returns 0 on failure, 1 on success (sync mode)
                      0 if last command is still executing, 1 success, 2 if there are no resources (async mode)   */
  int connect(IPAddress, uint16_t);
  int connectSSL(IPAddress, uint16_t);

  /** Connect to server by hostname
      @param host     Hostname
      @param port     Port
      @return returns 0 on failure, 1 on success (sync mode)
                      0 if last command is still executing, 1 success, 2 if there are no resources (async mode)
   */
  int connect(const char *host, uint16_t port);
  int connectSSL(const char *host, uint16_t port);

  /** Initialize write in request
      @param sync     Sync mode
   */
  void beginWrite(bool sync = false);

  /** Write a character in request
      @param c      Character
      @return size
   */
  size_t write(uint8_t c);

  /** Write a characters buffer in request
      @param buf      Buffer
      @return buffer size
   */
  size_t write(const uint8_t *buf);

  /** Write a characters buffer with size in request
      @param (uint8_t*) Buffer
      @param (size_t)   Buffer size
      @return buffer size
   */
  size_t write(const uint8_t*, size_t);

  /** Finish write request
      @param sync     Sync mode
   */
  void endWrite(bool sync = false);

  /** Check if connected to server
      @return 1 if connected
   */
  uint8_t connected();

  operator bool();

  /** Read from response buffer and copy size specified to buffer
      @param buf      Buffer    
      @param size     Buffer size
      @return bytes read
   */
  int read(uint8_t *buf, size_t size);

  /** Read a character from response buffer
      @return character
   */
  int read();

  /** Check if exists a response available
      @return 1 if exists, 0 if not exists
   */
  int available();

  /** Read a character from response buffer but does not move the pointer.
      @return character
   */
  int peek();

  /** Flush response buffer
   */
  void flush();

  /** Stop client
   */
  void stop();

  /** Set ssl profile
   */
  void setCertificateValidationLevel(uint8_t ssl);

  virtual void handleUrc(const String& urc);

private:
  int connect();

  bool _synch;
  int _socket;
  int _connected;

  int _state;
  IPAddress _ip;
  const char* _host;
  uint16_t _port;
  bool _ssl;

  int _sslprofile;
  bool _writeSync;
  String _response;
};

#endif
