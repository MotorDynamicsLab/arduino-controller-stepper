//###########################################################################
// arcs.h
// library for controlling the integrated stepper driver on the ARC-S board
//
// $Copyright: Copyright (C) LDO Systems 2018
//###########################################################################
#ifndef _ARCS_H_
#define _ARCS_H_
#include "Gpio.h"

enum ArcsMicroStep
{
  ARCS_DIV2,
  ARCS_DIV4,
  ARCS_DIV8,
  ARCS_DIV16,
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

class Arcs
{
private:
	Gpio pinEn;
	Gpio pinReset;
	Gpio pinStep;
	Gpio pinDir;
	Gpio pinTorque;

public:
  Arcs();
  void Initialize(uint8_t stepPin,uint8_t dirPin);
  void setSpeed(uin32_t frequency);
  void setDir(ArcsDirection dir)
  void setMicroStep(ArcsMicroStep setting);
  void setCurrent(ArcsCurrentMode mode);
  void moveMotor();
  void stopMotor();
  void Reset();
};

#endif /* _ARCS_H_ */
