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
  String command;
  command.reserve(29);

  command += "AT+UBANDSEL=";

  if (band == GSM_MODE_EGSM) {
    command += "900";
  } else if (band == GSM_MODE_DCS) {
    command += "1800";
  } else if (band == GSM_MODE_PCS) {
    command += "1900";
  } else if (band == GSM_MODE_EGSM_DCS) {
    command += "900,1900";
  } else if (band == GSM_MODE_GSM850_PCS) {
    command += "850,1900";
  } else if (band == GSM_MODE_GSM850_EGSM_DCS_PCS) {
    command += "800,850,900,1900";
  }

  for (int i = 0; i < 10; i++) {
    MODEM.send(command);
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
