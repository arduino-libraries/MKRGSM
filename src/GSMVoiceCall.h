#ifndef _GSM_VOICE_CALL_H_INCLUDED
#define _GSM_VOICE_CALL_H_INCLUDED

#include "Modem.h"

enum GSM3_voiceCall_st { IDLE_CALL, CALLING, RECEIVINGCALL, TALKING};

class GSMVoiceCall : public ModemUcrHandler {

public:
  /** Service creation
      @param synch    If true, the service calls are synchronois
    */
  GSMVoiceCall(bool synch = true);

  /** Voice call status
      @return Status of the voice call, as described in GSM3MobileVoiceProvider.h
            { IDLE_CALL, CALLING, RECEIVINGCALL, TALKING};
  */
  GSM3_voiceCall_st getvoiceCallStatus();

  /** Get last command status
      @return Returns 0 if last command is still executing, 1 success, >1 error
  */
  int ready();

  /** Place a voice call. If asynchronous, returns while ringing. If synchronous
      returns if the call is stablished or cancelled.
      @param to         Receiver number. Country extension can be used or not.
                        Char buffer should not be released or used until command is over
      @param timeout    In millisecods. Time ringing before closing the call. 
                        Only used in synchronous mode.
                        If zero, ring undefinitely
      @return In asynchronous mode returns 0 if last command is still executing, 1 success, >1 error
              In synchronous mode returns 1 if the call is placed, 0 if not.
  */    
  int voiceCall(const char* to, unsigned long timeout = 30000);

  /** Accept an incoming voice call
      @return In asynchronous mode returns 0 if last command is still executing, 1 success, >1 error
              In synchronous mode returns 1 if the call is answered, 0 if not.
  */    
  int answerCall();

  /** Hang a stablished call or an incoming ring
      @return In asynchronous mode returns 0 if last command is still executing, 1 success, >1 error
              In synchronous mode returns 1 if the call is answered, 0 if not.
  */    
  int hangCall();

  /** Retrieve the calling number, put it in buffer
      @param buffer     pointer to the buffer memory
      @param bufsize    size of available memory area, at least should be 10 characters
      @return In asynchronous mode returns 0 if last command is still executing, 1 success, >1 error
              In synchronous mode returns 1 if the number is correcty taken 0 if not
  */    
  int retrieveCallingNumber(char* buffer, int bufsize);


  virtual void handleUcr(const String& ucr);
private:
  int _synch;
  GSM3_voiceCall_st _callStatus;
};

#endif
