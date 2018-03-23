//###########################################################################
// current-setup.ino
// Use this program to adjust the current setting on the stepper driver.
//
// - Pressing the "START/STOP" button toggles the enable state of 
//   the stepper driver.
// - Pressing the "DOWN" button toggles between normal current mode
//   and low current mode.
//
// $Copyright: Copyright (C) LDO Systems
//###########################################################################
#define DOWN_PIN 24
#define START_PIN 26

#define setup_vref() (DDRH |= 1 << 7)
#define vref_normal() (PORTH |= 1 << 7)
#define vref_low() (PORTH &= (~(1 << 7)))

#define setup_ena() (DDRJ |= 1 << 2)
#define enable_stepper() (PORTJ &= (~(1 << 2)))
#define disable_stepper() (PORTJ |= 1 << 2)

const int debounceDelay = 100;

struct Button
{
  int pinNum;
  int state;
  int lastState;
  unsigned long lastDebounceTime;

  Button(int pin) :
    pinNum(pin),
    lastDebounceTime(0)
  {}
};

Button downBtn(DOWN_PIN);
Button startBtn(START_PIN);
bool isStepperEnabled = false;
bool isVrefNormalState = true;

void setup()
{
  //setup A4988 pins
  disable_stepper();
  vref_normal();
  setup_ena();
  setup_vref();

  //setup buttons
  setupButton(downBtn);
  setupButton(startBtn);
}


void loop() {
  //check if DOWN button changed
  if (checkButton(downBtn))
  {
    //switch between low and high current mode
    if (LOW == downBtn.state)
    {
      if (isVrefNormalState)
      {
        vref_low();
        isVrefNormalState = false;
      }
      else
      {
        vref_normal();
        isVrefNormalState = true;
      }
    }
  }

  //check if START/STOP button changed
  if (checkButton(startBtn))
  {
    //switch between enable and disable state
    if (LOW == startBtn.state)
    {
      if (isStepperEnabled)
      {
        disable_stepper();
        isStepperEnabled = false;
      }
      else
      {
        enable_stepper();
        isStepperEnabled = true;
      }
    }
  }
}


//Configures the button and initializes internal states
void setupButton(Button& btn)
{
  pinMode(btn.pinNum, INPUT_PULLUP);
  int init_reading = digitalRead(btn.pinNum);
  
  btn.state = init_reading;
  btn.lastState = init_reading;
}


//Checks and updates the state of the button
//returns true if state has changed
bool checkButton(Button& btn)
{
  bool stateChanged = false;
  int reading = digitalRead(btn.pinNum);

  if (reading != btn.lastState)
  {
    btn.lastDebounceTime = millis();
  }

  if ( (millis() - btn.lastDebounceTime) > debounceDelay)
  {
    if (reading != btn.state)
    {
      btn.state = reading;
      stateChanged = true;
    }
  }

  btn.lastState = reading;
  
  return stateChanged;
}

