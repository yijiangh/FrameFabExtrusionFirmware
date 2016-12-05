/*
Pins.h - Pin Mapping for the SLAM Extruder Board.
Created by Mitchell Gu
*/

#ifndef Pins_h
#define Pins_h

// DISPLAY
#define DISP Serial1

// INDICATOR LEDS
#define EXT_LED 18
#define RET_LED 19
#define HOT_LED 20
#define FAN_LED 21

// KNOB + ENCODER
#define ENC_PIN_A 11
#define ENC_PIN_B 12
#define KNOB_RLED 6
#define KNOB_GLED 5
#define KNOB_BLED 3
#define KNOB_BTN_PIN 4

// HEATER + Fans
#define HEATER_PIN 22
#define THERM_PIN A14
#define EXT_FAN_PIN 9
#define AUX_FAN_PIN 10

//STEPPER
#define STEP_PIN 16
#define DIR_PIN 17
#define MSTEP_PIN 13
#define RESET_PIN 14
#define SLEEP_PIN 15

// SERVO
#define SERVO_PIN 23

// KUKA INPUT
#define KUKA_PIN_1 8
#define KUKA_PIN_2 7

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