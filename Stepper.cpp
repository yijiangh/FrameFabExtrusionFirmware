#include "Stepper.h"


Stepper::Stepper(
	int stepPin,
	int dirPin,
	int mStepPin,
	float mmPerStep,
	int initialSpeed,
	bool initialDir):
	_stepPin(stepPin),
	_dirPin(dirPin),
	_mStepPin(mStepPin),
	_MM_PER_STEP(mmPerStep)
{
	// Initialize Pins
	pinMode(_stepPin, OUTPUT);
	pinMode(_dirPin, OUTPUT);
	pinMode(_mStepPin, OUTPUT);

	setSpeed(initialSpeed, false);

	digitalWrite(_dirPin, initialDir);
	digitalWrite(_mStepPin, HIGH); // 1/32 micro stepping
}

Stepper::~Stepper()
{
}

void Stepper::enable()
{
	setTimer();
}

void Stepper::disable()
{
	stepTimer->Stop();
}

void Stepper::setSpeed(float speed, bool run)
{
	// speed is mm per second
	// MM_PER_STEP is mm per step
	// I want microseconds per step

	usPerStep = (int)(_MM_PER_STEP / speed * 1E6f);
	if (run)
	{
		SetTimer();
	}
}

bool stepOn;
void STEPISR()
{
	stepOn = !stepOn;
	digitalWrite(_stepPin, stepOn);
}

void Stepper::setTimer()
{
	stepTimer->Stop();
	stepTimer->setInterval(usPerStep / 2);
	stepTimer->setOnTimer(&STEPISR);
	stepTimer->Start();
}

void Stepper::setDir(bool dir) 
{
	digitalWrite(_dirPin, dir);
}