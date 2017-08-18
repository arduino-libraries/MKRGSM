#include "GPRS.h"

GSM3_NetworkStatus_t GPRS::attachGPRS(char* apn, char* user_name, char* password, bool synchronous)
{
  return ERROR;
}

GSM3_NetworkStatus_t GPRS::detachGPRS(bool synchronous)
{
  return ERROR;
}

int GPRS::ready()
{
  return 0;
}

IPAddress GPRS::getIPAddress()
{
  return IPAddress(0, 0, 0, 0);
}