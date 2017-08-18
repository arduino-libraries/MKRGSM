#include "GSM_SMS.h"

GSM_SMS::GSM_SMS(bool synch)
{
}

size_t GSM_SMS::write(uint8_t c)
{
  return 0;
}

int GSM_SMS::beginSMS(const char* to)
{
  return 0;
}

int GSM_SMS::ready()
{
  return 0;
}

int GSM_SMS::endSMS()
{
  return 0;
}

int GSM_SMS::available()
{
  return 0;
}

int GSM_SMS::remoteNumber(char* number, int nlength)
{
  return 2;
}

int GSM_SMS::read()
{
  return -1;
}

int GSM_SMS::peek()
{
  return -1;
}

void GSM_SMS::flush()
{
}
