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

#include "GSMLocation.h"

#define GSM_LOCATION_UPDATE_INTERVAL 1100

GSMLocation::GSMLocation() :
  _commandSent(false),
  _locationAvailable(false),
  _latitude(0),
  _longitude(0),
  _altitude(0),
  _uncertainty(0)
{
  MODEM.addUrcHandler(this);
}

GSMLocation::~GSMLocation()
{
  MODEM.removeUrcHandler(this);
}

int GSMLocation::begin()
{
  MODEM.send("AT+ULOCCELL=1");

  if (MODEM.waitForResponse() != 1) {
    return 0;
  }

  return 1;
}

int GSMLocation::available()
{
  MODEM.poll();

  if (!_commandSent) {
    _commandSent = true;
    _locationAvailable = false;

    MODEM.send("AT+ULOC=2,2,0,1,1");
    MODEM.waitForResponse();
  }

  if (_locationAvailable) {
    _commandSent = false;
    _locationAvailable = false;

    return 1;
  }

  return 0;
}

float GSMLocation::latitude()
{
  return _latitude;
}

float GSMLocation::longitude()
{
  return _longitude;
}

long GSMLocation::altitude()
{
  return _altitude;
}

long GSMLocation::accuracy()
{
  return _uncertainty;
}

void GSMLocation::handleUrc(const String& urc)
{
  if (urc.startsWith("+UULOC: ")) {
    String temp = urc;
    int lastCommaIndex;

    _locationAvailable = true;

    lastCommaIndex = temp.lastIndexOf(',');
    _uncertainty = temp.substring(lastCommaIndex + 1).toInt();
    temp.remove(lastCommaIndex);

    lastCommaIndex = temp.lastIndexOf(',');
    _altitude = temp.substring(lastCommaIndex + 1).toInt();
    temp.remove(lastCommaIndex);

    lastCommaIndex = temp.lastIndexOf(',');
    _longitude = temp.substring(lastCommaIndex + 1).toFloat();
    temp.remove(lastCommaIndex);

    lastCommaIndex = temp.lastIndexOf(',');
    _latitude = temp.substring(lastCommaIndex + 1).toFloat();
    temp.remove(lastCommaIndex);
  }
}
