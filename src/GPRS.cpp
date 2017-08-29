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
  MODEM.addUrcHandler(this);
}

GPRS::~GPRS()
{
  MODEM.removeUrcHandler(this);
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

int GPRS::hostByName(const char* hostname, IPAddress& result)
{
  String response;
  String command;
  command.reserve(14 + strlen(hostname));

  command += "AT+UDNSRN=0,\"";
  command += hostname;
  command += "\"";

  MODEM.send(command);

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
  String command;
  command.reserve(25 + strlen(hostname));

  command += "AT+UPING=\"";
  command += hostname;
  command += "\",1,32,5000,"; // retries, size, timeout
  command += ttl;
  
  _pingResult = 0;

  MODEM.send(command);
  if (MODEM.waitForResponse() != 1) {
    return GPRS_PING_ERROR;
  };

  while (_pingResult == 0) {
    MODEM.poll();
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
  }
}
