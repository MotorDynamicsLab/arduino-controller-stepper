//###########################################################################
// arcs.h
// library for controlling the integrated stepper driver on the ARC-S board
//
// $Copyright: Copyright (C) LDO Systems 2018
//###########################################################################
#ifndef _ARCS_H_
#define _ARCS_H_
#include "Arduino.h"

class Arcs
{
private:
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

	struct GpioIninStruct
	{
		PinChannel ch;
		uint8_t pinnum;
	};

	GpioIninStruct pinEn;
	GpioIninStruct pinStep;
	GpioIninStruct pinDir;
	GpioIninStruct pinReset;
	GpioIninStruct pinCurrentMode;
	GpioIninStruct pinTorque;
	GpioIninStruct pinMs1;
	GpioIninStruct pinMs2;
	GpioIninStruct pinMs3;
	volatile uint32_t lines;
	volatile double rev;
	uint16_t acceleratedspeed;
	uint32_t accelerategap;
	const uint8_t minSpeed = 3;
	bool isStartup;

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

private:
	ArcsMicroStep microstep;
	void speedTransmission(double speedvalue);
	void configPin(GpioIninStruct pininfo);
	void writePin(GpioIninStruct pininfo,bool state);

public:
	Arcs();
	void Initialize();
	void configMotor(uint32_t stepsPerRev, ArcsMicroStep microstep = 16);
	void setSpeed(double speedRPM);
	void setDir(ArcsDirection dir);
	void setMicroStep(ArcsMicroStep microstep);
	void setCurrent(ArcsCurrentMode mode);
	void enableMotor();
	void disableMotor();
	void Reset();
	void setAcceleration(uint8_t acceValue, uint32_t delaytimeus = 1000);

	///Start pulsing to run the motor
	inline void moveMotor() { PRR1 &= ~_BV(PRTIM5); isStartup = true; OCR5A = 0xfffe; speedTransmission(rev); };

	///Stop the pulse to stop the motor
	inline void stopMotor() { speedTransmission(0); isStartup = false; };
};

#endif /* _ARCS_H_ */
