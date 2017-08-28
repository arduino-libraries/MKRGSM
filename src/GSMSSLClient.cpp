#include "GSMSSLClient.h"

GSMSSLClient::GSMSSLClient(bool synch) :
  GSMClient(synch)
{
}

GSMSSLClient::~GSMSSLClient()
{
}

int GSMSSLClient::connect(IPAddress ip, uint16_t port)
{
  return connectSSL(ip, port);
}

int GSMSSLClient::connect(const char* host, uint16_t port)
{
  return connectSSL(host, port);
}
