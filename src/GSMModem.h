#ifndef _GSM_MODEM_H_INCLUDED
#define _GSM_MODEM_H_INCLUDED

#include <Arduino.h>

#include "GSM.h"

class GSMModem {

public:

  /** Constructor */
  GSMModem();

  /** Check modem response and restart it
   */
  int begin();

  /** Obtain modem IMEI (command AT)
      @return modem IMEI number
   */
  String getIMEI();
};

#endif
