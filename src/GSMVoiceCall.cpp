#include "GSMVoiceCall.h"

GSMVoiceCall::GSMVoiceCall(bool synch)
{
}

GSM3_voiceCall_st GSMVoiceCall::getvoiceCallStatus()
{
  return IDLE_CALL;
}

int GSMVoiceCall::ready()
{
  return 0;
}
    
int GSMVoiceCall::voiceCall(const char* to, unsigned long timeout)
{
  return 2;
}

   
int GSMVoiceCall::answerCall()
{
  return 0;
}
    
int GSMVoiceCall::hangCall()
{
  return 0;
}

int GSMVoiceCall::retrieveCallingNumber(char* buffer, int bufsize)
{
  return 0;
}
