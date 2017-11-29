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

#ifndef _GSM_PIN_H_INCLUDED
#define _GSM_PIN_H_INCLUDED

#include <Arduino.h>

class GSMPIN {

public:

  /** Constructor */
  GSMPIN();

  /** Check modem response and restart it
   */
  void begin();

  /** Check if PIN lock or PUK lock is activated
      @return 0 if PIN lock is off, 1 if PIN lock is on, -1 if PUK lock is on, -2 if error exists
   */
  int isPIN();

  /** Check if PIN code is correct and valid
      @param pin      PIN code
      @return 0 if is correct, -1 if is incorrect
   */
  int checkPIN(String pin);

  /** Check if PUK code is correct and establish new PIN code
      @param puk      PUK code
      @param pin      New PIN code
      @return 0 if successful, otherwise return -1
   */
  int checkPUK(String puk, String pin);

  /** Change PIN code
      @param old      Old PIN code
      @param pin      New PIN code
   */
  void changePIN(String old, String pin);

  /** Change PIN lock status
      @param pin      PIN code
   */
  void switchPIN(String pin);

  /** Check if modem was registered in GSM/GPRS network
      @return 0 if modem was registered, 1 if modem was registered in roaming, -1 if error exists
   */
  int checkReg();

  /** Return if PIN lock is used
      @return true if PIN lock is used, otherwise, return false
   */
  bool getPINUsed();

  /** Set PIN lock status
      @param used     New PIN lock status
   */
  void setPINUsed(bool used);

private:
  bool _pinUsed;
};

#endif
