/*
Button.h - A class for pushbutton.
Created by Yijiang Huang
*/

#include "Button.h"

Button::Button(
	int input_pin,
	int debounce_interval)
{
	// Initialize button with debouncing
	pinMode(input_pin, INPUT_PULLUP);
	btn_ = new Bounce();
	btn_->attach(input_pin);			// Attach button
	btn_->interval(debounce_interval);  // Debounce interval
}


Button::~Button()
{
	delete btn_;
}

// Returns true if the button state has changed since last check
bool Button::BtnChanged() 
{
	return btn_->update();
}

// Returns true if button is currently depressed
bool Button::BtnPushed() 
{
	return !btn_->read();
}
