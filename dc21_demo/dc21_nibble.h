/*
  dc21_nibble.h - Library
  created July 23, 2013
*/

#ifndef DC21_NIBBLE
#define DC21_NIBBLE

#include "Arduino.h"
#include "dc21_badge.h"
#include "dc21_text.h"

class DC21_nibble {
	public:
		DC21_nibble(DC21_badge* badgeToLinkTo); // Constructor
		
		void play();  // Start a game
		void reset(); // Reset variables
		void gameOver(); // Thy game is over.
		
		void getNextApple(); // Get the next apple location
		bool snakeCollision(byte collision_location); // Check location for possible collision
		void addNewLocationToStack(); // Add the new Snake Location to the 'top' of the stack, update as needed

	private:	
		DC21_badge* badge;     // Pointer to the badge instance
		
		byte nibbleStack[256]; // up to 256 locations. High 4 bits are X-axis, and low 4 bits are Y-Axis
		unsigned int nibbleStack_length; // Length of locations on the nibbleStack
		unsigned int snake_length; // Length of the snake, including hidden portions after picking up an apple
		unsigned int snakeSpeed; // Current snakeSpeed, lower is faster
		unsigned int score; // Current score
		char direction; // Current Direction
		byte apple_location; // Current Apple Location
		byte snake_location; // Current Snake Location
		long lastUpdate; // Game timer
		bool stillPlaying; // Are you still playing?
};


#endif