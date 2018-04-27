//
//	Morse Code Playback Functions
//	Mark Jessop 2018-04
//
#ifndef RS41FOX_MORSE_H
#define RS41FOX_MORSE_H

#include "config.h"
#include "radio.h"
#include "delay.h"

// All morse delays
#define MORSE_DELAY 1200 / MORSE_WPM
#define MORSE_DELAY_DOT (MORSE_DELAY * 1)
#define MORSE_DELAY_DASH (MORSE_DELAY * 3)
#define MORSE_DELAY_SPACE (MORSE_DELAY * 7)

// All morse characters
#define MORSE_DOT '.'
#define MORSE_DASH '-'

void sendDotOrDash (char unit);
void sendMorseSequence (char* sequence);
void sendMorse(char* message);


#endif //RS41FOX_MORSE_H