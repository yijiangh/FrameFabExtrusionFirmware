/*
Pins.h - Pin Mapping for the SLAM Extruder Board.
Created by Mitchell Gu
*/

#ifndef Pins_h
#define Pins_h

// INDICATOR LEDS 
// - extusion & retraction
#define EXT_LED 3
#define RET_LED 2

// BUTTONS
#define BUTTON_ENABLE 11
#define BUTTON_INV	  10

// POTENTIOMETER
#define POTENTIOMETER_PIN 0 // A0

//STEPPER
#define RESET_PIN   0
#define SLEEP_PIN   1
#define MSTEP_PIN	4
#define STEP_PIN	5
#define DIR_PIN		6

// KUKA INPUT
#define KUKA_ENABLE 9
#define KUKA_INV    8
#define KUKA_ANALOG 1 // A1

#endif