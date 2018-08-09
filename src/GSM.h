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

#ifndef _GSM_H_INCLUDED
#define _GSM_H_INCLUDED

#include <Arduino.h>

enum GSM3_NetworkStatus_t { ERROR, IDLE, CONNECTING, GSM_READY, GPRS_READY, TRANSPARENT_CONNECTED, OFF};

enum {
  READY_STATE_CHECK_SIM,
  READY_STATE_WAIT_CHECK_SIM_RESPONSE,
  READY_STATE_UNLOCK_SIM,
  READY_STATE_WAIT_UNLOCK_SIM_RESPONSE,
  READY_STATE_SET_PREFERRED_MESSAGE_FORMAT,
  READY_STATE_WAIT_SET_PREFERRED_MESSAGE_FORMAT_RESPONSE,
  READY_STATE_SET_HEX_MODE,
  READY_STATE_WAIT_SET_HEX_MODE,
  READY_STATE_SET_AUTOMATIC_TIME_ZONE,
  READY_STATE_WAIT_SET_AUTOMATIC_TIME_ZONE_RESPONSE,
  READY_STATE_ENABLE_DTMF_DETECTION,
  READY_STATE_WAIT_ENABLE_DTMF_DETECTION_RESPONSE,
  READY_STATE_CHECK_REGISTRATION,
  READY_STATE_WAIT_CHECK_REGISTRATION_RESPONSE,
  READY_STATE_SET_REPORTING_CALL_STATUS,
  READY_STATE_WAIT_SET_REPORTING_CALL_STATUS,
  READY_STATE_DONE
};

class GSM {

public:
  /** Constructor
      @param debug    Determines debug mode
    */
  GSM(bool debug = false);

  /** Start the GSM/GPRS modem, attaching to the GSM network
      @param pin         SIM PIN number (4 digits in a string, example: "1234"). If
                         NULL the SIM has no configured PIN.
      @param restart     Restart the modem. Default is TRUE. The modem receives
                         a signal through the Ctrl/D7 pin. If it is shut down, it will
                         start-up. If it is running, it will restart. Takes up to 10
                         seconds
      @param synchronous If TRUE the call only returns after the Start is complete
                         or fails. If FALSE the call will return inmediately. You have
                         to call repeatedly ready() until you get a result. Default is TRUE.
      @return If synchronous, GSM3_NetworkStatus_t. If asynchronous, returns 0.
    */
  GSM3_NetworkStatus_t begin(const char* pin = 0, bool restart = true, bool synchronous = true);

  /** Check network access status
      @return 1 if Alive, 0 if down
   */
  int isAccessAlive();

  /** Shutdown the modem (power off really)
      @return true if successful
    */
  bool shutdown();

  /** Secure shutdown the modem (power off really)
      @return always true
    */
  bool secureShutdown();

  /** Get last command status
      @return returns 0 if last command is still executing, 1 success, >1 error
    */
  int ready();

  unsigned long getTime();

  int lowPowerMode();
  int noLowPowerMode();

  GSM3_NetworkStatus_t status();
  int getReadyState();

private:
  GSM3_NetworkStatus_t _state;
  int _readyState;
  const char* _pin;
  String _response;
};

#endif
