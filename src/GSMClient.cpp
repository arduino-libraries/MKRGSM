#include "GSMClient.h"

GSMClient::GSMClient(bool synch)
{

}

GSMClient::GSMClient(int socket, bool synch)
{
}

int GSMClient::ready()
{
  return 0;
}

int GSMClient::connect(IPAddress, uint16_t)
{
}

int GSMClient::connect(const char *host, uint16_t port)
{
  return 0;
}

void GSMClient::beginWrite(bool sync)
{
}

size_t GSMClient::write(uint8_t c)
{
  return write(&c);
}

size_t GSMClient::write(const uint8_t *buf)
{
  return write(buf, strlen((const char*)buf));
}

size_t GSMClient::write(const uint8_t*, size_t)
{
  return 0;
}

void GSMClient::endWrite(bool sync)
{
}

uint8_t GSMClient::connected()
{
  return 0;
}

GSMClient::operator bool()
{
  return false;
}

int GSMClient::read(uint8_t *buf, size_t size)
{
  return 0;
}

int GSMClient::read()
{
  return -1;
}

int GSMClient::available()
{
  return 0;
}

int GSMClient::peek()
{
  return -1;
}

void GSMClient::flush()
{
}

void GSMClient::stop()
{
}
