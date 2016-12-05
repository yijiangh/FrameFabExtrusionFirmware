/*
Options.h - General parameters for SLAMWare
Created by Mitchell Gu
Modified by Yijiang Huang 
@ MIT DS, Dec/2016
*/

#ifndef Options_h
#define Options_h

// MODE CONFIGURATION
#define EXTRUDE_MODE 1
#define EXTRUDE_VAL 50
#define EXTRUDE_MIN 10
#define EXTRUDE_MAX 200
#define EXTRUDE_HUE 120

// Display, Interface Options
#define DISPBAUD 9600
#define LONG_PRESS_TIME 300
#define DEBOUNCE_INTERVAL 5 // in ms

// Extrusion Options
#define MM_PER_STEP 0.00345 // Calibrated from several measurements
#define MM_OUT_OVER_IN 1.90 // How many mm come out when 1 mm goes in, depends on nozzle, calibrated from measurement
#define RETRACT_SPEED 10	// mm per s
#define RETRACT_LENGTH 3	// mm

#endif