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
}


///Update pulse speed
void Arcs::updateStep()
{
	OCR5A = uint16_t((IOCLOCK / 2) / (lines * rev * microstep / 60) + 0.5) - 1;
	delayMicroseconds(accelerategap);
}


///Initialize the pin
///Note that the Step pin can only select one of the 46 pin(PL3), the remaining pins can be more demanding choice
void Arcs::Initialize(ConfigPinStruct pinInfo)
{
	pinEn.Initialize(pinInfo.pinEnCh, Gpio::_OUTPUT, pinInfo.pinEnNum);
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
	TCCR5A |= _BV(COM5A0) | _BV(WGM51) | _BV(WGM50);
	TCCR5B |= _BV(WGM52) | _BV(WGM53);
	TCCR5B |= _BV(CS50);
}


///Set the speed of the stepper motor
void Arcs::configSpeed(uint32_t lines, double rev, ArcsMicroStep microstep)
{
	this->lines = lines;
	this->rev = rev;
	this->microstep = microstep;
	//Set the motor to stop at the beginning
	updateStep();
	PRR1 |= _BV(PRTIM5);
}


///accelerate
void Arcs::speedUp(double value)
{
	if ((rev * lines) < (IOCLOCK / 2))
	{
		rev += value;
		updateStep();
		delayMicroseconds(accelerategap);
	}
}


///slow down
void Arcs::slowDown(double value)
{
	if (rev > value)
	{
		rev -= value;
	}
	else
	{
		rev = minSpeed;
	}
	updateStep();
	delayMicroseconds(accelerategap);
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


///Set the acceleration
///acceleration = accspe / (delaytime / 1000000)
void Arcs::setAcceleratedSpeed(uint8_t accspe, uint32_t delaytime)
{
	acceleratedspeed = accspe;
	accelerategap = delaytime;
}


///Change to the specified speed
void Arcs::speedTransmission(double speedvalue)
{
	double currentSpeed = IOCLOCK * 30 / (OCR5A + 1) / lines / microstep;
	bool isSpeedup = false;

	if (0 != speedvalue)
	{
		if (uint16_t(IOCLOCK * 30 / lines / microstep / speedvalue - 1) < OCR5A)
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
			OCR5A = uint16_t((IOCLOCK * 30) / lines / microstep / currentSpeed - 1);
			currentSpeed += acceleratedspeed;
			delayMicroseconds(accelerategap);
		}

		if (currentSpeed > speedvalue)
		{
			OCR5A = uint16_t((IOCLOCK * 30) / lines / microstep / speedvalue - 1);
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
			OCR5A = uint16_t((IOCLOCK * 30) / lines / microstep / currentSpeed - 1);
			currentSpeed -= acceleratedspeed;
			delayMicroseconds(accelerategap);
		}

		if (currentSpeed <= speedvalue)
		{
			if (0 == speedvalue)
			{
				OCR5A = (IOCLOCK * 30) / lines / microstep / 1 - 1;
				delayMicroseconds(accelerategap);
				PRR1 |= _BV(PRTIM5);
			}
			else
			{
				OCR5A = uint16_t((IOCLOCK * 30) / lines / microstep / speedvalue - 1);
				delayMicroseconds(accelerategap);
			}
		}

		if (0 < speedvalue)
		{
			rev = speedvalue;
		}
	}
}
