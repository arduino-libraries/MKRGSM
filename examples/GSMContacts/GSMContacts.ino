/*

 This uses an MKR GSM 1400 to access the FLASH memory on the sim card that contains contact information. 

Circuit:
* MKR GSM 1400 board
* SIM card

 created 18 Sept 2019
 by Joshua Klein
*/

#include <MKRGSM.h>

GSMContacts contacts; 

void setup() {
  delay(5); //give hardware time to settle 
  Serial.begin(9600);
  while(!Serial);
  Serial.println("starting modem...");
  contacts.begin();
 //Serial.println("ready");
}
void loop(){
    if(contacts.ready()) {
      char * i; 
      Contact c; 
      i = contacts.search("");
      if(i[0] == 0) {
        Serial.println("search returned zero results");
        /* loop indefinitely */
        while(true);
      }
      Serial.print("search returned ");
      Serial.print(strlen(i));
      Serial.println(" results");
      while(*i != 0) {
        Serial.println((byte)(*i),DEC);
        contacts.get(c,*i);
        Serial.print(" num:");
        Serial.println(c.Number); 
        Serial.print(" type:");
        Serial.println(c.Type);
        Serial.print(" name:");
        Serial.println(c.Name); 
        i++;
      }
      
      /* loop indefinitely */
      while(true);
    }
    delay(10000);
}