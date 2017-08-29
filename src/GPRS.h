#ifndef _GPRS_H_INCLUDED
#define _GPRS_H_INCLUDED

#include <IPAddress.h>
#include "GSM.h"

#include "Modem.h"

enum {
  GPRS_PING_DEST_UNREACHABLE = -1, 
  GPRS_PING_TIMEOUT = -2,
  GPRS_PING_UNKNOWN_HOST = -3,
  GPRS_PING_ERROR = -4
};

class GPRS : public ModemUrcHandler {

public:

  GPRS();
  virtual ~GPRS();

  /** Attach to GPRS/GSM network
      @param networkId    APN GPRS
      @param user         Username
      @param pass         Password
      @return connection status
   */
  GSM3_NetworkStatus_t networkAttach(char* networkId, char* user, char* pass)
  {
      return attachGPRS(networkId, user, pass);
  };

  /** Detach GPRS/GSM network
      @return connection status
   */
  GSM3_NetworkStatus_t networkDetach(){ return detachGPRS(); };

  /** Attach to GPRS service
      @param apn          APN GPRS
      @param user_name    Username
      @param password     Password
      @param synchronous  Sync mode
      @return connection status
   */
  GSM3_NetworkStatus_t attachGPRS(char* apn, char* user_name, char* password, bool synchronous = true);

  /** Detach GPRS service
      @param synchronous  Sync mode
      @return connection status
   */
  GSM3_NetworkStatus_t detachGPRS(bool synchronous = true);

  /** Returns 0 if last command is still executing
      @return 1 if success, >1 if error 
   */
  int ready();

  /** Get actual assigned IP address in IPAddress format
      @return IP address in IPAddress format
   */
  IPAddress getIPAddress();


  int hostByName(const char* hostname, IPAddress& result);
  int hostByName(const String &hostname, IPAddress& result) { return hostByName(hostname.c_str(), result); }

  int ping(const char* hostname, uint8_t ttl = 128);
  int ping(const String& hostname, uint8_t ttl = 128);
  int ping(IPAddress ip, uint8_t ttl = 128);

  void handleUrc(const String& urc);

private:
  const char* _apn;
  const char* _username;
  const char* _password;
  int _state;
  GSM3_NetworkStatus_t _status;
  String _response;
  int _pingResult;
};

#endif
