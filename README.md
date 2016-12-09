#FrameFabExtrusionFirmware

 Author: Yijiang Huang
 This code is modified snippet taken from
 SLAM Extruder V2 Firmware from Mitchell Gu
 https://github.com/mitchgu/SLAMStruder-firmware

##Features:
- Manual control motor switch on/off button
- Manual control motor inverse		button
- Manual control motor feedtate		rotary button
- Controls stepper motor extrusion rate
- Reads two digital inputs from KUKA Arm
	- Dig[0] : switch extrusion on/off
	- Dig[1] : inverse extrusion
- Reads Analog signal from KUKA Arm to control motor feedrate

Note: the manual and automatic motor control can
 * interrupt each other, enabling user to manually 
 * control the motor while the firmware is listening 
 * to KUKA's signal.

##Dependencies:
- [Bounce2 library](https://github.com/thomasfredericks/Bounce2)
- [TimerObject](https://github.com/aron-bordin/ArduinoTimerObject)
- [ElapseMillis](https://github.com/pfeerick/elapsedMillis)