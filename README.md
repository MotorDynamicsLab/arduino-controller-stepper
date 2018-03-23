# arduino-controller-stepper
This repository contains sample code for the ARC-S (ARduino-Controller-Stepper) board. 
The code provided can be used as is or modified freely to suit your own application.
Contained are two projects:
- the "arc-s" directory contains main arc-s sample code which the board comes shipped with. It contains a arcs library that can be easily ported to other projects.
- the "current-setter" directory contains a project used to help configure the current limit of the stepper driver.

## Arduino - Stepper Driver Interface
ARC-S uses pins which are unused by the Arduino Mega to control the stepper driver (except for STEP pin). 
This allows ARC-S to be used as an ordinary Arduino Mega if needed. The following table lists the pin connections between
the Arduino microcontroller (ATmega2560) and stepper driver (A4988):  

| A4988  | ATmega2560 |
|--------|------------|
| STEP   | PL3 (D46)  |
| DIR    | PJ3        |
| ENABLE | PJ2        |
| MS1    | PJ5        |
| MS2    | PJ6        |
| MS3    | PJ7        |
| VREF   | PH7*       |

\* _PH7 is used to switch between normal current and low current mode. Two trimpots on the board are used to adjust
the current limit in these two modes._ 

### Pin Manipulation
Manipulating pins in the stepper driver interface requires some simple register and bit manipulation:

To setup a pin as ouput use:
```c
DDRx |= (1<<y);
```
where x is the port and y is the pin offset. For example, setting up ENABLE pin: 
```c
DDRJ |= (1<<2);
```
To set a pin to logic high use: 
```c
PORTx |= (1<<y);
```
For example, setting STEP to logic high: 
```c
PORTL |= (1<<3);
```
Similarly, to clear a pin to logic low use:
```c
PORTx &= (~(1<<y));
```
For example, clearing MS1:
```c
PORTJ &= (~(1<<5));
```

### ENABLE Pin
The ENABLE pin switches the stepper drive on or off. 
The polarity of the pin is active low, so the stepper drive is enabled when the pin is logic low and is disabled when the pin is logic high. 
When the enable pin is not configured, a pull-up circuit on the PCB ensures that the stepper driver stays disabled by default.

### STEP & DIR Pins
The stepper drive moves the motor by one step when the STEP pin transitions from logic low to logic high (a.k.a. rising edge). 
The size of each step is controlled by the microstep settings. Thus the user can control the distance in which the motor moves by controlling the number of square wave pulses on the STEP pin as well as the speed of the motor by varying the frequency of the squarewaves. 
Changing the logic level on the DIR pin will change the direction of movement from subsequent STEP signals.

### Microstep Setting
The combined logic value of pins MS1, MS2, MS3 determines the microstep setting of the stepper drive. 
The following table defines possible combinations of MS1, MS2, MS3 and their corresponding microstep setting:

|  MS1 | MS2  | MS3  | Microstep Resolution | Steps/Rev (200 step motor) |
|------|------|------|----------------------|----------------------------|
| LOW  | LOW  | LOW  | Full step            | 200 steps/rev              |
| HIGH | LOW  | LOW  | Half step            | 400 steps/rev              |
| LOW  | HIGH | LOW  | 1/4 step             | 800 steps/rev              |
| HIGH | HIGH | LOW  | 1/8 step             | 1600 steps/rev             |
| HIGH | HIGH | HIGH | 1/16 step            | 3200 steps/rev             |

### Current Mode Switching
The ARC-S board contains a circuit that allows the application to switch between two current modes. Each current mode is adjustable with a trimpot on the board.
Toggling the value of pin PH7 switches between the two current modes. 
- Setting PH7 to logic high leaves the stepper driver in "normal current" mode. 
- Clearing PH7 to logic low switches the stepper driver to "low current" mode.
- The stepper driver remains in "normal current" mode if PH7 is left unconfigured.

## Simple Control Panel
ARC-S integrates a simple pushbutton and switch control panel. The control panel contains 3 momentary pushbuttons (UP, DOWN, START/STOP)
and a single switch (CW/CCW). These buttons and switches are linked to normal Arduino digital I/O pins and can be read using `digitalRead()`. 

| Button | Arduino Pin |
|--------|-------------|
| UP         | 22      |
| DOWN       | 24      |
| START/STOP | 26      |
| CW/CCW     | 28      |

- For the pushbuttons (UP, DOWN, START/STOP), pressing the button connects the corresponding Arduino pin to ground (logic LOW); releasing the button leaves the pin unconnected (floating). 
- For the switch (CW/CCW), toggling to "CCW" connects the Arduino pin to ground while toggling to "CW" leaves the pin unconnected.
- In order to use the control panel properly, the pins should be configured with `pinMode(x, INPUT_PULLUP)`. This enables the internal pullup and makes the pins read logic high when they are unconnected externally.
