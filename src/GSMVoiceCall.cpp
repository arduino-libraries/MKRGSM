#include "GSMVoiceCall.h"

GSMVoiceCall::GSMVoiceCall(bool synch) :
  _synch(synch),
  _callStatus(IDLE_CALL)
{
  MODEM.addUrcHandler(this);
}

GSMVoiceCall::~GSMVoiceCall()
{
  MODEM.removeUrcHandler(this);
}

GSM3_voiceCall_st GSMVoiceCall::getvoiceCallStatus()
{
  MODEM.poll();

  return _callStatus;
}

int GSMVoiceCall::ready()
{
  return MODEM.ready();
}
    
int GSMVoiceCall::voiceCall(const char* to, unsigned long timeout)
{
  if (_synch) {
    while (ready() == 0);
  } else {
    int r = ready();

    if (r == 0) {
      return 0;
    }
  }

  String command;
  command.reserve(4 + strlen(to));

  command += "ATD";
  command += to;
  command += ";";

  MODEM.send(command);

  if (_synch) {
    if (MODEM.waitForResponse(180000) != 1) {
      return 0;
    }

    _callStatus = CALLING;
    for (unsigned long start = millis(); (timeout == 0) || (millis() < (start + timeout));) {
      if (getvoiceCallStatus() != CALLING) {
        break;
      }
    }

    if (getvoiceCallStatus() != TALKING) {
      hangCall();
      return 0;
    }

    return 1;
  } else {
    return ready();
  }
}
   
int GSMVoiceCall::answerCall()
{
  if (_synch) {
    while (ready() == 0);
  } else {
    int r = ready();

    if (r == 0) {
      return 0;
    }
  }

  MODEM.send("ATA");

  if (_synch) {
    if (MODEM.waitForResponse(20000) == 1) {
      return 1;
    }
  }

  return 0;
}
    
int GSMVoiceCall::hangCall()
{
  if (_synch) {
    while (ready() == 0);
  } else {
    int r = ready();

    if (r == 0) {
      return 0;
    }
  }

  MODEM.send("ATH");

  if (_synch) {
    if (MODEM.waitForResponse(200000) == 1) {
      return 1;
    }
  }

  return 0;
}

int GSMVoiceCall::retrieveCallingNumber(char* buffer, int bufsize)
{
  String response;

  if (_synch) {
    while (ready() == 0);
  } else {
    int r = ready();

    if (r == 0) {
      return 0;
    }
  }

  MODEM.send("AT+CLCC");
  if (MODEM.waitForResponse(100, &response) == 1) {
    int phoneNumberStartIndex = response.indexOf("\"");
    if (phoneNumberStartIndex != -1) {
      int i = phoneNumberStartIndex + 1;

      while (i < (int)response.length() && bufsize > 1) {
        char c = response[i];

        if (c == '"') {
          break;
        }

        *buffer++ = c;
        bufsize--;
        i++;
      }

      *buffer = '\0';
      return 1;
    } else {
      *buffer = '\0';
    }
  }

  return 0;
}

void GSMVoiceCall::handleUrc(const String& urc)
{
  if (urc.startsWith("+UCALLSTAT: ")) {
    int status = urc.charAt(urc.length() - 1) - '0';

    if (status == 0 || status == 1 || status == 7) {
      _callStatus = TALKING;
    } else if (status == 2 || status == 3) {
      _callStatus = CALLING;
    } else if (status == 4 || status == 5) {
      _callStatus = RECEIVINGCALL;
    } else {
      _callStatus = IDLE_CALL;
    }
  }
}
