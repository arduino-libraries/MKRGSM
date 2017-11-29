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
