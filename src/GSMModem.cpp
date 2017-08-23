#include "Modem.h"

#include "GSMModem.h"

GSMModem::GSMModem()
{
}

int GSMModem::begin()
{
  if (!MODEM.begin()) {
    return 0;
  }

  return 1;
}

String GSMModem::getIMEI()
{
  String imei;

  imei.reserve(15);

  MODEM.send("AT+CGSN");
  MODEM.waitForResponse(100, &imei);

  return imei;
}
