#include "Modem.h"

#include "GPRS.h"

enum {
  GPRS_STATE_IDLE,
  GPRS_STATE_ATTACH,
  GPRS_STATE_WAIT_ATTACH_RESPONSE,
  GPRS_STATE_SET_APN,
  GPRS_STATE_WAIT_SET_APN_RESPONSE,
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
  _status(IDLE)
{

}

GSM3_NetworkStatus_t GPRS::attachGPRS(char* apn, char* user_name, char* password, bool synchronous)
{
  _apn = apn;
  _username = user_name;
  _password = password;

  _state = GPRS_STATE_ATTACH;
  _status = CONNECTING;

  if (synchronous) {
    while (ready() == 0) {
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
      String command;
      command.reserve(14 + strlen(_apn));

      command += "AT+UPSD=0,1,\"";
      command += _apn;
      command += "\"";

      MODEM.send(command);
      _state = GPRS_STATE_WAIT_SET_APN_RESPONSE;
      ready = 0;
      break;
    }

    case GPRS_STATE_WAIT_SET_APN_RESPONSE: {
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
      String command;
      command.reserve(14 + strlen(_username));

      command += "AT+UPSD=0,2,\"";
      command += _username;
      command += "\"";

      MODEM.send(command);
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
      String command;
      command.reserve(14 + strlen(_password));

      command += "AT+UPSD=0,3,\"";
      command += _password;
      command += "\"";

      MODEM.send(command);
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
      MODEM.send("AT+CGATT=1");
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
  return IPAddress(0, 0, 0, 0);
}