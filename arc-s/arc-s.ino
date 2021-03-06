#include "arcs.h"
#define DIR 28
#define SPEED_UP 22
#define SLOW_DOWN 24
#define STOP_START 26
#define DELLAY 100

Arcs arcs;
bool dirState;
bool isMotorRun = false;
int32_t speed = 1000;

void setup() {
  // put your setup code here, to run once:
  //nitialize the pin
  pinMode(DIR,INPUT);
  digitalWrite(DIR,HIGH);
  
  pinMode(SPEED_UP,INPUT);
  digitalWrite(SPEED_UP,HIGH);
  
  pinMode(SLOW_DOWN,INPUT);
  digitalWrite(SLOW_DOWN,HIGH);
  
  pinMode(STOP_START,INPUT);
  digitalWrite(STOP_START,HIGH);
  
  //Configure subdivision and steps as well as initial speed and the number of motor steps and subdivisions
  arcs.initialize(200,16);
  
  //Check the direction before starting
  dirState = digitalRead(DIR);
  if(dirState)
    arcs.setDir(Arcs::ARCS_REVERSE);
  else
    arcs.setDir(Arcs::ARCS_FORWARD);

  arcs.setSpeed(speed);
}

void loop() {
  // put your main code here, to run repeatedly:
  //change direction
  if( dirState != digitalRead(DIR) )
  {
	//Judge the running status, if it is running, stop and then change the direction
    dirState = digitalRead(DIR);
    if(isMotorRun)
        arcs.stopMotor();
    if( debounce(DIR) )
    {
        arcs.setDir(Arcs::ARCS_FORWARD);
        Serial.println("ARCS_FORWARD");
    }
    else
    {
        arcs.setDir(Arcs::ARCS_REVERSE);
        Serial.println("ARCS_REVERSE");
    }
    if(isMotorRun)
        arcs.moveMotor(); 
    delay(10);
  }

  //accelerate
  if(!digitalRead(SPEED_UP))
  {
    if( debounce(SPEED_UP) )
	{
		speed += 10;
		arcs.setSpeed(speed);
	}
    delay(DELLAY);
  }

  //slow down
  if(!digitalRead(SLOW_DOWN))
  {
    if( debounce(SLOW_DOWN) )
	{
		speed -= 10;
    if(speed <= 0)
      speed = 1;
		arcs.setSpeed(speed);
	}
    delay(DELLAY);
  }

  //Start and stop
  if(!digitalRead(STOP_START))
  {
    if( debounce(STOP_START) )
    {
      while( !digitalRead(STOP_START) );
      isMotorRun = !isMotorRun;
      if(isMotorRun)
		      arcs.moveMotor();
      else
        arcs.stopMotor();
    }
	//Waiting for the button to release, to prevent continuous switching in an operation
    delay(DELLAY);
  }
}


///Eliminate button jitter
bool debounce(uint8_t pin)
{
  delay(10);
  if(digitalRead(pin))
    return false;
   else
    return true;
}
