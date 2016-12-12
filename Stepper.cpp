#include "Stepper.h"

Stepper::Stepper(
	int stepPin,
	int dirPin,
	float mmPerStep,
	TimerObject *ptimer,
	int initialSpeed,
	bool initialDir):
	step_pin_(stepPin),
	dir_pin_(dirPin),
	mm_per_step_(mmPerStep),
	potm_ratio_(0),
	kanalog_ratio_(0)
{
	// Initialize Pins
	pinMode(step_pin_,  OUTPUT);
	pinMode(dir_pin_,   OUTPUT);

	pstep_timer_ = ptimer;

	SetSpeed(initialSpeed, false);

	digitalWrite(dir_pin_, initialDir);
}

Stepper::~Stepper()
{
	//delete pstep_timer_;
}

void Stepper::Enable(bool incre)
{
	SetTimer(incre);
}

void Stepper::Disable()
{
	pstep_timer_->Stop();
}

void Stepper::SetSpeed(float speed, bool run, bool incre)
{
	speed_ = speed;
	us_per_step_ = ConvertSpeed();

	if (run)
	{
		SetTimer(incre);
	}
}

void Stepper::SetPotmRatio(float ratio)
{
	potm_ratio_ = ratio;
	SetSpeed(speed_, true);
}

void Stepper::SetKAnalogRatio(float ratio)
{
	//kanalog_ratio_ = ratio;
	kanalog_ratio_ = 0;
	SetSpeed(speed_, true);
}

bool step_on;
void STEPISR()
{
	step_on = !step_on;
	digitalWrite(STEP_PIN, step_on);
}

void Stepper::SetTimer(bool incre)
{
	pstep_timer_->Stop();
	
	//pstep_timer_->setInterval(us_per_step_ / 2);

	if (incre)
	{
		pstep_timer_->setInterval(us_per_step_
			+ (int)POTM_SPEED_INCREASE		* potm_ratio_
			+ (int)KANALOG_SPEED_INCREASE  * kanalog_ratio_);
	}
	else
	{
		pstep_timer_->setInterval(us_per_step_);
	}

	pstep_timer_->setOnTimer(&STEPISR);
	pstep_timer_->Start();
}

void Stepper::SetDir(bool dir) 
{
	digitalWrite(dir_pin_, dir);
	current_dir_ = dir;
}

void Stepper::Update()
{
	pstep_timer_->Update();
}

int Stepper::ConvertSpeed()
{
	// speed is mm per second
	// MM_PER_STEP is mm per step
	// I want microseconds per step

	return speed_;
	//return (int)(mm_per_step_ / speed_ * 1E6f);
}