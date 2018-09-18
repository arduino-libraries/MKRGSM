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

//get one contact by id AT+CPBR=1 or AT+CPBR=1,99


#include "GSMContacts.h"

GSMContacts::GSMContacts() : 
  _commandSent(false),
  _contactAvailable(false)
{
  //MODEM.debug();
  _contactList[0] = '\0';
}
GSMContacts::~GSMContacts() 
{
}
int GSMContacts::begin()
{
  return MODEM.begin();
}

int GSMContacts::ready() {
 String contacts;
  
  contacts.reserve(15);
  MODEM.send("AT+CPBS=\"SM\""); //"AT+CPBS=\"SM\""
  MODEM.waitForResponse(1800000, &contacts);
  if( MODEM.ready() == 2) {
    return 0;
  }
  Serial.println("ready"); 
  return MODEM.ready();
}
int GSMContacts::del(char id) {
	
}
int GSMContacts::update(Contact & c) {
	
}
int GSMContacts::put(Contact & c) {
	
}
int GSMContacts::get(Contact & c, char id){
	String response;
	MODEM.sendf("AT+CPBR=%i",id);
	MODEM.waitForResponse(1800000, &response);
	if( MODEM.ready() == 2) {
		return 0;
	}
	int index = response.indexOf("+CPBF: ");
	response.remove(0, index + strlen("+CPBF: ") -1);
	response.trim();
	
	//get number
	int start = response.indexOf("\"")+1; 
	int stop = response.indexOf("\"",response.indexOf("\"")+1);
	c.Number = response.substring(start,stop);
	response.remove(0,stop);
	
	//get type 
	start = response.indexOf("\"");
	stop = response.indexOf("\"",start+1);
	c.Type = response.substring(start+2,stop-1).toInt();
	response.remove(0,stop);
	
	//get name
	start = response.indexOf("\"",stop+2);
	stop = response.indexOf("\"",start+2);
	c.Name = response.substring(start+2,stop);
	//c.Name.trim();
	//c.Name.remove(c.Name.indexOf("\""));
	response.remove(stop);
	c.Id = (int)id;
	return 1;
}
char * GSMContacts::search(const char * q)
{
  String contacts;
  Serial.println("getcontacts"); 
  contacts.reserve(15);
  
  MODEM.sendf("AT+CPBF=\"%s\"",q);
  MODEM.waitForResponse(1800000, &contacts);
  
  
  if( MODEM.ready() == 2) {
    return NULL;
  }
  int j = 0; 
  int index = contacts.indexOf("+CPBF: ");
  int id_index = 0;   
  while((index != -1) && (j < MAX_MKRGSM_CONTACTS)) {
	contacts.remove(0, index + strlen("+CPBF: ") -1);
	
    id_index = contacts.substring(1, contacts.indexOf(",")).toInt();
	//Serial.println(contacts.substring(0, contacts.indexOf("\n")));
	//Serial.println(id_index);
	//Serial.print("s:"); 
	_contactList[j] = (char)id_index;
	index = contacts.indexOf("+CPBF: ");
    j++;
  }
  _contactList[j] = '\0';
  //Serial.println(contacts);
  char * returnptr = _contactList;
  return returnptr;
}
