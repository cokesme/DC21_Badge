/*
  dc21_tetris.cpp - Library
  created July 23, 2013
*/

#include "Arduino.h"
#include "dc21_tetris.h"
#include "dc21_badge.h"
#include "dc21_text.h"

// Default Constructor
DC21_tetris::DC21_tetris(DC21_badge* badgeToLinkTo) {
	badge = badgeToLinkTo;
	
	reset();
}


// Reset the game
void DC21_tetris::reset() {

}


// Play the game
void DC21_tetris::play() {
	// If both buttons are pressed, leave the game
	while(!badge->bothButtons_debounced(200)) {
		// Main game loop
		
		// check the buttons
		if(badge->button1_debounced()){
			//
		}

		// reading the bottom button 
		if(badge->button2_debounced()){
			//
		}
		
	}
}