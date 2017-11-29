/*
  This file is part of the MKR GSM library.
  Copyright (C) 2017  Arduino AG (http://www.arduino.cc/)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Modem.h"

#include "GSMScanner.h"

GSMScanner::GSMScanner(bool trace)
{
  if (trace) {
    MODEM.debug();
  }
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
