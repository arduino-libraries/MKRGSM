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

#ifndef _GSM_CONTACTS_H_INCLUDED
#define _GSM_CONTACTS_H_INCLUDED

#define MAX_MKRGSM_CONTACTS 250
#define MAX_MKRGSM_CONTACTS_NAME 18
#define MAX_MKRGSM_CONTACTS_NUMBER 80

#define NATIONAL_NUMBER 129
#define NATIONAL_NUMBER_ALT 161
#define INTERNATIONAL_NUMBER 145
#define NETWORK_NUMBER 177

#include <Arduino.h>
#include "Modem.h"
struct Contact {
  String Name; 
  String Number;
  int Type; 
  int Id; 
}; 

class GSMContacts {
public:

  /** Constructor */
  GSMContacts();
  ~GSMContacts(); 

  /** Check modem response and restart it
   */
  int begin();
  int ready();
  int available();
  /** Obtain modem IMEI (command AT)
      @return modem IMEI number
   */
  char * search(const char * q);
  char * search(String &q) { return search(q.c_str()); };
  
  
  int get(Contact & c, char id);
  int get(Contact & c, int id) { return get(c, (char) id);};
  
  
  int del(char id);
  int del(int id) { return del((char) id);};
  
  int update(Contact & c);
  int add(Contact & c);
  
 private:
  bool _commandSent;
  bool _contactAvailable;
  char _contactList[MAX_MKRGSM_CONTACTS+1];
  int _contactIndex;
  
};

#endif
