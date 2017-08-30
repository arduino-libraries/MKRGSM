#include "Modem.h"

#include "GSMPIN.h"

GSMPIN::GSMPIN()
{
}

void GSMPIN::begin()
{
  MODEM.begin();
}

int GSMPIN::isPIN()
{
  String response;

  for (unsigned long start = millis(); millis() < (start + 1000);) {
    MODEM.send("AT+CPIN?");

    if (MODEM.waitForResponse(10000, &response) == 1) {
      if (response.startsWith("+CPIN: ")) {
        if (response.endsWith("READY")) {
          return 0;
        } else if (response.endsWith("SIM PIN")) {
          return 1;
        } else if (response.endsWith("SIM PUK")) {
          return -1;
        } else {
          return -2;
        }
      }
    }

    delay(100);
  }

  return -2;
}

int GSMPIN::checkPIN(String pin)
{
  MODEM.sendf("AT+CPIN=\"%s\"", pin.c_str());
  if (MODEM.waitForResponse(10000) == 1) {
    return 0;
  }

  return -1;
}


int GSMPIN::checkPUK(String puk, String pin)
{
  MODEM.sendf("AT+CPIN=\"%s\",\"%s\"", puk.c_str(), pin.c_str());
  if (MODEM.waitForResponse(10000) == 1) {
    return 0;
  }

  return -1;
}

void GSMPIN::changePIN(String old, String pin)
{
#warning "changePIN not implemented"
}

void GSMPIN::switchPIN(String pin)
{
#warning "switchPIN not implemented"
}

int GSMPIN::checkReg()
{
  for (unsigned long start = millis(); millis() < (start + 10000L);) {
    MODEM.send("AT+CREG?");

    String response = "";

    if (MODEM.waitForResponse(100, &response) == 1) {
      if (response.startsWith("+CREG: ")) {
        if (response.endsWith(",1")) {
          return 0;
        } else if (response.endsWith(",5")) {
          return 1;
        }
      }
    }

    delay(100);
  }

  return -1;
}

bool GSMPIN::getPINUsed()
{
#warning "getPINUsed not implemented"

  return false;
}

void GSMPIN::setPINUsed(bool used)
{
#warning "setPINUsed not implemented"

}
