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
#include "Options.h"

class Stepper
{
public:
	Stepper(
		int step_pin,
		int dir_pin,
		int mstep_pin,
		int reset_pin,
		int sleep_pin,
		float mm_per_step,
		TimerObject *ptimer,
		int   initial_speed = 500,
		bool  initial_dir = true);
	~Stepper();

	void Enable(bool incre = true);
	void Disable();
	
	void SetSpeed(float speed, bool run = true, bool incre = true);
	void SetPotmRatio(float ratio);
	void SetKAnalogRatio(float ratio);

	void SetTimer(bool incre);
	void SetDir(bool dir);
	void Update();

	bool GetDir()	{ return current_dir_; }
	int	 GetUsPerStep() { return us_per_step_; }

private:
	int  ConvertSpeed();

private:
	const int	step_pin_;
	const int	dir_pin_;
	const int	mstep_pin_;
	const int	reset_pin_;
	const int	sleep_pin_;
	const float mm_per_step_;
	int			us_per_step_;
	
	float speed_;
	bool  current_dir_;
	float potm_ratio_;
	float kanalog_ratio_;

	TimerObject *pstep_timer_;
};

#endif