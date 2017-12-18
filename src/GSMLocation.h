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

#ifndef _GSM_LOCATION_H_INCLUDED
#define _GSM_LOCATION_H_INCLUDED

#include <Arduino.h>

#include "Modem.h"

class GSMLocation : public ModemUrcHandler {

public:
  GSMLocation();
  virtual ~GSMLocation();

  int begin();

  int available();

  float latitude();
  float longitude();
  long altitude();
  long accuracy();

  void handleUrc(const String& urc);

private:
  bool _commandSent;
  bool _locationAvailable;

  float _latitude;
  float _longitude;
  long _altitude;
  long _uncertainty;
};

#endif
