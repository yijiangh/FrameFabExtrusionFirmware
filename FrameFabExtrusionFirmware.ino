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
 */

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

// MODE INITIALIZATION
//int mode = TEMP_MODE;
//std::array<int, 4> modeVal = { TEMP_VAL, EXTRUDE_VAL, FAN_VAL, SERVO_VAL };
//std::array<int, 4> modeMin = { TEMP_MIN, EXTRUDE_MIN, FAN_MIN, SERVO_MIN };
//std::array<int, 4> modeMax = { TEMP_MAX, EXTRUDE_MAX, FAN_MAX, SERVO_MAX };
//std::array<int, 4> modeHue = { TEMP_HUE, EXTRUDE_HUE, FAN_HUE, SERVO_HUE };

// Retract duration in us should be retract length (mm) divided by retract speed (mm per s) * 1E3f
const int RETRACT_DURATION = (int)(1E3f * RETRACT_LENGTH / RETRACT_SPEED);

// OBJECTS
// Manual control button
Button button_enable(BUTTON_ENABLE, DEBOUNCE_INTERVAL);
Button button_inv(BUTTON_INV, DEBOUNCE_INTERVAL);

// Manual control potentiometer
AnalogReader potentiometer(POTENTIOMETER_PIN, FILTER_FREQUENCY);

TimerObject *ptimer = new TimerObject(200);
Stepper stepper(
	STEP_PIN,
	DIR_PIN,
	MM_PER_STEP * MM_OUT_OVER_IN,
	ptimer);

// KUKA Inputs
// Treat them as switches to be debounced
Bounce kuka_enable = Bounce();
Bounce kuka_inv    = Bounce();
AnalogReader kuka_analog(KUKA_ANALOG, FILTER_FREQUENCY);

// STATE VARIABLES

bool extruding = false;
bool retracting = false;
bool btn_held = false;
float last_potm_val = 0;
float last_kanalog_val = 0;
long  last_btn_press;
elapsedMillis since_retract;

void updateMode(int _mode) 
{
	//switch (_mode) 
	//{
	//case EXTRUDE_MODE:
	//	if (!retracting) {
	//		// Don't change speed when it's retracting, when it stops retracting then this will be called again.
	//		stepper.setSpeed(modeVal[EXTRUDE_MODE] / 10.0);
	//	}
	//	break;
	//}
}

void CheckKUKAEnable() 
{
	//Serial.println(kuka_enable.update());
	kuka_enable.update();
	Serial.println(kuka_enable.read());
	//if (kuka_enable.update()) 
	//{
	//	if (kuka_enable.read())
	//	{ 
	//		StartExtrude(); 
	//	}
	//	else 
	//	{
	//		StartRetract(); 
	//	}
	//}
}

void CheckKUKAInv()
{
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

		if (last_kanalog_val != ratio)
		{
			last_kanalog_val = ratio;
			stepper.SetKAnalogRatio(ratio);
		}
	}
}

void StartExtrude()
{
	StopRetract(); // if at all
	extruding = true;
	digitalWrite(EXT_LED, HIGH);
	stepper.SetDir(0);
	stepper.Enable();
}

void StartInvExtrude()
{
	if (extruding)
	{
		StopRetract(); // if at all
		extruding = true;
		digitalWrite(EXT_LED, HIGH);
		stepper.SetDir(!stepper.GetDir());
		stepper.Enable();
	}
}

void StartRetract() 
{
	extruding = false;
	retracting = true;
	digitalWrite(EXT_LED, LOW);
	digitalWrite(RET_LED, HIGH);
	stepper.SetSpeed(RETRACT_SPEED);
	stepper.SetDir(0);
	stepper.Enable();
	since_retract = 0;
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
		stepper.SetDir(1);
		updateMode(EXTRUDE_MODE);
		stepper.Disable();
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
  
  pinMode(KUKA_INV, INPUT_PULLUP);
  kuka_inv.attach(KUKA_INV);	   // Attach knob button
  kuka_inv.interval(DEBOUNCE_INTERVAL);

  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);

  updateMode(EXTRUDE_MODE);
}

void loop()
{
	CheckBtnEnable();
	CheckBtnInv();
	CheckPotentiometer();
	CheckKUKAEnable();
	//// CheckKUKAInv();

	stepper.Update();

	//if (retracting && (since_retract > RETRACT_DURATION))
	//{
	//	StopRetract();
	//}
}