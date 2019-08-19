/*
  Radio Access Technology selection for Arduino MKR GSM 1400

  This sketch allows you to select your preferred Radio Access
  Technology (RAT).

  You can choose among 2G, 3G/UTMS or a combination of both.

  The chosen configuration will be saved to modem's internal memory
  and will be preserved through MKR GSM 1400 sketch uploads.

  In order to change the RAT, you will need to run this sketch again.

  Circuit:
  - MKR GSM 1400 board
  - Antenna
  - SIM card

  Created 19 August 2019
  by Riccardo Rizzo

*/

#include <MKRGSM.h>


void setup() {
  Serial.begin(9600);
  while (!Serial);

  MODEM.begin();
  while (!MODEM.noop());

  for (int i = 0; i < 80; i++) Serial.print("*");
  Serial.println();
  Serial.println("This sketch allows you to select your preferred");
  Serial.println("GSM Radio Access Technology (RAT).");
  Serial.println();
  Serial.println("You can choose among 2G, 3G/UMTS or a combination of both.");
  Serial.println();
  Serial.println("The chosen configuration will be saved to modem's internal memory");
  Serial.println("and will be preserved through MKR GSM 1400 sketch uploads.");
  Serial.println();
  Serial.println("In order to change the RAT, you will need to run this sketch again.");
  for (int i = 0; i < 80; i++) Serial.print("*");

  Serial.println();
  Serial.println();
  Serial.println("Please choose your Radio Access Technology:");
  Serial.println();
  Serial.println("    0 - 2G only");
  Serial.println("    1 - 3G/UMTS only");
  Serial.println("    2 - 2G preferred, 3G/UMTS as failover");
  Serial.println("    3 - 3G/UMTS preferred, 2G as failover (default)");
  Serial.println();
}

void loop() {
  String uratChoice;

  Serial.print("> ");

  Serial.setTimeout(-1);
  while (Serial.available() == 0);
  String uratInput = Serial.readStringUntil('\n');
  uratInput.trim();
  int urat = uratInput.toInt();
  Serial.println(urat);

  switch (urat) {
    case 0:
      uratChoice = "0";
      break;
    case 1:
      uratChoice = "2";
      break;
    case 2:
      uratChoice = "1,0";
      break;
    case 3:
      uratChoice = "1,2";
      break;
    default:
      Serial.println("Invalid input. Please, retry.");
      return;
  }

  setRAT(uratChoice);
  apply();

  Serial.println();
  Serial.println("Radio Access Technology selected.");
  Serial.println("Now you can upload your 2G or 3G application sketch.");
  while (true);
}

bool setRAT(String choice)
{
  String response;

  Serial.print("Disconnecting from network: ");
  MODEM.sendf("AT+COPS=2");
  MODEM.waitForResponse(10000);
  Serial.println("done.");

  Serial.print("Setting Radio Access Technology: ");
  MODEM.sendf("AT+URAT=%s", choice.c_str());
  MODEM.waitForResponse(10000, &response);
  Serial.println("done.");

  return true;
}

bool apply()
{
  Serial.print("Applying changes and saving configuration: ");
  MODEM.reset();
  delay(5000);
  Serial.println("Modem restart");
  MODEM.begin(true);

  do {
    delay(1000);
    MODEM.noop();
  } while (MODEM.waitForResponse(1000) != 1);

  Serial.println("done.");
  
  return true;
}
