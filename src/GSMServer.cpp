#include "GSMServer.h"

GSMServer::GSMServer(uint8_t port, bool synch)
{
}

int GSMServer::ready()
{
  return 0;
}

void GSMServer::begin()
{
}

GSMClient GSMServer::available(bool synch)
{
}


void GSMServer::beginWrite()
{
}

size_t GSMServer::write(uint8_t c)
{
  return write(&c, sizeof(c));
}

size_t GSMServer::write(const uint8_t *buf)
{
  return write(buf, strlen((const char*)buf));
}

size_t GSMServer::write(const uint8_t *buf, size_t sz)
{
  return 0;
}

void GSMServer::endWrite()
{
}

void GSMServer::stop()
{
}
