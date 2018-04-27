//
//	Morse Code Playback Functions
//	Mark Jessop 2018-04
//
//  Based on code from https://github.com/Paradoxis/Arduino-morse-code-translator/blob/master/main.ino
//
#include "morse.h"

// Letters
char* MORSE_LETTERS[] = {
  ".-",     // A
  "-...",   // B
  "-.-.",   // C
  "-..",    // D
  ".",      // E
  "..-.",   // F
  "--.",    // G
  "....",   // H
  "..",     // I
  ".---",   // J
  "-.-",    // K
  ".-..",   // L
  "--",     // M
  "-.",     // N
  "---",    // O
  ".--.",   // P
  "--.-",   // Q
  ".-.",    // R
  "...",    // S
  "-",      // T
  "..-",    // U
  "...-",   // V
  ".--",    // W
  "-..-",   // X
  "-.--",   // Y
  "--.."    // Z
};



// Numerals.
char* MORSE_NUMBERS[] = {
  "-----",   // 0
  ".----",   // 1
  "..---",   // 2
  "...--",   // 3
  "....-",   // 4
  ".....",   // 5
  "-....",   // 6
  "--...",   // 7
  "---..",   // 8
  "----."    // 9
};

// Symbols (though we handle this in a bit of a hacky way.)
char* MORSE_SYMBOLS[] = {
	".-.-.-" // .
};


// Send a single character
void sendDotOrDash (char unit) {

  radio_enable_tx();

  // Unit is a dot (500 ms)
  if (unit == MORSE_DOT) {
    _delay_ms(MORSE_DELAY_DOT);
  }

  // Unit is a dash (1500 ms)
  if (unit == MORSE_DASH) {
    _delay_ms(MORSE_DELAY_DASH);
  }

  // Inter-element gap
  radio_inhibit_tx();
  _delay_ms(MORSE_DELAY); 
}


void sendMorseSequence (char* sequence) {

  // Counter
  int i = 0;

  // Loop through every character until an 'end-of-line' (null) character is found
  while (sequence[i] != '\0') {
    sendDotOrDash(sequence[i]);
    i++;
  }

  // Delay between every letter
  _delay_ms(MORSE_DELAY * 3);
}



void sendMorse(char* message){

	int i = 0;
	while (message[i] != '\0'){
		char current = message[i];

		// Lower case letters
	    if (current >= 'a' && current <= 'z') {
	      sendMorseSequence(MORSE_LETTERS[current - 'a']);
	    }

	    // Capital case letters
	    else if (current >= 'A' && current <= 'Z') {
	      sendMorseSequence(MORSE_LETTERS[current - 'A']);
	    }

	    // Numbers
	    else if (current >= '0' && current <= '9') {
	      sendMorseSequence(MORSE_NUMBERS[current - '0']);
	    }

	    else if (current == '.'){
	    	sendMorseSequence(MORSE_SYMBOLS[0]);
	    }

	    // Space character (3500  ms)
	    else if (current == ' ') {
	      _delay_ms(MORSE_DELAY_SPACE);
	    }

	    // Treat all other characters as a space.
	    else{
	    	_delay_ms(MORSE_DELAY_SPACE);
	    }

		i++;
	}

	radio_disable_tx();
}


