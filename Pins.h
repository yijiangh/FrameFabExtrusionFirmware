/*
Pins.h - Pin Mapping for the SLAM Extruder Board.
Created by Mitchell Gu
*/

#ifndef Pins_h
#define Pins_h

// INDICATOR LEDS 
// - extusion & retraction
#define EXT_LED 18
#define RET_LED 19

// BUTTONS
#define BUTTON_ENABLE 4
#define BUTTON_INV	  4

//STEPPER
#define STEP_PIN 16
#define DIR_PIN 17
#define MSTEP_PIN 13

// KUKA INPUT
#define KUKA_ENABLE 8
#define KUKA_INV    7

//-------------------------------------
#define MotorDir_A      6
#define MotorStp_A      5
#define MotorEna_A      4

#define MotorStart_A    11
#define MotorInvert_A   10
#define UrInv           8
#define UrStart         9

#define S_LED1          3
#define S_LED2          2

#endif