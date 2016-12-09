/*
	Stepper.h - A custom class to drive a stepper extruder motor
	Created by Mitchell Gu
	Modified by Yijiang Huang to fit in Arduino System
*/

#ifndef Stepper_h
#define Stepper_h

//#include <array>
#include <arduino.h>
#include <TimerObject.h>
#include "Pins.h"

class Stepper
{
public:
	Stepper(
		int stepPin,
		int dirPin,
		int mStepPin,
		float mm_per_step,
		int initial_speed =1,
		bool initial_dir = true);
	~Stepper();

	void enable();
	void disable();
	void setSpeed(float speed, bool run = true);
	void setDir(bool dir);
	void setTimer();

private:
	const int _stepPin;
	const int _dirPin;
	const int _mStepPin;
	const float _MM_PER_STEP;
	int usPerStep;
	
	TimerObject *stepTimer;
};

#endif