//###########################################################################
// arcs.c
// library for controlling the integrated stepper driver on the ARC-S board
//
// $Copyright: Copyright (C) LDO Systems 2018
//###########################################################################
#include "arcs.h"
#define IOCLOCK 16000000

Arcs::Arcs()
{
}


///Update pulse speed
void Arcs::updateStep()
{
  TCCR5B &= ~( _BV(CS52) | _BV(CS51) | _BV(CS50) );
  if(lines * speedmin * microstep <= 7000)
  {
    //1024 frequency
    TCCR5B |= _BV(CS52) | _BV(CS50);
    //When not divisible, try to minimize the error
    OCR5A =  uint16_t( (IOCLOCK / 2 / 1024) / (lines * speedmin * microstep / 60) + 0.5 ) - 1;
  }
  else
  {
    //1 frequency
    TCCR5B |=  _BV(CS50);
    OCR5A =  uint16_t( (IOCLOCK / 2 ) / (lines * speedmin * microstep / 60) + 0.5 ) - 1;
  }
  TCNT5 = 0;
}


///Initialize the pin
///Note that the Step pin can only select one of the 46 pin(PL3), the remaining pins can be more demanding choice
void Arcs::Initialize(ConfigPinStruct pinInfo)
{
	pinEn.Initialize(pinInfo.pinEnCh,Gpio::_OUTPUT,pinInfo.pinEnNum);
	pinReset.Initialize(pinInfo.pinResetCh, Gpio::_OUTPUT, pinInfo.pinResetNum);
	pinCurrentMode.Initialize(pinInfo.pinCurrentModeCh, Gpio::_OUTPUT, pinInfo.pinCurrentModeNum);
	pinDir.Initialize(pinInfo.pinDirCh, Gpio::_OUTPUT, pinInfo.pinDirNum);
	pinMs1.Initialize(pinInfo.pinMs1Ch, Gpio::_OUTPUT, pinInfo.pinMs1Num);
	pinMs2.Initialize(pinInfo.pinMs2Ch, Gpio::_OUTPUT, pinInfo.pinMs2Num);
	pinMs3.Initialize(pinInfo.pinMs3Ch, Gpio::_OUTPUT, pinInfo.pinMs3Num);
	pinStep.Initialize(Gpio::_PL, Gpio::_OUTPUT, 3);

	//Initialize timer 5
	TCCR5A = 0;
	TCCR5B = 0;
	TCCR5A |= _BV(COM5A0);
	TCCR5B |= _BV(WGM52);
}


///Set the speed of the stepper motor
void Arcs::configSpeed(uint32_t lines, double speedmin,ArcsMicroStep microstep)
{
	this->lines = lines;
	this->speedmin = speedmin;
	this->microstep = microstep;
	updateStep();
 //Set the motor to stop at the beginning
  stopMotor();
}


///accelerate
void Arcs::speedUp(double value)
{
	if ((speedmin * lines) < (IOCLOCK / 2))
	{
		speedmin += value;
		updateStep();
	}
}


///slow down
void Arcs::slowDown(double value)
{
	if (speedmin > 0)
	{
		speedmin -= value;
		updateStep();
	}	
}


///Set the direction of rotation
void Arcs::setDir(ArcsDirection dir)
{
	if (ARCS_FORWARD == dir)
	{
		pinDir.Write(LOW);
	}
	else
	{
		pinDir.Write(HIGH);
	}
}


///Set the breakdown value
void Arcs::setMicroStep(ArcsMicroStep microstep)
{
  this->microstep = microstep;
	switch (microstep)
	{
		case ARCS_DIV1:
		{
			pinMs1.Write(LOW);
			pinMs2.Write(LOW);
			pinMs3.Write(LOW);
		}
		break;
		case ARCS_DIV2:
		{
			pinMs1.Write(HIGH);
			pinMs2.Write(LOW);
			pinMs3.Write(LOW);
		}
		break;
		case ARCS_DIV4:
		{
			pinMs1.Write(LOW);
			pinMs2.Write(HIGH);
			pinMs3.Write(LOW);
		}
		break;
		case ARCS_DIV8:
		{
			pinMs1.Write(HIGH);
			pinMs2.Write(HIGH);
			pinMs3.Write(LOW);
		}
		break;
		case ARCS_DIV16:
		{
			pinMs1.Write(HIGH);
			pinMs2.Write(HIGH);
			pinMs3.Write(HIGH);
		}
		break;
		default:
		break;
	}
 updateStep();
}


///Set torque
void Arcs::setCurrent(ArcsCurrentMode mode)
{
	if (ARCS_MODE_1 == mode)
	{
		pinCurrentMode.Write(LOW);
	}
	else
	{
		pinCurrentMode.Write(HIGH);
	}
}


///Enable stepper motor drive module
void Arcs::enableMotor()
{
  pinEn.Write(LOW);
}


///Disable stepper motor drive module
void Arcs::disableMotor()
{
  pinEn.Write(HIGH);
}


///Restart stepper motor drive module
void Arcs::Reset()
{
	pinReset.Write(LOW);
	delay(1);
	pinReset.Write(HIGH);
}
