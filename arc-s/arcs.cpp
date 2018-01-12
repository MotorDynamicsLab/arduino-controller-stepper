//###########################################################################
// arcs.c
// library for controlling the integrated stepper driver on the ARC-S board
//
// $Copyright: Copyright (C) LDO Systems 2018
//###########################################################################
#include "arcs.h"
#include <math.h>
#define IOCLOCK 16000000

Arcs::Arcs()
{
	accelerategap = 1000;
	acceleratedspeed = 10;
	isStartup = false;
}


///Initialize the pin and timer5
void Arcs::Initialize()
{
	pinEn.ch = _PJ;
	pinEn.pinnum = 2;
	configPin(pinEn);

	pinStep.ch = _PL;
	pinStep.pinnum = 3;
	configPin(pinStep);

	pinDir.ch = _PJ;
	pinDir.pinnum = 3;
	configPin(pinDir);

	pinCurrentMode.ch = _PH;
	pinCurrentMode.pinnum = 7;
	configPin(pinCurrentMode);

	pinTorque.ch = _PJ;
	pinTorque.pinnum = 2;
	configPin(pinTorque);

	pinMs1.ch = _PJ;
	pinMs1.pinnum = 5;
	configPin(pinMs1);

	pinMs2.ch = _PJ;
	pinMs2.pinnum = 8;
	configPin(pinMs2);

	pinMs3.ch = _PJ;
	pinMs3.pinnum = 7;
	configPin(pinMs3);


	//Initialize timer 5
	TCCR5A = 0;
	TCCR5B = 0;
	TCCR5A |= _BV(COM5A0) | _BV(WGM51) | _BV(WGM50);
	TCCR5B |= _BV(WGM52) | _BV(WGM53);
	TCCR5B |= _BV(CS50);
}


///Configure the number of motor steps and subdivisions
void Arcs::configMotor(uint32_t stepsPerRev, ArcsMicroStep microstep)
{
	lines = stepsPerRev;
	this->microstep = microstep;
  OCR5A = 0xfffe;
	PRR1 |= _BV(PRTIM5);
}


///Set the speed
void Arcs::setSpeed(double speedRPM)
{
  if(speedRPM <= minSpeed)
  {
    rev = minSpeed;
    return;
  }
    
	rev = speedRPM;
	if( !isStartup )
    return;
	speedTransmission(speedRPM);
}


///Set the direction of rotation
void Arcs::setDir(ArcsDirection dir)
{
	if (ARCS_FORWARD == dir)
	{
		writePin(pinDir,LOW);
	}
	else
	{
		writePin(pinDir,HIGH);
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
		writePin(pinMs1,LOW);
		writePin(pinMs2,LOW);
		writePin(pinMs3,LOW);
	}
	break;
	case ARCS_DIV2:
	{
		writePin(pinMs1,HIGH);
		writePin(pinMs2,LOW);
		writePin(pinMs3,LOW);
	}
	break;
	case ARCS_DIV4:
	{
		writePin(pinMs1,LOW);
		writePin(pinMs2,HIGH);
		writePin(pinMs3,LOW);
	}
	break;
	case ARCS_DIV8:
	{
		writePin(pinMs1,HIGH);
		writePin(pinMs2,HIGH);
		writePin(pinMs3,LOW);
	}
	break;
	case ARCS_DIV16:
	{
		writePin(pinMs1,HIGH);
		writePin(pinMs2,HIGH);
		writePin(pinMs3,HIGH);
	}
	break;
	default:
		break;
	}
	speedTransmission(rev);
}


///Set torque
void Arcs::setCurrent(ArcsCurrentMode mode)
{
	if (ARCS_MODE_1 == mode)
	{
		writePin(pinCurrentMode,LOW);
	}
	else
	{
		writePin(pinCurrentMode,HIGH);
	}
}


///Enable stepper motor drive module
void Arcs::enableMotor()
{
	writePin(pinEn,LOW);
}


///Disable stepper motor drive module
void Arcs::disableMotor()
{
	writePin(pinEn,HIGH);
}


///Restart stepper motor drive module
void Arcs::Reset()
{
	writePin(pinReset,LOW);
	delay(1);
	writePin(pinReset,HIGH);
}


///Set the acceleration
///acceleration = accspe / (delaytime / 1000000) * 60
void Arcs::setAcceleration(double accelRpmPerSec)
{
	acceleratedspeed = accelRpmPerSec / 1000000;
}


///Change to the specified speed
void Arcs::speedTransmission(double speedvalue)
{
	double temp = IOCLOCK * 30 / lines / microstep;
	double currentSpeed = temp / (OCR5A + 1);

	bool isSpeedup = false;

	if (0 != speedvalue)
	{
		if ( uint16_t( temp / speedvalue - 1 ) < OCR5A )
		{
			isSpeedup = true;
		}
		else
		{
			isSpeedup = false;
		}
	}
	else
	{
		isSpeedup = false;
	}

	if (isSpeedup)
	{
		while (currentSpeed <= speedvalue)
		{
			OCR5A = uint16_t( temp / currentSpeed - 1);
			currentSpeed += acceleratedspeed;
			delayMicroseconds(accelerategap);
		}

		if (currentSpeed > speedvalue)
		{
			OCR5A = uint16_t( temp / speedvalue - 1);
			currentSpeed = speedvalue;
			delayMicroseconds(accelerategap);
		}
		rev = speedvalue;
	}
	//slowdown
	else
	{
		while (currentSpeed > speedvalue)
		{
			OCR5A = uint16_t( temp / currentSpeed - 1);
			currentSpeed -= acceleratedspeed;
			delayMicroseconds(accelerategap);
		}

		if (currentSpeed <= speedvalue)
		{
			if (0 == speedvalue)
			{
				OCR5A = temp - 1;
				delayMicroseconds(accelerategap);
				PRR1 |= _BV(PRTIM5);
			}
			else
			{
				OCR5A = uint16_t( temp / speedvalue - 1);
				delayMicroseconds(accelerategap);
			}
		}

		if (0 < speedvalue)
		{
			rev = speedvalue;
		}
	}
}


///Configure the corresponding pin as output
void Arcs::configPin(GpioIninStruct pininfo)
{
	volatile uint8_t *reg, *out;
	reg = portModeRegister(pininfo.ch);
	out = portOutputRegister(pininfo.ch);

	uint8_t oldSREG = SREG;
	cli();
	*reg |= (1 << pininfo.pinnum);
	SREG = oldSREG;
}


///Modify the GPIO pin output level
void Arcs::writePin(GpioIninStruct pininfo, bool state)
{
	volatile uint8_t *out;
	out = portOutputRegister(pininfo.ch);

	uint8_t oldSREG = SREG;
	cli();

	if (LOW == state)
	{
		*out &= ~(1 << pininfo.pinnum);
	}
	else
	{
		*out |= (1 << pininfo.pinnum);
	}

	SREG = oldSREG;
}
