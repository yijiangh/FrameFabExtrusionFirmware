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

#include <elapsedMillis.h>
#include <TimerObject.h>
#include <Bounce2.h>
#include "Options.h"
#include "Pins.h"
#include "Stepper.h"

// values for status control
int ButtonEnaVal_A    = 0;
int ButtonEnaOldVal_A = 0;
int ButtonFlag        = 0;
int ButtonDirVal_A    = 0;
int ButtonDirOldVal_A = 0;
int UrDirOldVal_A       = 0;
int UrDirVal_A          = 0;
int UrFlag              = 0;
int UrEnaVal_A          = 0;
int UrEnaOldVal_A       = 0;

int MotorSpeed_A      = 0;
int UrAnalogOut       = 0;
int MotorState_A      = 0;
int MotorInv_A        = 1;

int MS_Button = 0;
int MS_Ur     = 0;

// MODE INITIALIZATION
//int mode = TEMP_MODE;
//std::array<int, 4> modeVal = { TEMP_VAL, EXTRUDE_VAL, FAN_VAL, SERVO_VAL };
//std::array<int, 4> modeMin = { TEMP_MIN, EXTRUDE_MIN, FAN_MIN, SERVO_MIN };
//std::array<int, 4> modeMax = { TEMP_MAX, EXTRUDE_MAX, FAN_MAX, SERVO_MAX };
//std::array<int, 4> modeHue = { TEMP_HUE, EXTRUDE_HUE, FAN_HUE, SERVO_HUE };

// Retract duration in us should be retract length (mm) divided by retract speed (mm per s) * 1E3f
const int RETRACT_DURATION = (int)(1E3f * RETRACT_LENGTH / RETRACT_SPEED);

// OBJECTS
Stepper stepper(
	STEP_PIN,
	DIR_PIN,
	MSTEP_PIN,
	MM_PER_STEP * MM_OUT_OVER_IN);

// KUKA Inputs
// Treat them as switches to be debounced
Bounce KUKAIN1 = Bounce();
Bounce KUKAIN2 = Bounce();

// STATE VARIABLES
bool extruding = false;
bool retracting = false;
bool btnHeld = false;
long lastBtnPress;
elapsedMillis sinceRetract;

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

void checkKUKA() 
{
	if (KUKAIN1.update()) 
	{
		if (KUKAIN1.read()) 
		{ 
			startExtrude(); 
		}
		else 
		{ 
			startRetract(); 
		}
	}
}

void startExtrude() 
{
	stopRetract(); // if at all
	extruding = true;
	digitalWrite(EXT_LED, HIGH);
	stepper.enable();
}

void startRetract() 
{
	extruding = false;
	retracting = true;
	digitalWrite(EXT_LED, LOW);
	digitalWrite(RET_LED, HIGH);
	stepper.setSpeed(RETRACT_SPEED);
	stepper.setDir(0);
	stepper.enable();
	sinceRetract = 0;
}

void stopRetract() 
{
	if (retracting) {
		retracting = false;
		digitalWrite(RET_LED, LOW);
		stepper.setDir(1);
		updateMode(EXTRUDE_MODE);
		stepper.disable();
	}
}

void setup() {
  //Serial.begin(9600);
  
  pinMode(MotorDir_A, OUTPUT);
  pinMode(MotorStp_A, OUTPUT);
  pinMode(MotorEna_A, OUTPUT);

  pinMode(MotorStart_A, INPUT_PULLUP);
  pinMode(MotorInvert_A, INPUT_PULLUP);
  pinMode(UrInv, INPUT);
  pinMode(UrStart, INPUT);

  pinMode(S_LED1, OUTPUT);
  pinMode(S_LED2, OUTPUT);

  //--------------------------------
  Serial.begin(9600); // USB serial for debugging

  // knob.encWrite(modeVal[mode]);

  // Setup LED Indicators
  pinMode(EXT_LED, OUTPUT);
  pinMode(RET_LED, OUTPUT);

  // Setup auxiliary fan pin
  pinMode(AUX_FAN_PIN, OUTPUT);

  // Setup KUKA inputs
  pinMode(KUKA_PIN_1, INPUT_PULLUP);
  KUKAIN1.attach(KUKA_PIN_1); // Attach knob button
  KUKAIN1.interval(DEBOUNCE_INTERVAL);
  pinMode(KUKA_PIN_2, INPUT_PULLUP);
  KUKAIN2.attach(KUKA_PIN_2); // Attach knob button
  KUKAIN2.interval(DEBOUNCE_INTERVAL);

  updateMode(EXTRUDE_MODE);
}

