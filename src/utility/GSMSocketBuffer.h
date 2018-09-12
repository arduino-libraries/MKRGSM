/*
  This file is part of the MKR GSM library.
  Copyright (C) 2018  Arduino AG (http://www.arduino.cc/)

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

#ifndef _GSMSOCKET_BUFFER_H_INCLUDED
#define _GSMSOCKET_BUFFER_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

class GSMSocketBufferClass {

public:
public:
  GSMSocketBufferClass();
  virtual ~GSMSocketBufferClass();

  void close(int socket);

  int available(int socket);
  int peek(int socket);
  int read(int socket, uint8_t* data, size_t length);

private:
  struct {
    uint8_t* data;
    uint8_t* head;
    int length;
  } _buffers[7];
};

extern GSMSocketBufferClass GSMSocketBuffer;

#endif
