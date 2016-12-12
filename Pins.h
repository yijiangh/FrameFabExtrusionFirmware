/*
Pins.h - Pin Mapping for the SLAM Extruder Board.
Created by Mitchell Gu
*/

#ifndef Pins_h
#define Pins_h

// INDICATOR LEDS 
// - extusion & retraction
#define EXT_LED 12
#define RET_LED 13

// BUTTONS
#define BUTTON_ENABLE 11
#define BUTTON_INV	  10

// POTENTIOMETER
#define POTENTIOMETER_PIN 0 // A0

//STEPPER
#define STEP_PIN  5
#define DIR_PIN   6

// KUKA INPUT
#define KUKA_ENABLE 9
#define KUKA_INV    8
#define KUKA_ANALOG 1 // A1

#endif