/*
  dc21_nibble.cpp - Library
  created July 23, 2013
*/

#include "Arduino.h"
#include "dc21_nibble.h"
#include "dc21_badge.h"
#include "dc21_text.h"

// Default Constructor
DC21_nibble::DC21_nibble(DC21_badge* badgeToLinkTo) {
	badge = badgeToLinkTo;
	randomSeed(millis()); // See the "random" number generator
}


// Reset the game
void DC21_nibble::reset() {
	for(int x = 0; x < 256; x++){
		nibbleStack[x] = 0;
	}
	snake_location = 0b01110111; // location of snake
	apple_location = 0; // location of apple
	nibbleStack[0] = snake_location;
	nibbleStack[1] = snake_location;
	nibbleStack[2] = snake_location;
	nibbleStack_length = 3; // size of the nibbleStack. Locations the snake is occupying
	snakeSpeed = 250; // milliseconds between moves, lower is faster
	direction = 's';  // s->stop, u->up, d->down, l->left, r->right
	score = 0;        // No game, no score. Sounds harse. Isn't.
	stillPlaying = true; // Set to false to end game...
	lastUpdate = millis(); // Game timer
	badge -> clear();
	
	badge->setPixel((snake_location >> 4),(snake_location & 0b00001111),3);
	getNextApple();
}


// Get the next apple location
void DC21_nibble::getNextApple(){
	bool foundSpot = false;
	
	while(!foundSpot){
		// Generate a random spot
		apple_location = (byte) random(256);
		
		if(!snakeCollision(apple_location)){
			foundSpot = true;
		}
	}
	badge->setPixel((apple_location >> 4),(apple_location & 0b00001111),2);	
}


// Check location for possible collision
bool DC21_nibble::snakeCollision(byte collision_location){ // Check location for possible collision
	// Check if snake is already using the spot
	for(int temp = 0; temp < nibbleStack_length; temp ++){
		if(nibbleStack[temp] == collision_location){
			return true; // Collision!
		}
	}
	return false; // No collision here. Continue
}


// Add the new Snake Location to the 'top' of the stack, update as needed
void DC21_nibble::addNewLocationToStack(){
	// Check for Apple at new location
	if(snake_location == apple_location){
		getNextApple();
		// TODO: Scoring, Speed Increase, etc.=================================================================
		snake_length++; // Increase overall snake length
		nibbleStack_length++;
	}

	badge->setPixel((nibbleStack[nibbleStack_length-1] >> 4),(nibbleStack[nibbleStack_length-1] & 0b00001111),0);
	
	// Move all positions down, add to stack. Check for tail increase / decrease
	for(int x = nibbleStack_length; x > 0; x--){
		nibbleStack[x] = nibbleStack[x-1]; // Shift all of the nibble stack down to make room for the new one
	}
	nibbleStack[0] = snake_location;
	
	// Draw Changes on the playing field
	if(nibbleStack_length >1){
		// Set the previous head to full brightness
		badge->setPixel((nibbleStack[1] >> 4),(nibbleStack[1] & 0b00001111),1);
		// Set the previous head to full brightness
		badge->setPixel((nibbleStack[nibbleStack_length-1] >> 4),(nibbleStack[nibbleStack_length-1] & 0b00001111),1);
	}
	// Set the head to full brightness
	badge->setPixel((nibbleStack[0] >> 4),(nibbleStack[0] & 0b00001111),3);
}


// Play the game
void DC21_nibble::play() {
	reset(); // Reset all the things
	
	// If both buttons are pressed, leave the game
	while(stillPlaying) {
		// Main game loop
		
		// Check if player wants to stop
		if(badge->bothButtons_debounced(200)){
			stillPlaying = false;
		}
		
		// Check the controls, update direction as needed
		if(badge->stick_x() < 8){
			if(direction !='r') direction = 'l';
		} else if (badge->stick_x() > 21){
			if(direction !='l') direction = 'r';
		} else if (badge->stick_y() < 8){
			if(direction !='d') direction = 'u';
		} else if (badge->stick_y() > 21){
			if(direction !='u') direction = 'd';
		}
		
		// Check for game update time, process commands
		if(millis() > lastUpdate + snakeSpeed){
			lastUpdate = millis(); // Update the game timer
			
			// Try to move the snake in the right direction
			switch(direction){
				case 'l':
					if((snake_location & 0b11110000) == 0b00000000){
						// Ran into a wall. Dead.
						gameOver();
					} else {
						// Where should this player be?
						byte tempByte = ((snake_location >> 4)-1) << 4;
						snake_location = (snake_location & 0b00001111) | tempByte;
						
						// Check collisions
						if(snakeCollision(snake_location)){
							gameOver(); // Snake has ran into itself. :(
						}
						addNewLocationToStack(); // Either empty, or has apple
					}
					break;
					
				case 'r':
					if((snake_location & 0b11110000) == 0b11110000){
						// Ran into a wall. Dead.
						gameOver();
					} else {
						// Where should this player be?
						byte tempByte = ((snake_location >> 4)+1) << 4;
						snake_location = (snake_location & 0b00001111) | tempByte;
						
						// Check collisions
						if(snakeCollision(snake_location)){
							gameOver(); // Snake has ran into itself. :(
						}
						addNewLocationToStack(); // Either empty, or has apple
					}
					break;
					
				case 'u':
					if((snake_location & 0b00001111) == 0b00000000){
						// Ran into a wall. Dead.
						gameOver();
					} else {
						// Where should this player be?
						snake_location = snake_location - 1;
						
						// Check collisions
						if(snakeCollision(snake_location)){
							gameOver(); // Snake has ran into itself. :(
						}
						addNewLocationToStack(); // Either empty, or has apple
					}
					break;
					
				case 'd':
					if((snake_location & 0b00001111) == 0b00001111){
						// Ran into a wall. Dead.
						gameOver();
					} else {
						// Where should this player be?
						snake_location = snake_location + 1;
						
						// Check collisions
						if(snakeCollision(snake_location)){
							gameOver(); // Snake has ran into itself. :(
						}
						addNewLocationToStack(); // Either empty, or has apple
					}
					break;
			}
		}
	}
}


// Thy game is over.
void DC21_nibble::gameOver(){
	// GAME OVER SHIT HERE!
	stillPlaying = false;
}

