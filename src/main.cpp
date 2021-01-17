#include <Arduino.h>
#include "parser.h"

extern bool USB_present;

long time1;
long time2;
void storePos();


void setup()
{
  // put your setup code here, to run once:

  initialize(0);
  time1 = millis();
//  inithandpad();

}
void loop() {
    // Si une donnée est dispo sur le port série
    while (Serial.available () > 0)
    {
      char c = '\0';
      c = Serial.read ();
      processIncomingByte (c);
    }
  move();
}

