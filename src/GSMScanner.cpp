#include "Modem.h"

#include "GSMScanner.h"

GSMScanner::GSMScanner(bool trace)
{
}

GSM3_NetworkStatus_t GSMScanner::begin()
{
  MODEM.begin();

  return IDLE;
}

String GSMScanner::getCurrentCarrier()
{
  String response;

  MODEM.send("AT+COPS?");
  if (MODEM.waitForResponse(180000, &response) == 1) {
    int firstQuoteIndex = response.indexOf('"');
    int lastQuoteIndex = response.lastIndexOf('"');

    if (firstQuoteIndex != -1 && lastQuoteIndex != -1 && firstQuoteIndex != lastQuoteIndex) {
      return response.substring(firstQuoteIndex + 1, lastQuoteIndex);
    }
  }

  return "";
}

String GSMScanner::getSignalStrength()
{
  String response;

  MODEM.send("AT+CSQ");
  if (MODEM.waitForResponse(100, &response) == 1) {
    int firstSpaceIndex = response.indexOf(' ');
    int lastCommaIndex = response.lastIndexOf(',');

    if (firstSpaceIndex != -1 && lastCommaIndex != -1) {
      return response.substring(firstSpaceIndex + 1, lastCommaIndex);
    }
  }

  return "";
}

String GSMScanner::readNetworks()
{
  String response;

  MODEM.send("AT+COPS=?");
  if (MODEM.waitForResponse(180000, &response) == 1) {
    String result;
    unsigned int responseLength = response.length();

    for(unsigned int i = 0; i < responseLength; i++) {
      for (; i < responseLength; i++) {
        if (response[i] == '"') {
          result += "> ";
          break;
        }
      }

      for (i++; i < responseLength; i++) {
        if (response[i] == '"') {
          result += '\n';
          break;
        }

        result += response[i];
      }

      for (i++; i < responseLength; i++) {
        if (response[i] == ')') {
          break;
        }
      }
    }

    return result;
  }

  return "";
}
