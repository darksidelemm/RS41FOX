// RS41FOX Config File.

#ifndef RS41HUP_CONFIG_H
#define RS41HUP_CONFIG_H

#ifdef USE_EXTERNAL_CONFIG
#include "config_external.h"
#else


//************GLOBAL Settings*****************
#define TRANSMIT_FREQUENCY  434.650f //Mhz middle frequency
#define CALLSIGN "N0CALL" // Put your callsign here, max. 15 characters

// TX Power
#define TX_POWER  5 // PWR 0...7 0- MIN ... 7 - MAX
// Power Levels measured at 434.650 MHz, using a Rigol DSA815, and a 10 kHz RBW
// Power measured by connecting a short (30cm) length of RG316 directly to the
// antenna/ground pads at the bottom of the RS41 PCB.
// 0 --> -1.9dBm
// 1 --> 1.3dBm
// 2 --> 3.6dBm
// 3 --> 7.0dBm
// 4 --> 10.0dBm
// 5 --> 13.1dBm - DEFAULT
// 6 --> 15.0dBm
// 7 --> 16.3dBm


//*************BEACON SETTINGS****************
// Morse Ident speed.
#define MORSE_WPM	20

// CW beacon on/off time.
#define ON_TIME		10
#define OFF_TIME	1

// On/Off Repeats.
#define ONOFF_REPEATS 1

// Low-Voltage Beacon 
// If enabled (uncommment), beacon the lat/lon of the fox in the CW ident when the battery voltage gets low.
//#define LOW_VOLTAGE_BEACON 1
#define LOW_VOLTAGE_BEACON_THRESHOLD 2.5 // Volts

// Low-Voltage Cutout - Kill power if the supply drops below this level.
#define LOW_VOLTAGE_CUTOUT 2.0 // Volts

//*************RTTY SETTINGS******************
// Used if the RTTY 'emergency' message is enabled.
#define BAUD_RATE  100 // RTTY Message Baud rate
#define RTTY_DEVIATION 0x3	// RTTY shift = RTTY_DEVIATION x 270Hz
#define RTTY_7BIT   1 // if 0 --> 5 bits
#define RTTY_USE_2_STOP_BITS   1

#endif

#endif //RS41HUP_CONFIG_H