//###########################################################################
// Gpio.h
// Hardware Layer class that manages a single GPIO pin
//
// $Copyright: Copyright (C) LDO Systems 2018
//###########################################################################
#include "Arduino.h"

// Class Definition
//

class Gpio
{	
public:
	enum PinChannel
		{
			_PA = 1,
			_PB = 2,
			_PC = 3,
			_PD = 4,
			_PE = 5,
			_PF = 6,
			_PG = 7,
			_PH = 8,
			_PJ = 10,
			_PK = 11,
			_PL = 12
		};

	enum PinMode
		{
			_INPUT = INPUT,
			_OUTPUT = OUTPUT,
			_INPUT_PULLUP = INPUT_PULLUP
		};

	enum PwmClock
		{
			_TIMER0A = 1,
			_TIMER0B = 2,
			_TIMER1A = 3,
			_TIMER1B = 4,
			_TIMER1C = 5,
			_TIMER2  = 6,
			_TIMER2A = 7,
			_TIMER2B = 8,
			_TIMER3A = 9,
			_TIMER3B = 10,
			_TIMER3C = 11,
			_TIMER4A = 12,
			_TIMER4B = 13,
			_TIMER4C = 14,
			_TIMER4D = 15,
			_TIMER5A = 16,
			_TIMER5B = 17,
			_TIMER5C = 18,
			_END     = 19
		};

	struct PwmClockStruct
		{
			PinChannel ch;
			uint8_t pinnum;
			PwmClock pwmclock;
		};

private:
	PinChannel ch;
	PinMode mod;
	uint8_t pinnum;

	void disconnectPwm();
	void turnOffPWM(uint8_t timer);

public:
	Gpio();
	void Initialize(PinChannel ch, PinMode mod,uint8_t pinnum);
	void SetPinMode(PinMode mod);
	void Write(uint8_t state);
	int Read();	
};
