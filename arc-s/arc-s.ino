#include "arcs.h"
#define DIR 28
#define SPEED_UP 22
#define SLOW_DOWN 24
#define STOP_START 26
#define DELLAY 100

Arcs arcs;
bool dirState;
bool isMotorRun = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(DIR,INPUT);
  digitalWrite(DIR,HIGH);
  
  pinMode(SPEED_UP,INPUT);
  digitalWrite(SPEED_UP,HIGH);
  
  pinMode(SLOW_DOWN,INPUT);
  digitalWrite(SLOW_DOWN,HIGH);
  
  pinMode(STOP_START,INPUT);
  digitalWrite(STOP_START,HIGH);

  Serial.begin(9600);

  Arcs::ConfigPinStruct pinInfo;
  pinInfo.pinEnCh = Gpio::_PJ;
  pinInfo.pinEnNum = 2;
  pinInfo.pinCurrentModeCh = Gpio::_PH;
  pinInfo.pinCurrentModeNum = 7;
  pinInfo.pinDirCh = Gpio::_PJ;
  pinInfo.pinDirNum = 3;
  pinInfo.pinMs1Ch = Gpio::_PJ;
  pinInfo.pinMs1Num = 5;
  pinInfo.pinMs2Ch = Gpio::_PJ;
  pinInfo.pinMs2Num = 6;
  pinInfo.pinMs3Ch = Gpio::_PJ;
  pinInfo.pinMs3Num = 7;
  arcs.Initialize(pinInfo);
  
  dirState = digitalRead(DIR);
  if(dirState)
    arcs.setDir(Arcs::ARCS_REVERSE);
  else
    arcs.setDir(Arcs::ARCS_FORWARD);
  arcs.configSpeed(200 ,1000,16);
}

void loop() {
  // put your main code here, to run repeatedly:
  if( dirState != digitalRead(DIR) )
  {
    dirState = digitalRead(DIR);
    arcs.stopMotor();
    if( DisappearsShakes(DIR) )
    {
        arcs.setDir(Arcs::ARCS_FORWARD);
        Serial.println("ARCS_FORWARD");
    }
    else
    {
        arcs.setDir(Arcs::ARCS_REVERSE);
        Serial.println("ARCS_REVERSE");
    }
    arcs.moveMotor(); 
    delay(10);
  }

  if(!digitalRead(SPEED_UP))
  {
    if(!isMotorRun)
      return;
    if( DisappearsShakes(SPEED_UP) )
      arcs.speedUp(10);
    delay(DELLAY);
  }

  if(!digitalRead(SLOW_DOWN))
  {
     if(!isMotorRun)
      return;
    if( DisappearsShakes(SLOW_DOWN) )
      arcs.slowDown(10);
    delay(DELLAY);
  }

  if(!digitalRead(STOP_START))
  {
    if( DisappearsShakes(STOP_START) )
    {
      isMotorRun = !isMotorRun;
      if(isMotorRun)
        arcs.moveMotor();
      else
        arcs.stopMotor();
    }

    while( !digitalRead(STOP_START) );
    delay(DELLAY);
  }
}

bool DisappearsShakes(uint8_t pin)
{
  delay(10);
  if(digitalRead(pin))
    return false;
   else
    return true;
}
