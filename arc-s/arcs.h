//###########################################################################
// arcs.h
// library for controlling the integrated stepper driver on the ARC-S board
//
// $Copyright: Copyright (C) LDO Systems 2018
//###########################################################################
#ifndef _ARCS_H_
#define _ARCS_H_
#include "Gpio.h"

class Arcs
{
private:
	Gpio pinEn;
	Gpio pinReset;
	Gpio pinStep;
	Gpio pinDir;
	Gpio pinCurrentMode;
	Gpio pinTorque;
	Gpio pinMs1;
	Gpio pinMs2;
	Gpio pinMs3;
	volatile uint32_t lines;
	volatile double speedmin;

public:
  enum ArcsMicroStep
  {
    ARCS_DIV1 = 1,
    ARCS_DIV2 = 2,
    ARCS_DIV4 = 4,
    ARCS_DIV8 = 8,
    ARCS_DIV16 = 16,
  };

  enum ArcsCurrentMode
  {
    ARCS_MODE_1,
    ARCS_MODE_2,
  };

  enum ArcsDirection
  {
   ARCS_FORWARD,
   ARCS_REVERSE,
  };
	struct ConfigPinStruct
	{
		Gpio::PinChannel pinEnCh;
		uint8_t pinEnNum;

		Gpio::PinChannel pinResetCh;
		uint8_t pinResetNum;

		Gpio::PinChannel pinCurrentModeCh;
		uint8_t pinCurrentModeNum;

		Gpio::PinChannel pinDirCh;
		uint8_t pinDirNum;

		Gpio::PinChannel pinMs1Ch;
		uint8_t pinMs1Num;

		Gpio::PinChannel pinMs2Ch;
		uint8_t pinMs2Num;

		Gpio::PinChannel pinMs3Ch;
		uint8_t pinMs3Num;
	};

private:
  ArcsMicroStep microstep;
	void updateStep();

public:
	Arcs();
	void Initialize(ConfigPinStruct pinInfo);
	void configSpeed(uint32_t lines,double speedmin,ArcsMicroStep microstep = 1);
	void speedUp(double value);
	void slowDown(double value);
	void setDir(ArcsDirection dir);
	void setMicroStep(ArcsMicroStep microstep);
	void setCurrent(ArcsCurrentMode mode);
	void enableMotor();
	void disableMotor();
	void Reset();

	///Start pulsing to run the motor
	inline void moveMotor() { PRR1 &= ~_BV(PRTIM5); updateStep(); };

	///Stop the pulse to stop the motor
	inline void stopMotor() { PRR1 |= _BV(PRTIM5); updateStep(); };
};

#endif /* _ARCS_H_ */
