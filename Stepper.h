/*
	Stepper.h - A custom class to drive a stepper extruder motor
	Created by Mitchell Gu
	Modified by Yijiang Huang to fit in Arduino System
*/

#ifndef Stepper_h
#define Stepper_h

#include <arduino.h>
#include <TimerObject.h>
#include "Pins.h"

class Stepper
{
public:
	Stepper(
		int stepPin,
		int dirPin,
		float mm_per_step,
		TimerObject *ptimer,
		int initial_speed = 200,
		bool initial_dir = true);
	~Stepper();

	void Enable();
	void Disable();
	void SetSpeed(float speed, bool run = true);
	void SetTimer();
	void SetDir(bool dir);
	void Update();

	bool GetDir()	{ return current_dir_; }
	int	 GetSpeed() { return speed_; }

private:
	int  ConvertSpeed();

private:
	const int	step_pin_;
	const int	dir_pin_;
	const float mm_per_step_;
	int   us_per_step_;
	
	float speed_;
	bool  current_dir_;
	
	TimerObject *pstep_timer_;
};

#endif