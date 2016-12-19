#include "Stepper.h"

Stepper::Stepper(
	int step_pin,
	int dir_pin,
	int mstep_pin,
	int reset_pin,
	int sleep_pin,
	float mm_per_step,
	TimerObject *ptimer,
	int initialSpeed,
	bool initialDir):
	step_pin_(step_pin),
	dir_pin_(dir_pin),
	mstep_pin_(mstep_pin),
	reset_pin_(reset_pin),
	sleep_pin_(sleep_pin),
	mm_per_step_(mm_per_step),
	potm_ratio_(0),
	kanalog_ratio_(0)
{
	// Initialize Pins
	pinMode(step_pin_,  OUTPUT);
	pinMode(dir_pin_,   OUTPUT);
	pinMode(mstep_pin_, OUTPUT);
	pinMode(reset_pin_, OUTPUT);
	pinMode(sleep_pin_, OUTPUT);

	pstep_timer_ = ptimer;

	SetSpeed(initialSpeed, false);

	digitalWrite(dir_pin_, initialDir);
	digitalWrite(mstep_pin_, HIGH);		 // 1/32 microstep
	digitalWrite(reset_pin_, HIGH);		 // Never Reset
	digitalWrite(sleep_pin_, LOW);	 // Start disabled
}

Stepper::~Stepper()
{
	//delete pstep_timer_;
}

void Stepper::Enable(bool incre)
{
	digitalWrite(sleep_pin_, HIGH);
	SetTimer(incre);
}

void Stepper::Disable()
{
	digitalWrite(sleep_pin_, LOW);
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
	
	//Serial.println(incre);
	//pstep_timer_->setInterval(us_per_step_ / 2);

	if (incre)
	{
		pstep_timer_->setInterval(us_per_step_
			+ (int)POTM_SPEED_INCREASE		* 0.73//potm_ratio_
			+ (int)KANALOG_SPEED_INCREASE   *  kanalog_ratio_);
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