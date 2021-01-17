/*
   Parser des commandes en provenance du port série
*/
#include "parser.h"
#include <Arduino.h>
#include <stdlib.h>
#include <EEPROMex.h>




#define __DEBUG1__ false
#if __DEBUG1__
    #define PRINT Serial.println
#else
    #define PRINT Serial.print
#endif


#define __DEBUG2__ false
#if __DEBUG2__
  #define PRINTDBG echo
#else
  #define PRINTDBG //

#endif





bool debug = false;
int TempCoefficientRaw = 1;
int TempCoefficient = 1;
int MaxSteps = 0;
bool MoonliteMode = false;
#define MAXCOMMAND  8
char    cmd[MAXCOMMAND];
char    parm[MAXCOMMAND];
char    outWire[MAXCOMMAND];
char    tempString[32];
#if defined(ARDUINO_AVR_NANO)
  static uint16_t waitvalue[5]= {1680, 1350, 1028, 700, 375}; // Focuser Patrick
#elif defined (ARDUINO_AVR_LEONARDO)
  static uint16_t waitvalue[5]= {910, 710, 510, 310, 110};
#endif
unsigned long hexstr2long(char *line);

AbsoluteDriver *driver = NULL;
unsigned long currentPos = 0;
unsigned long targetPos = 0;
unsigned long previousPos = 0;
long togo = 0; // attention valeur signé parceque compteur testé / 0
int direction = 0;
int stepMode = 0;
int speedNumber = 1;
int wait = waitvalue[0];
//////////////////////////////////
// Blocage , déblocage du focuser
// Le focuser est en position
// initiale bloquée
/////////////////////////////////
bool focuserlock =true;

// USB  disponible
bool USB_present = true;

// EEProm read/write
int addressCurrentPos;
const int memBase          = 350;



long int vparm = 0;
bool    interrupt = true;

void setnewPos(unsigned long value);

#define DEBUG true

// déclaration de fonctions
//long hexstr2long(char *line);
//void process_data (const char * input);

void echo(const char* cmd, long parm)
{
    char s[32];
    sprintf(s,"%s%ld",cmd,parm);
    Serial.println(s);
}

void stop()
{
    interrupt = true;
}


void go()
{
  interrupt = false;
}

void move()
{
  if (!interrupt)
  {
    if (togo > 0)
    {
      // Guuy
      driver->move(-direction, wait);
      
      //driver->move(direction, wait);
      currentPos += direction;
      PRINTDBG("c>", currentPos);
      PRINTDBG("d>", direction);
      togo--;
    }
    else
    {
      interrupt = true;
      previousPos = currentPos;
    }
  }
  else
  {
    if((digitalRead(INWARD) == LOW) && (digitalRead(OUTWARD) == LOW))
    {
      int t1 = millis();
      int t2;
      while((digitalRead(INWARD) == LOW) && (digitalRead(OUTWARD) == LOW))
      {
        t2 = millis() - t1;
      }
      if (t2 > 5000)
      {
        focuserlock = !focuserlock;
        if (focuserlock)
        {
          digitalWrite(DRIVER_EN, HIGH);
        }
        else
        {
          digitalWrite(DRIVER_EN, LOW);
        }
      }
      else if (t2 > 2000)  // Retour en position 0
      {
        setnewPos(0);
        go();
      }
      else if (t2 < 1000) // Changement de vitesse
      {
        speedNumber = (speedNumber < 5 ? speedNumber+1:1);
        wait = waitvalue[speedNumber-1];                
      }
      delay(500);

    } 
    else if(digitalRead(INWARD) == LOW) // Bouton déplacement infocus
    {
      if (currentPos > 0) // fonctionne seulement si currentPos est > 0
      {
        interrupt = false;
        direction = -1;
        togo = 1;
      }
    }
    else if(digitalRead(OUTWARD) == LOW) // Bouton déplacement outfocus
    {
      interrupt = false;
      direction = +1;
      togo = 1;
    }
  }  
}


void storePos() // save pos if it has changed or each minute
{
//   if (currentPos != previousPos)
//   {
//     EEPROM.writeLong(addressCurrentPos,currentPos);
//   }
 }

void setnewPos(unsigned long value)
{
  targetPos = value;
  togo = targetPos - currentPos;
  if (targetPos > currentPos) 
    direction = +1; //FORWARD
  else
    direction = -1; //INWARD
  togo = abs(togo);
  PRINTDBG("Dans setNewPos togo = ", togo);
  PRINTDBG("targetPos = ", targetPos);
  PRINTDBG("currentPos = ", currentPos);
  PRINTDBG("direction = ", direction);
  wait = waitvalue[speedNumber-1];
}


void initialize(int initPos)
{
      
    Serial.begin(SERIAL_SPEED);
    pinMode(DRIVER_DIR, OUTPUT);
    digitalWrite(DRIVER_DIR, HIGH);
    driver = new AbsoluteDriver(STEPS_REV, DRIVER_DIR, DRIVER_STEP, DRIVER_EN, M0, M1, M2);
    driver->setSpeedProfile(BasicStepperDriver::LINEAR_SPEED, MOTOR_ACCEL, MOTOR_ACCEL);
    currentPos      = initPos;
    targetPos       = initPos;
    previousPos     = initPos;
    direction       = +1;
    togo            = 0;
    stepMode        = 0;
    // Set microstep mode
    pinMode(INWARD, INPUT_PULLUP);
    pinMode(OUTWARD, INPUT_PULLUP);

  
    #if defined(ARDUINO_AVR_NANO)
      pinMode(M0, OUTPUT);
      pinMode(M1, OUTPUT);
      pinMode(M2, OUTPUT);
      digitalWrite(M0, LOW);
      digitalWrite(M1, LOW);
      digitalWrite(M2, HIGH);
    #else
      pinMode(DRIVER_EN, OUTPUT);
      digitalWrite(DRIVER_EN, LOW);
    #endif
    // EEPROM.setMemPool(memBase, EEPROMSizeUno);
    // addressCurrentPos      = EEPROM.getAddress(sizeof(long));
    // currentPos = EEPROM.readLong(addressCurrentPos);
}

unsigned long hexstr2long(char *line) {
  unsigned long ret = 0;
  ret = (unsigned long) strtol(line, NULL, 16);
  PRINTDBG("ret = ", ret);
  return (ret);
}

void process_data (const char * input)
{
    memset(tempString,'\0',sizeof(tempString));

    //parse de la commande

    int len = strlen(input);
    if (len == 1)
    {
        cmd[0] = input[0];
        cmd[1] = '\0';
    }
    else
    {
        if (len > 2)
        {
            memset(parm,0,MAXCOMMAND);
            strncpy(parm, input + 2, len - 2);

            vparm =  hexstr2long(parm);
            PRINTDBG("parm ", vparm);
        }
        cmd[0] = input[0];
        cmd[1] = input[1];
        cmd[2] = '\0';
    }
 
  // for now just display it
  // (but you could compare it to some value, convert to an integer, etc.)
  //Serial.print (cmd);  // end of process_data
  // toggle debug on/off

    if (!strcasecmp(cmd, "D"))
    {
        debug = !debug;
        if(debug)
        {
            Serial.println("Debug enabled");
        }
    }

    if (!strcasecmp(cmd, "GP"))
    {
        sprintf(tempString, "%lX#", currentPos);
        PRINT(tempString);
     }

    if (!strcasecmp(cmd, "GC")) {
        sprintf(tempString, "%02X#", TempCoefficientRaw);
        PRINT(tempString);
    }

    if (!strcasecmp(cmd, "GV")) {
        PRINT("10#");
    }

    if (!strcasecmp(cmd, "GH"))
    {
        sprintf(tempString,"%02X", stepMode);
        PRINT(tempString);
        PRINT('#');
    }

        // get temperature
    if (!strcasecmp(cmd, "GT")) {
    /*
      // Skip temperature reading when motor is running
      if (stepper.isMoving()) {
        if (TempSensor_Present)
          TempSensor_Reading = DHT_getTemperature();
      }

      // reset temp sensor read timer.
      millisLastTempSensorRead = millis();

      //char tempString[6];
      if (MoonliteMode)
        // compatability mode, 0.5 degeee resolution
        sprintf(tempString, "%04X", (int)(TempSensor_Reading/0.5));
      // else 0.125 degree resolution
      else sprintf(tempString, "%04X", (int)(TempSensor_Reading/0.125));
      PRINT(tempString);
      PRINT("#");
    */
        sprintf(tempString, "%02X", 20);
        PRINT(tempString);
        PRINT("#");
    }
    
    // renvoyer nouvelle position sur requête
    if (!strcasecmp(cmd, "GN")) {
        sprintf(tempString, "%lX#", targetPos);
        PRINT(tempString);
    }

    // Le focuser se déplace - 01 se déplace, 00 sinon
    if (!strcasecmp(cmd, "GI")) {
      if (!interrupt) {
        PRINT("01#");
      }
      else {
        PRINT("00#");
      }
    }

    // retourne la vitesse du moteur, seules valeurs possible 02, 04, 08, 10, 20, définies par la commande SD
    if (!strcasecmp(cmd, "GD")) {
        // speedNumber ets compris entre 1 et 5
        sprintf(tempString, "%02X#", 2<<(speedNumber-1));
        PRINT(tempString);
    }

    // Version du firmware "10"
    if (!strcasecmp(cmd, "GV")) {
      PRINT("10#");
    }

        //Coefficient de temperature
    if (!strcasecmp(cmd, "SC")) {
      TempCoefficientRaw = vparm;
      // covert signed 8-bit to signed int
      if ((TempCoefficientRaw & 0x80)) {// negtive
        TempCoefficient = TempCoefficientRaw - 256;
      }
      else {
        TempCoefficient = TempCoefficientRaw;
      }
    }

    // full step mode (non fonctionnel)
    if (!strcasecmp(cmd, "SF")) {
        stepMode = 0;
    }

    // half step mode 
    // full step mode
    if (!strcasecmp(cmd, "SH")) {
        stepMode = 1;
    }

    // mode de compatibilité: pas totalement conforme au protocole Moonlite
    if (!strcasecmp(cmd, "YM")) {
      MoonliteMode = false;
    }

    // Arrête le mouvement du focuser
    if (!strcasecmp(cmd, "FQ")) {
        stop();
    }

    // Lance le mouvement du focuser
    if (!strcasecmp(cmd, "FG")) {
        go();
     }


    // Commandes envoyées par la raquette (Non fonctionnel)
    if (!strcasecmp(cmd, "FI")) // move inward
    {
      PRINT(cmd);
    }
    if (!strcasecmp(cmd, "FO")) // move forward
    {
      PRINT(cmd);
    }

    // fixer la vitess, seules valeurs acceptables 02, 04, 08, 10, 20
    if (!strcasecmp(cmd, "SD"))
    {
        // speedNumber est compris entre 1 et 5
        // waitvalue contient les latences en µs entre deux pas
        speedNumber = log(vparm)/log(2);
        wait = waitvalue[speedNumber-1];
    }

    // Force la position (utile en mode debug, pas utilisé par Ekos ?)
    if (!strcasecmp(cmd, "SP")) {
        currentPos = vparm;
        // EEPROM.writeLong(addressCurrentPos,currentPos);
    }

    // fixer la nouvelle position avant déplacement
    if (!strcasecmp(cmd, "SN"))
    {
      setnewPos((unsigned long) vparm);
    }

    // enable TempComp (Non fonctionnel)
    if (!strcasecmp (cmd, "Y+"))
    {
    /*  TempCompEn = true;

      // Latch current position and average temperature.
      TempCompOriginalTemperature = TempSensor_Average;
      TempCompOriginalPosition = stepper.currentPosition();

      TempCompLastTemperature = TempSensor_Average;
      TempCompTargetPosition = TempCompOriginalPosition;
    */}

        // disable TempComp,  (Non fonctionnel)
    if (!strcasecmp (cmd, "Y-")) {
      //TempCompEn = false;
    }

    //(Non fonctionnel)
    if (!strcasecmp(cmd, "Z+")) {
    /*  if (TempCompEn) {
        PRINT("01#");
      }
      else {
        PRINT("00#");
      }
    */
    }

    // home the motor, hard-coded, ignore parameters since we only have one motor
    if (!strcasecmp(cmd, "PH")) {
      targetPos = 8000;
      interrupt = false;
    }

    // set backlash ( (Non fonctionnel))
    if (!strcasecmp(cmd, "YB")) {
     // backlash = vparm;
    }

    // get backlash set by YB  (Non fonctionnel)
    if (!strcasecmp(cmd, "ZB")) {
      //char tempString[6];
      // sprintf(tempString, "%02X", backlash);
      // PRINT(tempString);
      // PRINT("#");
    }

    // set TempComp threshold in unit of 0.25 degree  (Non fonctionnel)
    if (!strcasecmp(cmd, "YT")) {
    /*
      TempCompThresholdRaw = hexstr2long(param);
      TempCompThreshold = (float)TempCompThresholdRaw / 4; // covert to degree
    */
    }

    // get TempComp threshold set by YT  (Non fonctionnel)
    if (!strcasecmp(cmd, "ZT")) {
    /*
      //char tempString[6];
      sprintf(tempString, "%02X", TempCompThresholdRaw);
      PRINT(tempString);
      PRINT("#");
    */
    }
    //  (Non fonctionnel)
    if (!strcasecmp(cmd, "YX")) {
      MaxSteps = vparm;
    }
    //  (Non fonctionnel)
    if (!strcasecmp(cmd, "ZX")) {
      //char tempString[6];
      sprintf(tempString, "%04X", MaxSteps);
      PRINT(tempString);
      PRINT("#");
    }

    //  (Non fonctionnel)
    if (!strcasecmp(cmd, "ZA")) {
    /*
      int TempInt;
      TempInt = (int)(TempSensor_Average * 100);
      if (TempInt >= 0) {
        TempInt = TempInt & 0xFFFF;
      }
      else { // convert to 2's complement
        TempInt = ~abs(TempInt) & 0xFFFF;
      }

      //char tempString[6];
      sprintf(tempString, "%04X", TempInt);
      PRINT(tempString);
      PRINT("#");
    */
    }

    // Remise à zéro de la position du focuser (utilisé en mode debug)
    if (!strcasecmp(cmd, "RS"))
    {
      // Reset currentPos in eeprom
      currentPos = 0;
      // EEPROM.writeLong(addressCurrentPos,currentPos);
      // Serial.print("currentPos en EEPROM est réinitialise a : ");
      // Serial.println(currentPos)  ;  
    }

    // Debug Info
//    if (!strcasecmp(cmd, "SS"))
//      if(debug) outputDebugInfo();
}



void processIncomingByte (const byte inByte)
{
  static char input_line [MAXCOMMAND];
  static unsigned int input_pos = 0;


  switch (inByte)
  {
  case ':':
      input_pos = 0;
      break;
  case '#':   // end of text
      input_line [input_pos] = 0;  // terminateur 0
      // terminateur atteint process des données ci-après ...
      process_data (input_line);
      // remise à 0 pour l'acquisition suivante
      input_pos = 0;  
      break;
  case '\r':   // sauter le retour chariot
      break;
  case '\n':   // sauter le saut de ligne
      break;
  default:
      // on empile les caractères reçus dans te lableau de caractères
      if (input_pos < (MAXCOMMAND - 1))
        input_line [input_pos++] = inByte;
      break;
  }  // end of switch
   
} // end of processIncomingByte  

