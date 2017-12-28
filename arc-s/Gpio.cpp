//###########################################################################
// Gpio.cpp
// Hardware Layer class that manages a single GPIO pin
//
// $Copyright: Copyright (C) LDO Systems 2018
//###########################################################################
#include "Gpio.h"
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

static Gpio::PwmClockStruct PwmClockTable[] = {
												{Gpio::_PE,4,Gpio::_TIMER3B },
												{Gpio::_PE,5,Gpio::_TIMER3C },
												{Gpio::_PG,5,Gpio::_TIMER0B },
												{Gpio::_PE,3,Gpio::_TIMER3A },
												{Gpio::_PH,3,Gpio::_TIMER4A },
												{Gpio::_PH,4,Gpio::_TIMER4B },
												{Gpio::_PH,5,Gpio::_TIMER4C },
												{Gpio::_PH,6,Gpio::_TIMER2B },
												{Gpio::_PB,4,Gpio::_TIMER2A },
												{Gpio::_PB,5,Gpio::_TIMER1A },
												{Gpio::_PB,6,Gpio::_TIMER1B },
												{Gpio::_PB,7,Gpio::_TIMER0A },
												{Gpio::_PL,5,Gpio::_TIMER5C },
												{Gpio::_PL,4,Gpio::_TIMER5B },
												{Gpio::_PL,3,Gpio::_TIMER5A },
												{Gpio::_PL,3,Gpio::_END}
											  };



Gpio::Gpio()
{
}


void Gpio::turnOffPWM(uint8_t timer)
{
  switch (timer)
  {
    #if defined(TCCR1A) && defined(COM1A1)
    case TIMER1A:   cbi(TCCR1A, COM1A1);    break;
    #endif
    #if defined(TCCR1A) && defined(COM1B1)
    case TIMER1B:   cbi(TCCR1A, COM1B1);    break;
    #endif
    #if defined(TCCR1A) && defined(COM1C1)
    case TIMER1C:   cbi(TCCR1A, COM1C1);    break;
    #endif
    
    #if defined(TCCR2) && defined(COM21)
    case  TIMER2:   cbi(TCCR2, COM21);      break;
    #endif
    
    #if defined(TCCR0A) && defined(COM0A1)
    case  TIMER0A:  cbi(TCCR0A, COM0A1);    break;
    #endif
    
    #if defined(TCCR0A) && defined(COM0B1)
    case  TIMER0B:  cbi(TCCR0A, COM0B1);    break;
    #endif
    #if defined(TCCR2A) && defined(COM2A1)
    case  TIMER2A:  cbi(TCCR2A, COM2A1);    break;
    #endif
    #if defined(TCCR2A) && defined(COM2B1)
    case  TIMER2B:  cbi(TCCR2A, COM2B1);    break;
    #endif
    
    #if defined(TCCR3A) && defined(COM3A1)
    case  TIMER3A:  cbi(TCCR3A, COM3A1);    break;
    #endif
    #if defined(TCCR3A) && defined(COM3B1)
    case  TIMER3B:  cbi(TCCR3A, COM3B1);    break;
    #endif
    #if defined(TCCR3A) && defined(COM3C1)
    case  TIMER3C:  cbi(TCCR3A, COM3C1);    break;
    #endif

    #if defined(TCCR4A) && defined(COM4A1)
    case  TIMER4A:  cbi(TCCR4A, COM4A1);    break;
    #endif          
    #if defined(TCCR4A) && defined(COM4B1)
    case  TIMER4B:  cbi(TCCR4A, COM4B1);    break;
    #endif
    #if defined(TCCR4A) && defined(COM4C1)
    case  TIMER4C:  cbi(TCCR4A, COM4C1);    break;
    #endif      
    #if defined(TCCR4C) && defined(COM4D1)
    case TIMER4D: cbi(TCCR4C, COM4D1);  break;
    #endif      
      
    #if defined(TCCR5A)
    case  TIMER5A:  cbi(TCCR5A, COM5A1);    break;
    case  TIMER5B:  cbi(TCCR5A, COM5B1);    break;
    case  TIMER5C:  cbi(TCCR5A, COM5C1);    break;
    #endif
  }
}


void Gpio::ClosePwmClock()
{
	for (int i = 0; PwmClockTable[i].pwmclock != _END; i++)
	{
		if ((PwmClockTable[i].ch == ch) && (PwmClockTable[i].pinnum == pinnum))
		{
			turnOffPWM(PwmClockTable[i].pwmclock);
		}
	}
}


void Gpio::Initialize(PinChannel ch, PinMode mod, uint8_t pinnum)
{
	this->ch = ch;
	this->mod = mod;
	this->pinnum = pinnum;

	SetPinMode(mod);
}


void Gpio::SetPinMode(PinMode mod)
{
	volatile uint8_t *reg, *out;
	reg = portModeRegister(ch);
	out = portOutputRegister(ch);

	uint8_t oldSREG = SREG;
	cli();
	if (_INPUT == mod)
	{
		*reg &= ~(1 << pinnum);
		*out &= ~(1 << pinnum);
	}
	else if (_INPUT_PULLUP == mod)
	{
		*reg &= ~(1 << pinnum);
		*out |= (1 << pinnum);
	}
	else
	{
		*reg |= (1 << pinnum);
	}
	SREG = oldSREG;

	// If the pin that support PWM output, we need to turn it off
	ClosePwmClock();
}


void Gpio::Write(uint8_t state)
{
	volatile uint8_t *out;
	out = portOutputRegister(ch);

	uint8_t oldSREG = SREG;
	cli();

	if (LOW == state)
	{
		*out &= ~(1 << pinnum);
	}
	else
	{
		*out |= (1 << pinnum);
	}

	SREG = oldSREG;
}


int Gpio::Read()
{
	if (*portInputRegister(ch) & (1 << pinnum))
	{
		return HIGH;
	}
	else
	{
		return LOW;
	}
}
