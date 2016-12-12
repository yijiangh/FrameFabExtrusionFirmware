/*
Potentiometer.h - A class for potentiometer.
Created by Yijiang Huang
*/

#ifndef _POTENTIOMETER_h
#define _POTENTIOMETER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Filters.h>

class AnalogReader
{
public:
	AnalogReader(
		int input_pin,
		float filter_frequency = 5)
	{
		input_pin_ = input_pin;
		rc_filter_ = new FilterOnePole(LOWPASS, filter_frequency);
	}

	~AnalogReader() { delete rc_filter_; };

	// return value 0 ~ 1023
	int GetAnalog()
	{
		return rc_filter_->input((float)analogRead(input_pin_));
	}

private:
	int input_pin_;

	// Filter for Potentiometer - One Pole (RC) lowpass filter
	FilterOnePole *rc_filter_;
};


#endif

