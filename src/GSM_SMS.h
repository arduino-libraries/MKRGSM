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

#ifndef _GSM_SMS_H_INCLUDED
#define _GSM_SMS_H_INCLUDED

#include <Stream.h>

class GSM_SMS : public Stream {

public:
  /** Constructor
      @param synch    Determines sync mode
   */
  GSM_SMS(bool synch = true);

  /** Write a character in SMS message
      @param c      Character
      @return size
    */
  size_t write(uint8_t c);

  /** Begin a SMS to send it
      @param to     Destination
      @return error command if it exists
    */
  int beginSMS(const char* to);

  /** Get last command status
      @return returns 0 if last command is still executing, 1 success, >1 error
   */
  int ready();

  /** End SMS
      @return error command if it exists
   */
  int endSMS();

  /** Check if SMS available and prepare it to be read
      @return number of bytes in a received SMS
   */
  int available();

  /** Read sender number phone
      @param number   Buffer for save number phone
      @param nlength    Buffer length
      @return 1 success, >1 error
   */
  int remoteNumber(char* number, int nlength); 

  /** Read one char for SMS buffer (advance circular buffer)
      @return byte
   */
  int read();

  /** Read a byte but do not advance the buffer header (circular buffer)
      @return byte
   */
  int peek();

  /** Delete the SMS from Modem memory and proccess answer
   */
  void flush();

private:
  bool _synch;
  int _state;
  String _incomingBuffer;
  int _smsDataIndex;
  int _smsDataEndIndex;
  bool _smsTxActive;
};

#endif
