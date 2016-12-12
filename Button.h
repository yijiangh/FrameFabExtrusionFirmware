/*
	Button.h - A class for pushbutton.
	Created by Yijiang Huang
*/
#include <Arduino.h>
#include <Bounce2.h>

#ifndef Button_h
#define Button_h

class Button
{
	public:
		Button(
			int input_pin,
			int debounce_interval = 5
			);
		~Button();

		bool BtnChanged();
		bool BtnPushed();

	private:
		Bounce *btn_;
};

#endif
