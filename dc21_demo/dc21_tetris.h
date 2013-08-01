/*
  dc21_tetris.h - Library
  created July 23, 2013
*/

#ifndef DC21_TETRIS
#define DC21_TETRIS

#include "Arduino.h"
#include "dc21_badge.h"
#include "dc21_text.h"

class DC21_tetris{
	public:
		DC21_tetris(DC21_badge* badgeToLinkTo); // Constructor

		void play();  // Start a game
		void reset(); // Reset variables, and start a new game
			
	private:	
		DC21_badge* badge;     // Pointer to the badge instance
};


#endif