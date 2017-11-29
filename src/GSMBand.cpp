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

#include "GSMBand.h"

GSMBand::GSMBand(bool trace)
{
  if (trace) {
    MODEM.debug();
  }
}

GSM3_NetworkStatus_t GSMBand::begin()
{
  return (GSM3_NetworkStatus_t)MODEM.begin();
}

String GSMBand::getBand()
{
  String response;

  MODEM.send("AT+UBANDSEL?");

  if (MODEM.waitForResponse(100, &response) == 1) {
    if (response.startsWith("+UBANDSEL: ")) {
      response.remove(0, 11);

      if (response == "900") {
        return GSM_MODE_EGSM;
      } else if (response == "1800") {
        return GSM_MODE_DCS;
      } else if (response == "1900") {
        return GSM_MODE_PCS;
      } else if (response == "900,1800") {
        return GSM_MODE_EGSM_DCS;
      } else if (response == "850,1900") {
        return GSM_MODE_GSM850_PCS;
      } else if (response == "850,900,1800,1900") {
        return GSM_MODE_GSM850_EGSM_DCS_PCS;
      }
    }
  }

  return GSM_MODE_UNDEFINED;
}
    
bool GSMBand::setBand(String band)
{
  const char* bands;

  if (band == GSM_MODE_EGSM) {
    bands = "900";
  } else if (band == GSM_MODE_DCS) {
    bands = "1800";
  } else if (band == GSM_MODE_PCS) {
    bands = "1900";
  } else if (band == GSM_MODE_EGSM_DCS) {
    bands = "900,1900";
  } else if (band == GSM_MODE_GSM850_PCS) {
    bands = "850,1900";
  } else if (band == GSM_MODE_GSM850_EGSM_DCS_PCS) {
    bands = "800,850,900,1900";
  } else {
    return false;
  }

  for (int i = 0; i < 10; i++) {
    MODEM.sendf("AT+UBANDSEL=%s", bands);
    int result = MODEM.waitForResponse(10000);
    
    if (result == 1) {
      return true;
    } else if (result == 2) {
      return false;
    }

    // retry ...
    delay(100);
  }

  return false;
}
