#include "Modem.h"

#include "GSM.h"

enum {
  READY_STATE_CHECK_SIM,
  READY_STATE_WAIT_CHECK_SIM_RESPONSE,
  READY_STATE_UNLOCK_SIM,
  READY_STATE_WAIT_UNLOCK_SIM_RESPONSE,
  READY_STATE_SET_PREFERRED_MESSAGE_FORMAT,
  READY_STATE_WAIT_SET_PREFERRED_MESSAGE_FORMAT_RESPONSE,
  READY_STATE_CHECK_REGISTRATION,
  READY_STATE_WAIT_CHECK_REGISTRATION_RESPONSE,
  READY_STATE_SET_CALLING_LINE_IDENTIFICATION_PRESENTATION,
  READY_STATE_WAIT_CALLING_LINE_IDENTIFICATION_PRESENTATION_RESPONSE,
  READY_STATE_SET_CONNECTED_LINE_IDENTIFICATION_PRESENTATION,
  READY_STATE_WAIT_CONNECTED_LINE_IDENTIFICATION_PRESENTATION_RESPONSE,
  READY_STATE_DONE
};

GSM::GSM(bool debug) :
  _state(ERROR),
  _readyState(0),
  _pin(NULL)
{

}

GSM3_NetworkStatus_t GSM::begin(const char* pin, bool restart, bool synchronous)
{
  if (!MODEM.begin(restart)) {
    _state = ERROR;
  } else {
    _pin = pin;
    _state = IDLE;
    _readyState = READY_STATE_CHECK_SIM;

    if (synchronous) {
      while (ready() == 0) {
        delay(100);
      }
    } else {
      return (GSM3_NetworkStatus_t)0;
    }
  }

  return _state;
}

int GSM::isAccessAlive()
{
  return (MODEM.noop() == 1 ? 1 : 0);
}

bool GSM::shutdown()
{
  return false;
}

bool GSM::secureShutdown()
{
  return true;
}

int GSM::ready()
{
  if (_state == ERROR) {
    return 2;
  }

  int ready = MODEM.ready();

  if (ready == 0) {
    return 0;
  }

  switch (_readyState) {
    case READY_STATE_CHECK_SIM: {
      MODEM.setResponseDataStorage(&_response);
      MODEM.send("AT+CPIN?");
      _readyState = READY_STATE_WAIT_CHECK_SIM_RESPONSE;
      ready = 0;
      break;
    }

    case READY_STATE_WAIT_CHECK_SIM_RESPONSE: {
      if (ready > 1) {
        // error => retry
        _readyState = READY_STATE_CHECK_SIM;
        ready = 0;
      } else {
        if (_response.endsWith("READY")) {
          _readyState = READY_STATE_SET_PREFERRED_MESSAGE_FORMAT;
          ready = 0;
        } else if (_response.endsWith("SIM PIN")) {
          _readyState = READY_STATE_UNLOCK_SIM;
          ready = 0;
        } else {
          _state = ERROR;
          ready = 2;
        }
      }

      break;
    }

    case READY_STATE_UNLOCK_SIM: {
      if (_pin != NULL) {
        String command;
        command.reserve(10 + strlen(_pin));

        command += "AT+CPIN=\"";
        command += _pin;
        command += "\"";

        MODEM.setResponseDataStorage(&_response);
        MODEM.send("AT+CPIN?");

        _readyState = READY_STATE_WAIT_UNLOCK_SIM_RESPONSE;
        ready = 0;
      } else {
        _state = ERROR;
        ready = 2;
      }
      break;
    }

    case READY_STATE_WAIT_UNLOCK_SIM_RESPONSE: {
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_SET_PREFERRED_MESSAGE_FORMAT;
        ready = 0;
      }

      break;
    }

    case READY_STATE_SET_PREFERRED_MESSAGE_FORMAT: {
      MODEM.send("AT+CMGF=1");
      _readyState = READY_STATE_WAIT_SET_PREFERRED_MESSAGE_FORMAT_RESPONSE;
      ready = 0;
      break; 
    }

    case READY_STATE_WAIT_SET_PREFERRED_MESSAGE_FORMAT_RESPONSE: {
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_CHECK_REGISTRATION;
        ready = 0;
      }

      break;
    }

    case READY_STATE_CHECK_REGISTRATION: {
      MODEM.setResponseDataStorage(&_response);
      MODEM.send("AT+CREG?");
      _readyState = READY_STATE_WAIT_CHECK_REGISTRATION_RESPONSE;
      ready = 0;
      break; 
    }

    case READY_STATE_WAIT_CHECK_REGISTRATION_RESPONSE: {
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        int status = _response.charAt(_response.length() - 1) - '0';

        if (status == 0 || status == 4) {
          _readyState = READY_STATE_CHECK_REGISTRATION;
          ready = 0;
        } else if (status == 1 || status == 5 || status == 8) {
          _readyState = READY_STATE_SET_CALLING_LINE_IDENTIFICATION_PRESENTATION;
          _state = GSM_READY;
        } else if (status == 2) {
          _readyState = READY_STATE_CHECK_REGISTRATION;
          _state = CONNECTING;
        } else if (status == 3) {
          _state = ERROR;
          ready = 2;
        } 
      }

      break;
    }

    case READY_STATE_SET_CALLING_LINE_IDENTIFICATION_PRESENTATION: {
      MODEM.send("AT+CLIP=1");
      _readyState = READY_STATE_WAIT_CALLING_LINE_IDENTIFICATION_PRESENTATION_RESPONSE;
      ready = 0;
      break; 
    }

    case READY_STATE_WAIT_CALLING_LINE_IDENTIFICATION_PRESENTATION_RESPONSE: {
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_SET_CONNECTED_LINE_IDENTIFICATION_PRESENTATION;
        ready = 0;
      }

      break;
    }

    case READY_STATE_SET_CONNECTED_LINE_IDENTIFICATION_PRESENTATION: {
      MODEM.send("AT+COLP=1");
      _readyState = READY_STATE_WAIT_CONNECTED_LINE_IDENTIFICATION_PRESENTATION_RESPONSE;
      ready = 0;
      break; 
    }

    case READY_STATE_WAIT_CONNECTED_LINE_IDENTIFICATION_PRESENTATION_RESPONSE: {
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_DONE;
        ready = 1;
      }

      break;
    }

    case READY_STATE_DONE:
      break;
  }

  return ready;
}
