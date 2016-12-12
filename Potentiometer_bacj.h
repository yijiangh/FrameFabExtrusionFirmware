/*
Potentiometer.h - A class for potentiometer.
Created by Yijiang Huang
*/

#include "Filters/Filters.h"

#ifndef Potentiometer_h
#define Potentiometer_h

class PotentioMeter
{
public:
	PotentioMeter(
		int input_pin,
		float filter_frequency = 5)
	{
		input_pin_ = input_pin;
		rc_filter_ = new FilterOnePole(LOWPASS, filter_frequency);
	}

	~PotentioMeter() { delete rc_filter_; };

	// return value 0 ~ 1023
	int GetAnalog()
	{
		rc_filter_->input(analogRead(input_pin_));
	}

private:
	int input_pin_;

	// Filter for Potentiometer - One Pole (RC) lowpass filter
	FilterOnePole *rc_filter_;
};

#endif