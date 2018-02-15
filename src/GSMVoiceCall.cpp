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

  MODEM.sendf("ATD%s;", to);

  if (_synch) {
    if (MODEM.waitForResponse(180000) != 1) {
      return 0;
    }

    _callStatus = CALLING;
    for (unsigned long start = millis(); (timeout == 0) || ((millis() - start) < timeout);) {
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

int GSMVoiceCall::enableI2SInput(long sampleRate)
{
  if (disableI2SInput() == 0) {
    return 0;
  }

  int sampleRateNumber;

  switch(sampleRate) {
    case 8000:
      sampleRateNumber = 0;
      break;

    case 11025:
      sampleRateNumber = 1;
      break;

    case 12000:
      sampleRateNumber = 2;
      break;

    case 16000:
      sampleRateNumber = 3;
      break;

    case 22050:
      sampleRateNumber = 4;
      break;

    case 24000:
      sampleRateNumber = 5;
      break;

    case 32000:
      sampleRateNumber = 6;
      break;

    case 44100:
      sampleRateNumber = 7;
      break;

    case 48000:
      sampleRateNumber = 8;
      break;

    default:
      return 0;
      break;
  }

  MODEM.sendf("AT+UI2S=11,1,0,%d,1", sampleRateNumber); // mode, port, WA, sample rate, slave
  if (MODEM.waitForResponse() != 1) {
    return 0;
  }

  // enable
  MODEM.send("AT+USPM=1,1,0,0,2");
  if (MODEM.waitForResponse() != 1) {
    return 0;
  }

  return 1;
}

int GSMVoiceCall::peekDTMF()
{
  MODEM.poll();

  if (_dtmfBuffer.length()) {
    return _dtmfBuffer.charAt(0);
  }

  return -1;
}

int GSMVoiceCall::readDTMF()
{
  MODEM.poll();

  if (_dtmfBuffer.length()) {
    char c = _dtmfBuffer.charAt(0);

    _dtmfBuffer.remove(0, 1);

    return c;
  }

  return -1;
}

int GSMVoiceCall::writeDTMF(char c)
{
  switch (c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '#':
    case '*':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
      break;

    default:
      return 0; // invalid
      break;
  }

  MODEM.sendf("AT+VTS=%c", c);

  if (MODEM.waitForResponse(420000) != 1) {
    return 0;
  }

  return 1;
}

int GSMVoiceCall::disableI2SInput()
{
  MODEM.send("AT+USPM=255,255,0,0,2");

  if (MODEM.waitForResponse() == 1) {
    return 1;
  }

  return 0;
}

void GSMVoiceCall::handleUrc(const String& urc)
{
  if (urc.startsWith("+UCALLSTAT: ")) {
    int status = urc.charAt(urc.length() - 1) - '0';

    if (status == 0 || status == 1 || status == 7) {
      _callStatus = TALKING;
      _dtmfBuffer = "";
    } else if (status == 2 || status == 3) {
      _callStatus = CALLING;
    } else if (status == 4 || status == 5) {
      _callStatus = RECEIVINGCALL;
    } else {
      _callStatus = IDLE_CALL;
    }
  } else if (urc.startsWith("+UUDTMFD: ")) {
    _dtmfBuffer += urc.charAt(10);
  }
}
