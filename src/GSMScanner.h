#ifndef _GSM_SCANNER_H_INCLUDED
#define _GSM_SCANNER_H_INCLUDED

#include "GSM.h"

class GSMScanner {

public:
  /** Constructor
      @param trace    if true, dumps all AT dialogue to Serial
      @return - 
  */
  GSMScanner(bool trace = false);

  /** begin (forces modem hardware restart, so we begin from scratch)
      @return Always returns IDLE status
  */
  GSM3_NetworkStatus_t begin();

  /** Read current carrier
      @return Current carrier
   */
  String getCurrentCarrier();

  /** Obtain signal strength
      @return Signal Strength
   */
  String getSignalStrength();

  /** Search available carriers
    @return A string with list of networks available
   */
  String readNetworks();
};

#endif
