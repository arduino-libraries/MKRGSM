#ifndef _GSM_SSL_CLIENT_H_INCLUDED
#define _GSM_SSL_CLIENT_H_INCLUDED

#include "GSMClient.h"

class GSMSSLClient : public GSMClient {

public:
  GSMSSLClient(bool synch = true);
  virtual ~GSMSSLClient();

  virtual int connect(IPAddress ip, uint16_t port);
  virtual int connect(const char* host, uint16_t port);
};

#endif
