#ifndef _GSM_CLIENT_H_INCLUDED
#define _GSM_CLIENT_H_INCLUDED

#include <Client.h>

#include "Modem.h"


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
  int ready();

  /** Connect to server by IP address
      @param (IPAddress)
      @param (uint16_t)
      @return returns 0 if last command is still executing, 1 success, 2 if there are no resources
   */
  int connect(IPAddress, uint16_t);
  int connectSSL(IPAddress, uint16_t);

  /** Connect to server by hostname
      @param host     Hostname
      @param port     Port
      @return returns 0 if last command is still executing, 1 success, 2 if there are no resources
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

  virtual void handleUrc(const String& urc);

private:
  int connect();

  bool _synch;
  int _socket;

  int _state;
  IPAddress _ip;
  const char* _host;
  uint16_t _port;
  bool _ssl;

  bool _writeSync;
  String _response;
  int _peek;
};

#endif
