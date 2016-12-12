/* FF Extruder v2 Firmware
 * Author: Yijiang Huang
 * This code is modified snippet taken from
 * SLAM Extruder V2 Firmware from Mitchell Gu
 * https://github.com/mitchgu/SLAMStruder-firmware
 *
 * Features:
 * - Manual control motor switch on/off button
 * - Manual control motor inverse		button
 * - Manual control motor feedtate		rotary button
 * - Controls stepper motor extrusion rate
 * - Reads two digital inputs from KUKA Arm
 *		- Dig[0] : switch extrusion on/off
 *		- Dig[1] : inverse extrusion
 * - Reads Analog signal from KUKA Arm to control
 *	motor feedrate
 *
 * Note: the manual and automatic motor control can
 * interrupt each other, enabling user to manually 
 * control the motor while the firmware is listening 
 * to KUKA's signal.
 *
 * Dependencies:
 * - Bounce2 library (https://github.com/thomasfredericks/Bounce2)
 * - TimerObject	 (https://github.com/aron-bordin/ArduinoTimerObject)
 * - ElapseMillis	 (https://github.com/pfeerick/elapsedMillis)
 * - Filter			 (https://github.com/JonHub/Filters.git)
 */

#include <TimerObject.h>
#include <Bounce2.h>
#include <RunningStatistics.h>
#include <FloatDefine.h>
#include <FilterTwoPole.h>
#include <Filters.h>
#include <FilterOnePole.h>
#include <FilterDerivative.h>

#include <elapsedMillis.h>
#include "Options.h"
#include "Pins.h"
#include "Stepper.h"
#include "AnalogReader.h"
#include "Button.h"

// Retract duration in us should be retract length (mm) divided by retract speed (mm per s) * 1E3f
const int RETRACT_DURATION = (int) 500;
//const int RETRACT_DURATION = (int)(1E3f * RETRACT_LENGTH / RETRACT_SPEED);

// OBJECTS
// Manual control button
Button button_enable(BUTTON_ENABLE, DEBOUNCE_INTERVAL);
Button button_inv(BUTTON_INV, DEBOUNCE_INTERVAL);

// Manual control potentiometer
AnalogReader potentiometer(POTENTIOMETER_PIN, FILTER_FREQUENCY);

TimerObject *ptimer = new TimerObject(EXTRUDE_INITAL_SPEED);
Stepper stepper(
	STEP_PIN,
	DIR_PIN,
	MM_PER_STEP * MM_OUT_OVER_IN,
	ptimer,
	EXTRUDE_INITAL_SPEED);

// KUKA Inputs
// Treat them as switches to be debounced
Bounce kuka_enable = Bounce();
Bounce kuka_inv    = Bounce();
AnalogReader kuka_analog(KUKA_ANALOG, FILTER_FREQUENCY);

// STATE VARIABLES
bool extruding = false;
bool retracting = false;
bool rollback = false;
bool last_retract = false;
float last_potm_val = 0;
float last_kanalog_val = 0;

elapsedMillis since_retract;
elapsedMillis since_rollback;

// TEMPORAL STATE VARIABLES (PRETTY DUMB)
int last_kuka_enable_val = 0;
int last_kuka_inv_val = 0;

void updateMode(int _mode, bool execute) 
{
	switch (_mode) 
	{
	case EXTRUDE_MODE:
		if (!retracting) 
		{
			// Don't change speed when it's retracting,
			// when it stops retracting then this will be called again.
			stepper.SetSpeed(EXTRUDE_INITAL_SPEED, execute, true);
		}
		break;
	}
}

int msg1 = 0;
int msg2 = 0;

void CheckKUKAEnable() 
{
	// turn on /w off motor
	if (kuka_enable.update())
	{
		if (kuka_enable.read())
		{
			StartExtrude();
		}
		else
		{
			StartRetract();
		}
	}
}

void CheckKUKAInv()
{
	// not in use right now
	if (kuka_inv.update())
	{
		if (kuka_inv.read())
		{
			//change motor direction
		}
	}
}

void CheckBtnEnable()
{
	if (button_enable.BtnChanged())
	{
		// If button state changed
		if (button_enable.BtnPushed())
		{
			if(!extruding)
			{
				StartExtrude();
			}
			else
			{
				Serial.println("stop extrude!");
				StopExtrude();
			}
		}
	}
}

void CheckBtnInv()
{
	if (button_inv.BtnChanged())
	{
		// If button state changed
		if (button_inv.BtnPushed())
		{
			StartInvExtrude();
		}
	}
}

void CheckPotentiometer()
{
	if (extruding)
	{
		// change motor speed by potentiometer
		float ratio = (float)potentiometer.GetAnalog() / (float)1023;

		if (last_potm_val != ratio)
		{
			last_potm_val = ratio;
			stepper.SetPotmRatio(ratio);
		}
	}
}

void CheckKUKAAnalog()
{
	if (extruding)
	{
		// change motor speed by kuka analog input, the higher the faster
		float ratio = 1 - (float)kuka_analog.GetAnalog() / (float)1023;
		//Serial.println(ratio);

		if (last_kanalog_val != ratio)
		{
			last_kanalog_val = ratio;
			stepper.SetKAnalogRatio(ratio);
		}
	}
}

void StartExtrude()
{
	if (!last_retract)
	{
		StopRetract(); // if at all
		extruding = true;

		digitalWrite(RET_LED, LOW);
		digitalWrite(EXT_LED, HIGH);
			
		stepper.SetDir(FORTH_DIRECTION);
		stepper.Enable(true);
	}
	else
	{
		StartRollback();
	}
}

void StartInvExtrude()
{
	if (extruding)
	{
		//StopRetract(); // if at all
		digitalWrite(EXT_LED, HIGH);
		stepper.SetDir(!stepper.GetDir());
	}
}

void StartRetract() 
{
	extruding = false;
	retracting = true;

	digitalWrite(EXT_LED, LOW);
	digitalWrite(RET_LED, HIGH);

	stepper.SetDir(BACK_DIRECTION);
	stepper.SetSpeed(RETRACT_SPEED, true, false);
	since_retract = 0;

	// flag for rollback
	last_retract = true;
}

void StartRollback()
{
	extruding = false;
	rollback  = true;

	digitalWrite(EXT_LED, HIGH);
	digitalWrite(RET_LED, HIGH);
	
	stepper.SetDir(FORTH_DIRECTION);
	stepper.SetSpeed(RETRACT_SPEED, true, false);
	since_rollback = 0;
}

void StopExtrude()
{
	if(extruding)
	{
		extruding = false;
		stepper.Disable();
	}
}

void StopRetract() 
{
	if (retracting) 
	{
		retracting = false;
		digitalWrite(RET_LED, LOW);
		stepper.SetDir(FORTH_DIRECTION);
		updateMode(EXTRUDE_MODE, false);
		stepper.Disable();
	}
}

void StopRollback()
{
	if (rollback)
	{
		rollback = false;
		extruding = true;

		digitalWrite(RET_LED, LOW);
		stepper.SetDir(FORTH_DIRECTION);
		updateMode(EXTRUDE_MODE, true);
		last_retract = false;
	}
}

void setup() 
{
  Serial.begin(9600); // USB serial for debugging

  // Setup LED Indicators
  pinMode(EXT_LED, OUTPUT);
  pinMode(RET_LED, OUTPUT);

  // Setup KUKA inputs
  pinMode(KUKA_ENABLE, INPUT);
  kuka_enable.attach(KUKA_ENABLE); // Attach knob button
  kuka_enable.interval(DEBOUNCE_INTERVAL);

  pinMode(KUKA_INV, INPUT);
  kuka_inv.attach(KUKA_INV);	   // Attach knob button
  kuka_inv.interval(DEBOUNCE_INTERVAL);

  // Setup Motor control pins
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);

  updateMode(EXTRUDE_MODE, false);
}

void loop()
{
	CheckBtnEnable();
	CheckBtnInv();
	CheckPotentiometer();
	
	CheckKUKAEnable();
	// CheckKUKAAnalog();
	// CheckKUKAInv();

	if (retracting && (since_retract > RETRACT_DURATION))
	{
		StopRetract();
	}

	if (rollback && (since_rollback > RETRACT_DURATION))
	{
		StopRollback();
	}

	stepper.Update();
}