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
#define DEBOUNCE_INTERVAL 5 // in ms
#define FILTER_FREQUENCY  5 // filters out analog change faster than 5 HZ

// Extrusion Options 
// - these parameters are not well-caliberated now
// - parameter tuning base on guess-and-check
#define MM_PER_STEP 0.00345 // Calibrated from several measurements
#define MM_OUT_OVER_IN 1.90 // How many mm come out when 1 mm goes in, depends on nozzle, calibrated from measurement

#define EXTRUDE_INITAL_SPEED 400
#define RETRACT_SPEED	 	 100
#define RETRACT_LENGTH		 3	// mm

#define POTM_SPEED_INCREASE		100
#define KANALOG_SPEED_INCREASE	1000

#define FORTH_DIRECTION 0 // extrude    direction
#define BACK_DIRECTION	1 // retraction direction

#endif