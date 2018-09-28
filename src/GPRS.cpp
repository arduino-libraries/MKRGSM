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

#include "GPRS.h"

enum {
  GPRS_STATE_IDLE,
  GPRS_STATE_ATTACH,
  GPRS_STATE_WAIT_ATTACH_RESPONSE,
  GPRS_STATE_SET_APN,
  GPRS_STATE_WAIT_SET_APN_RESPONSE,
  GPRS_STATE_SET_AUTH_MODE,
  GPRS_STATE_WAIT_SET_AUTH_MODE_RESPONSE,
  GPRS_STATE_SET_USERNAME,
  GPRS_STATE_WAIT_SET_USERNAME_RESPONSE,
  GPRS_STATE_SET_PASSWORD,
  GPRS_STATE_WAIT_SET_PASSWORD_RESPONSE,
  GPRS_STATE_SET_DYNAMIC_IP,
  GPRS_STATE_WAIT_SET_DYNAMIC_IP_RESPONSE,
  GPRS_STATE_ACTIVATE_IP,
  GPRS_STATE_WAIT_ACTIVATE_IP_RESPONSE,
  GPRS_STATE_CHECK_PROFILE_STATUS,
  GPRS_STATE_WAIT_CHECK_PROFILE_STATUS_RESPONSE,

  GPRS_STATE_DEACTIVATE_IP,
  GPRS_STATE_WAIT_DEACTIVATE_IP_RESPONSE,
  GPRS_STATE_DEATTACH,
  GPRS_STATE_WAIT_DEATTACH_RESPONSE
};

GPRS::GPRS() :
  _apn(NULL),
  _username(NULL),
  _password(NULL),
  _status(IDLE),
  _timeout(0)
{
  MODEM.addUrcHandler(this);
}

GPRS::~GPRS()
{
  MODEM.removeUrcHandler(this);
}

GSM3_NetworkStatus_t GPRS::attachGPRS(const char* apn, const char* user_name, const char* password, bool synchronous)
{
  _apn = apn;
  _username = user_name;
  _password = password;

  _state = GPRS_STATE_ATTACH;
  _status = CONNECTING;

  if (synchronous) {
    unsigned long start = millis();

    while (ready() == 0) {
      if (_timeout && !((millis() - start) < _timeout)) {
        _state = ERROR;
        break;
      }

      delay(100);
    }
  } else {
    ready();
  }

  return _status;
}

GSM3_NetworkStatus_t GPRS::detachGPRS(bool synchronous)
{
  _state = GPRS_STATE_DEACTIVATE_IP;

  if (synchronous) {
    while (ready() == 0) {
      delay(100);
    }
  } else {
    ready();
  }

  return _status;
}

int GPRS::ready()
{
  int ready = MODEM.ready();

  if (ready == 0) {
    return 0;
  }

  switch (_state) {
    case GPRS_STATE_IDLE:
    default: {
      break;
    }

    case GPRS_STATE_ATTACH: {
      MODEM.send("AT+CGATT=1");
      _state = GPRS_STATE_WAIT_ATTACH_RESPONSE;
      ready = 0;
      break;
    }

    case GPRS_STATE_WAIT_ATTACH_RESPONSE: {
      if (ready > 1) {
        _state = GPRS_STATE_IDLE;
        _status = ERROR;
      } else {
        _state = GPRS_STATE_SET_APN;
        ready = 0;
      }
      break;
    }
    case GPRS_STATE_SET_APN: {
      MODEM.sendf("AT+UPSD=0,1,\"%s\"", _apn);
      _state = GPRS_STATE_WAIT_SET_APN_RESPONSE;
      ready = 0;
      break;
    }

    case GPRS_STATE_WAIT_SET_APN_RESPONSE: {
      if (ready > 1) {
        _state = GPRS_STATE_IDLE;
        _status = ERROR;
      } else {
        _state = GPRS_STATE_SET_AUTH_MODE;
        ready = 0;
      }
      break;
    }

    case GPRS_STATE_SET_AUTH_MODE: {
       MODEM.sendf("AT+UPSD=0,6,3");
      _state = GPRS_STATE_WAIT_SET_AUTH_MODE_RESPONSE;
      ready = 0;
      break;
    }

    case GPRS_STATE_WAIT_SET_AUTH_MODE_RESPONSE: {
      if (ready > 1) {
        _state = GPRS_STATE_IDLE;
        _status = ERROR;
      } else {
        _state = GPRS_STATE_SET_USERNAME;
        ready = 0;
      }
      break;
    }

    case GPRS_STATE_SET_USERNAME: {
      MODEM.sendf("AT+UPSD=0,2,\"%s\"", _username);
      _state = GPRS_STATE_WAIT_SET_USERNAME_RESPONSE;
      ready = 0;
      break;
    }

    case GPRS_STATE_WAIT_SET_USERNAME_RESPONSE: {
      if (ready > 1) {
        _state = GPRS_STATE_IDLE;
        _status = ERROR;
      } else {
        _state = GPRS_STATE_SET_PASSWORD;
        ready = 0;
      }
      break;
    }

    case GPRS_STATE_SET_PASSWORD: {
      MODEM.sendf("AT+UPSD=0,3,\"%s\"", _password);
      _state = GPRS_STATE_WAIT_SET_PASSWORD_RESPONSE;
      ready = 0;
      break;
    }

    case GPRS_STATE_WAIT_SET_PASSWORD_RESPONSE: {
      if (ready > 1) {
        _state = GPRS_STATE_IDLE;
        _status = ERROR;
      } else {
        _state = GPRS_STATE_SET_DYNAMIC_IP;
        ready = 0;
      }
      break;
    }

    case GPRS_STATE_SET_DYNAMIC_IP: {
      MODEM.send("AT+UPSD=0,7,\"0.0.0.0\"");
      _state = GPRS_STATE_WAIT_SET_DYNAMIC_IP_RESPONSE;
      ready = 0;
      break;
    }

    case GPRS_STATE_WAIT_SET_DYNAMIC_IP_RESPONSE: {
      if (ready > 1) {
        _state = GPRS_STATE_IDLE;
        _state = ERROR;
      } else {
        _state = GPRS_STATE_ACTIVATE_IP;
        ready = 0;
      }
      break;
    }

    case GPRS_STATE_ACTIVATE_IP: {
      MODEM.send("AT+UPSDA=0,3");
      _state = GPRS_STATE_WAIT_ACTIVATE_IP_RESPONSE;
      ready = 0;
      break;
    }

    case GPRS_STATE_WAIT_ACTIVATE_IP_RESPONSE: {
      if (ready > 1) {
        _state = GPRS_STATE_IDLE;
        _status = ERROR;
      } else {
        _state = GPRS_STATE_CHECK_PROFILE_STATUS;
        ready = 0;
      }
      break;
    }

    case GPRS_STATE_CHECK_PROFILE_STATUS: {
      MODEM.setResponseDataStorage(&_response);
      MODEM.send("AT+UPSND=0,8");
      _state = GPRS_STATE_WAIT_CHECK_PROFILE_STATUS_RESPONSE;
      ready = 0;
      break;
    }
    
    case GPRS_STATE_WAIT_CHECK_PROFILE_STATUS_RESPONSE: {
      if (ready > 1 || !_response.endsWith(",1")) {
        _state = GPRS_STATE_IDLE;
        _status = ERROR;
      } else {
        _state = GPRS_STATE_IDLE;
        _status = GPRS_READY;
      }
      break;
    }

    case GPRS_STATE_DEACTIVATE_IP: {
      MODEM.send("AT+UPSDA=0,4");
      _state = GPRS_STATE_WAIT_DEACTIVATE_IP_RESPONSE;
      ready = 0;
      break;
    }

    case GPRS_STATE_WAIT_DEACTIVATE_IP_RESPONSE: {
      if (ready > 1) {
        _state = GPRS_STATE_IDLE;
        _status = ERROR;
      } else {
        _state = GPRS_STATE_DEATTACH;
        ready = 0;
      }
      break;
    }

    case GPRS_STATE_DEATTACH: {
      MODEM.send("AT+CGATT=0");
      _state = GPRS_STATE_WAIT_DEATTACH_RESPONSE;
      ready = 0;
      break;
    }

    case GPRS_STATE_WAIT_DEATTACH_RESPONSE: {
      if (ready > 1) {
        _state = GPRS_STATE_IDLE;
        _status = ERROR;
      } else {
        _state = GPRS_STATE_IDLE;
        _status = IDLE;
      }
      break;
    }
  }

  return ready;
}

IPAddress GPRS::getIPAddress()
{
  String response;

  MODEM.send("AT+UPSND=0,0");
  if (MODEM.waitForResponse(100, &response) == 1) {
    if (response.startsWith("+UPSND: 0,0,\"") && response.endsWith("\"")) {
      response.remove(0, 13);
      response.remove(response.length() - 1);

      IPAddress ip;

      if (ip.fromString(response)) {
        return ip;
      }
    }
  }

  return IPAddress(0, 0, 0, 0);
}

void GPRS::setTimeout(unsigned long timeout)
{
  _timeout = timeout;
}

int GPRS::hostByName(const char* hostname, IPAddress& result)
{
  String response;

  MODEM.sendf("AT+UDNSRN=0,\"%s\"", hostname);
  if (MODEM.waitForResponse(70000, &response) != 1) {
    return 0;
  }

  if (!response.startsWith("+UDNSRN: \"") || !response.endsWith("\"")) {
    return 0;
  }

  response.remove(0, 10);
  response.remove(response.length() - 1);

  if (result.fromString(response)) {
    return 1;
  }

  return 0;
}

int GPRS::ping(const char* hostname, uint8_t ttl)
{
  String response;
  
  _pingResult = 0;

  MODEM.sendf("AT+UPING=\"%s\",1,32,5000,%d", hostname, ttl);
  if (MODEM.waitForResponse() != 1) {
    return GPRS_PING_ERROR;
  };

  for (unsigned long start = millis(); (millis() - start) < 5000 && (_pingResult == 0);) {
    MODEM.poll();
  }

  if (_pingResult == 0) {
    _pingResult = GPRS_PING_TIMEOUT;
  }

  return _pingResult;
}

int GPRS::ping(const String &hostname, uint8_t ttl)
{
  return ping(hostname.c_str(), ttl);
}

int GPRS::ping(IPAddress ip, uint8_t ttl)
{
  String host;
  host.reserve(15);

  host += ip[0];
  host += '.';
  host += ip[1];
  host += '.';
  host += ip[2];
  host += '.';
  host += ip[3];

  return ping(host, ttl);
}

GSM3_NetworkStatus_t GPRS::status()
{
  MODEM.poll();

  return _status;
}

void GPRS::handleUrc(const String& urc)
{
  if (urc.startsWith("+UUPINGER: ")) {
    if (urc.endsWith(",8")) {
      _pingResult = GPRS_PING_UNKNOWN_HOST;
    } else {
      _pingResult = GPRS_PING_ERROR;
    }
  } else if (urc.startsWith("+UUPING: ")) {
    int lastCommaIndex = urc.lastIndexOf(',');

    if (lastCommaIndex == -1) {
      _pingResult = GPRS_PING_ERROR;
    } else {
      _pingResult = urc.substring(lastCommaIndex + 1).toInt();

      if (_pingResult == -1) {
        _pingResult = GPRS_PING_TIMEOUT;
      } else if (_pingResult <= 0) {
        _pingResult = GPRS_PING_ERROR;
      }
    }
  } else if (urc.startsWith("+UUPSDD: ")) {
    int profileId = urc.charAt(urc.length() - 1) - '0';

    if (profileId == 0) {
      // disconnected
      _status = IDLE;
    }
  }
}
