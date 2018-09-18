#ifndef _GSM_CONTACTS_H_INCLUDED
#define _GSM_CONTACTS_H_INCLUDED

#define MAX_MKRGSM_CONTACTS 250
#define MAX_MKRGSM_CONTACTS_NAME 18
#define MAX_MKRGSM_CONTACTS_NUMBER 80

#define NATIONAL_NUMBER 129
#define NATIONAL_NUMBER_2 161
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
  
  int put(Contact & c);
  int update(Contact & c);
 
  void dump();
  
 private:
  bool _commandSent;
  bool _contactAvailable;
  char _contactList[MAX_MKRGSM_CONTACTS+1];
  int _contactIndex;
  
};

#endif
