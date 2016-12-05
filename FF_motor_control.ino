/* Stepper test module
Design by Yu Lei for MPP 3DP industry
*/

// ReModified by Yijiang Huang in 2016/8/12, for fiber printing project, ASW-USTC

// Pin assignments for Motor A and Motor B
// inculding Pin direction, steps, and enable
// Motors are controlled by a trimpot for speed, a buttion for On/Off and a button for direction

#include <array>
#include <Bounce2.h>
//#include "Options.h"
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
int mode = TEMP_MODE;
std::array<int, 4> modeVal = { TEMP_VAL, EXTRUDE_VAL, FAN_VAL, SERVO_VAL };
std::array<int, 4> modeMin = { TEMP_MIN, EXTRUDE_MIN, FAN_MIN, SERVO_MIN };
std::array<int, 4> modeMax = { TEMP_MAX, EXTRUDE_MAX, FAN_MAX, SERVO_MAX };
std::array<int, 4> modeHue = { TEMP_HUE, EXTRUDE_HUE, FAN_HUE, SERVO_HUE };

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

void updateMode(int _mode = mode) 
{
	switch (_mode) 
	{
	case EXTRUDE_MODE:
		if (!retracting) {
			// Don't change speed when it's retracting, when it stops retracting then this will be called again.
			stepper.setSpeed(modeVal[EXTRUDE_MODE] / 10.0);
		}
		break;
	}
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


void loop() {

  // Speed Button on Box
  MotorSpeed_A  = analogRead(A0);

  // UrAnalog output
  UrAnalogOut = analogRead(A1);

  // Button on Box for on/off and motor inverse
  ButtonEnaVal_A = digitalRead(MotorStart_A);
  ButtonDirVal_A = digitalRead(MotorInvert_A);

  //when not pressing, start_button read = 1
  //Serial.println(ButtonEnaVal_A);

  ButtonChangeMotorState();
  ButtonInverseMotor();

  // Ur motor on/off and inverse
  UrEnaVal_A = digitalRead(UrStart);
  UrDirVal_A = digitalRead(UrInv);
  UrChangeMotorState();
  UrInverseMotor();

  AlterInverse();

  ButtonSpeed();
  UrAnalogSpeed();

  //400
  int sp_param = 200;

  digitalWrite(MotorStp_A, HIGH);

  delayMicroseconds(sp_param + MS_Button + MS_Ur);
    
  digitalWrite(MotorStp_A, LOW);

  delayMicroseconds(sp_param + MS_Button + MS_Ur);

  // -------------------------------
  //checkBtn();
  //checkKnob();
  checkKUKA();
  if (retracting && (sinceRetract > RETRACT_DURATION) && !(btnHeld))
  {
	  stopRetract(); 
  }
}

void ButtonChangeMotorState()
{
  // Button Start motor
  if ((ButtonEnaVal_A == 0) && (ButtonEnaOldVal_A == 1))
  {
    MotorState_A = 1 - MotorState_A;
  }

  ButtonEnaOldVal_A = ButtonEnaVal_A;

  if (MotorState_A == 1)
  {
    // Motor running
    digitalWrite (MotorEna_A, LOW);
  }
  else
  {
    digitalWrite (MotorEna_A, HIGH);
  }
}

void UrChangeMotorState()
{
  // Button Start motor
  if (UrEnaVal_A == 1 && UrEnaOldVal_A == 0)
  {
    MotorState_A = 1;
    delay (10);
  }

  if (UrEnaVal_A == 0 && UrEnaOldVal_A == 1)
  {
    MotorState_A = 0;
    delay (10);
  }

  UrEnaOldVal_A = UrEnaVal_A;

  if (MotorState_A == 1)
  {
    digitalWrite (MotorEna_A, LOW);
  }
  else
  {
    digitalWrite (MotorEna_A, HIGH);
  }
}

void ButtonInverseMotor()
{
  // Serial.println(MotorInv_A);
  // Inverse motor
  if ((ButtonDirVal_A == 1) && (ButtonDirOldVal_A == 0))
  {
    MotorInv_A = 1 - MotorInv_A;
    delay (10);
  }
  ButtonDirOldVal_A = ButtonDirVal_A;
}

void UrInverseMotor()
{
  if ((UrDirVal_A == 1) && (UrDirOldVal_A == 0))
  {
    MotorInv_A = 1;
    delay (10);
  }

  if ((UrDirVal_A == 0) && (UrDirOldVal_A == 1))
  {
    MotorInv_A = 0;
    delay (10);
  }

  UrDirOldVal_A = UrDirVal_A;
}

void AlterInverse()
{
  if (MotorState_A == 1)
  {
    if (MotorInv_A == 1)
    {
      digitalWrite(S_LED1, LOW);
      digitalWrite(S_LED2, HIGH);
    }
    else
    {
      digitalWrite(S_LED1, HIGH);
      digitalWrite(S_LED2, LOW);
    }

    if (MotorInv_A == 0)
    {
      digitalWrite (MotorDir_A, LOW);
    }
    else
    {
      digitalWrite (MotorDir_A, HIGH);
    }
  }
}

void ButtonSpeed()
{
  double top = 130;
  double intv = top / 9;
  double range = 1000;

  if (MotorSpeed_A < intv)
  {
    MS_Button = (int)range / 9 * 8;
    //  digitalWrite(S_LED1,HIGH);
    //  digitalWrite(S_LED2,LOW);
    //  digitalWrite(S_LED3,LOW);
    //  digitalWrite(S_LED4,LOW);
    //  digitalWrite(S_LED5,LOW);
  }

  if (MotorSpeed_A < intv * 2 && MotorSpeed_A >= intv)
  {
    MS_Button = (int)range / 9 * 7;
    //  digitalWrite(S_LED1,HIGH);
    //  digitalWrite(S_LED2,HIGH);
    //  digitalWrite(S_LED3,LOW);
    //  digitalWrite(S_LED4,LOW);
    //  digitalWrite(S_LED5,LOW);
  }

  if (MotorSpeed_A < intv * 3 && MotorSpeed_A >= intv * 2)
  {
    MS_Button = (int)range / 9 * 6;
    //  digitalWite(S_LED1,HIGH);
    //  digitalWrite(S_LED2,HIGH);
    //  digitalWrite(S_LED3,LOW);
    //  digitalWrite(S_LED4,LOW);
    //  digitalWrite(S_LED5,LOW);
  }

  if (MotorSpeed_A < intv * 4 && MotorSpeed_A >= intv * 3)
  {
    MS_Button = (int)range / 9 * 5;
    //  digitalWrite(S_LED1,HIGH);
    //  digitalWrite(S_LED2,HIGH);
    //  digitalWrite(S_LED3,HIGH);
    //  digitalWrite(S_LED4,LOW);
    //  digitalWrite(S_LED5,LOW);
  }

  if (MotorSpeed_A < intv * 5 && MotorSpeed_A >= intv * 4)
  {
    MS_Button = (int)range / 9 * 4;
    //  digitalWrite(S_LED1,HIGH);
    //  digitalWrite(S_LED2,HIGH);
    //  digitalWrite(S_LED3,HIGH);
    //  digitalWrite(S_LED4,HIGH);
    //  digitalWrite(S_LED5,LOW);
  }

  if (MotorSpeed_A < intv * 6 && MotorSpeed_A >= intv * 5)
  {
    MS_Button = (int)range / 9 * 3;
    //  digitalWrite(S_LED1,HIGH);
    //  digitalWrite(S_LED2,HIGH);
    //  digitalWrite(S_LED3,HIGH);
    //  digitalWrite(S_LED4,HIGH);
    //  digitalWrite(S_LED5,LOW);
  }

  if (MotorSpeed_A < intv * 7 && MotorSpeed_A >= intv * 6)
  {
    MS_Button = (int)range / 9 * 2;
    //  digitalWrite(S_LED1,HIGH);
    //  digitalWrite(S_LED2,HIGH);
    //  digitalWrite(S_LED3,HIGH);
    //  digitalWrite(S_LED4,HIGH);
    //  digitalWrite(S_LED5,LOW);
  }

  if (MotorSpeed_A < intv * 8 && MotorSpeed_A >= intv * 7)
  {
    MS_Button = (int)range / 9 * 1;
    //  digitalWrite(S_LED1,HIGH);
    //  digitalWrite(S_LED2,HIGH);
    //  digitalWrite(S_LED3,HIGH);
    //  digitalWrite(S_LED4,HIGH);
    //  digitalWrite(S_LED5,LOW);
  }

  if (MotorSpeed_A < intv * 9 && MotorSpeed_A >= intv * 8)
  {
    MS_Button = (int)range / 9 * 0;
    //  digitalWrite(S_LED1,HIGH);
    //  digitalWrite(S_LED2,HIGH);
    //  digitalWrite(S_LED3,HIGH);
    //  digitalWrite(S_LED4,HIGH);
    //  digitalWrite(S_LED5,LOW);
  }
}

void UrAnalogSpeed()
{
  double top = 870;
  MS_Ur = (double((top - UrAnalogOut))/top) * 1000;

//  Serial.println(MS_Ur);
}
