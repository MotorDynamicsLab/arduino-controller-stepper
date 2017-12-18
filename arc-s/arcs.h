//###########################################################################
// arcs.h
// library for controlling the integrated stepper driver on the ARC-S board
//
// $Copyright: Copyright (C) LDO Systems 2018
//###########################################################################
#ifndef _ARCS_H_
#define _ARCS_H_

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
public:
  Arcs();
  void setMicroStep(ArcsMicroStep setting);
  void setCurrent(ArcsCurrentMode mode);
  void moveMotor();
  void stopMotor();
};

#endif /* _ARCS_H_ */
