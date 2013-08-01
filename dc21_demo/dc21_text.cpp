/*
  dc21_text.cpp - Library
  created July 23, 2013
*/

#include "Arduino.h"
#include "dc21_badge.h"
#include "dc21_text.h"


// Default Constructor
DC21_text::DC21_text(DC21_badge* badgeToLinkTo) {
	badge = badgeToLinkTo;

	scrollRate = 65;     // Milliseconds between updates.
	bounce = false;       // False = "Loop", True = "Bounce"
	timesToRepeat = 0;   // Number of times to repeat. 0 = Forever. FOR-EV-ER. FOR-EV-ER.	
	brightness = 3;      // 1 = low, 2 = medium, 3 = high

	startX = 0;          // Starting X coordinate in the frameBuffer
	startY = 0;          // Starting Y coordinate in the frameBuffer
	endX = 15;           // Ending X coordinate in the frameBuffer
	
	// For bitwise maths to set a bit
	bit16_set[0] = 0x0001;bit16_set[1] = 0x0002;bit16_set[2] = 0x0004;bit16_set[3] = 0x0008;
	bit16_set[4] = 0x0010;bit16_set[5] = 0x0020;bit16_set[6] = 0x0040;bit16_set[7] = 0x0080;
	bit16_set[8] = 0x0100;bit16_set[9] = 0x0200;bit16_set[10] = 0x0400;bit16_set[11] = 0x0800;
	bit16_set[12] = 0x1000;bit16_set[13] = 0x2000;bit16_set[14] = 0x4000;bit16_set[15] = 0x8000;
	bit8_set[0] = 0x01;bit8_set[1] = 0x02;bit8_set[2] = 0x04;bit8_set[3] = 0x08;
	bit8_set[4] = 0x10;bit8_set[5] = 0x20;bit8_set[6] = 0x40;bit8_set[7] = 0x80;
	// For bitwise maths to clear a bit
	bit16_clear[0] = 0xFFFE;bit16_clear[1] = 0xFFFD;bit16_clear[2] = 0xFFFB;bit16_clear[3] = 0xFFF7;
	bit16_clear[4] = 0xFFEF;bit16_clear[5] = 0xFFDF;bit16_clear[6] = 0xFFBF;bit16_clear[7] = 0xFF7F;
	bit16_clear[8] = 0xFEFF;bit16_clear[9] = 0xFDFF;bit16_clear[10] = 0xFBFF;bit16_clear[11] = 0xF7FF;
	bit16_clear[12] = 0xEFFF;bit16_clear[13] = 0xDFFF;bit16_clear[14] = 0xBFFF;bit16_clear[15] = 0x7FFF;
	bit8_clear[0] = 0xFE;bit8_clear[1] = 0xFD;bit8_clear[2] = 0xFB;bit8_clear[3] = 0xF7;
	bit8_clear[4] = 0xEF;bit8_clear[5] = 0xDF;bit8_clear[6] = 0xBF;bit8_clear[7] = 0x7F;

	reset();
}


// Reset ALL the things!
void DC21_text::reset() {
	direction = 1;
	timesRepeated = 0;
	lastUpdate = 0;
	currentOffset = 0;
	
	// Clear the mergeBuffer
	mergeBuffer[0] = 0x0000; mergeBuffer[1] = 0x0000; mergeBuffer[2] = 0x0000; mergeBuffer[3] = 0x0000;
	mergeBuffer[4] = 0x0000; mergeBuffer[5] = 0x0000; mergeBuffer[6] = 0x0000; mergeBuffer[7] = 0x0000;
	mergeBuffer[8] = 0x0000; mergeBuffer[9] = 0x0000; mergeBuffer[10] = 0x0000; mergeBuffer[11] = 0x0000;
	mergeBuffer[12] = 0x0000; mergeBuffer[13] = 0x0000; mergeBuffer[14] = 0x0000; mergeBuffer[15] = 0x0000;
	
	// Recalculate the clearBuffer mask
	clearBuffer = 0xFFFF;
	// Generate Clear Buffer Mask and Clear old data out of the frameBuffer (only where we want our text to go though.)
	for(int x = 0; x <= (endX - startX); x++){
		clearBuffer = clearBuffer & bit16_clear[15-x-startX];
	}
}


// Jump to a specific offset and update the frameBuffer
void DC21_text::jumpToOffset(unsigned int position){
	// Disable the scrollRate delay
	unsigned int oldScrollRate = scrollRate;
	scrollRate = 0;
	
	// Scroll the text in as fast as possible
	int temp = 0;
	if(position > 15) temp = position - 15;
	while( temp < position ){
		currentOffset = temp;
		update();
		temp++;
	}
	
	// Restore the original scroll rate delay
	scrollRate = oldScrollRate;
}


// Copy newText into textString
void DC21_text::setTextString(String newText) {
	// Clear the current String
	textString = "";
	
	// Manually copy the string over
	textString = newText;
	
	// Reset ALL the things!
	reset();	
}

// Copy newText into textString
void DC21_text::setTextString(char* newText) {
	// Clear the current String
	textString = "";
	
	// Manually copy the string over
	textString = newText;
	
	// Reset ALL the things!
	reset();
}


// Update the textBuffer, Then copy from textBuffer to Frame
// Needs to be called at least every [scrollRate] milliseconds.
void DC21_text::update() {
	unsigned int textLength = textString.length();
	unsigned int currentCharacter = 0;
	
	if(millis() >= (lastUpdate + scrollRate) && (timesToRepeat == 0 || timesToRepeat >= timesRepeated) && textLength > 0){
		lastUpdate = millis();
		
		int x;
		if(direction == 1){
			//Clear any extra shifted data from the mergeBuffer
			if(startX > 0){
				mergeBuffer[0] = mergeBuffer[0] & bit16_clear[15-startX];
				mergeBuffer[1] = mergeBuffer[1] & bit16_clear[15-startX];
				mergeBuffer[2] = mergeBuffer[2] & bit16_clear[15-startX];
				mergeBuffer[3] = mergeBuffer[3] & bit16_clear[15-startX];
				mergeBuffer[4] = mergeBuffer[4] & bit16_clear[15-startX];
				mergeBuffer[5] = mergeBuffer[5] & bit16_clear[15-startX];
				mergeBuffer[6] = mergeBuffer[6] & bit16_clear[15-startX];
				mergeBuffer[7] = mergeBuffer[7] & bit16_clear[15-startX];
				mergeBuffer[8] = mergeBuffer[8] & bit16_clear[15-startX];
				mergeBuffer[9] = mergeBuffer[9] & bit16_clear[15-startX];
				mergeBuffer[10] = mergeBuffer[10] & bit16_clear[15-startX];
				mergeBuffer[11] = mergeBuffer[11] & bit16_clear[15-startX];
				mergeBuffer[12] = mergeBuffer[12] & bit16_clear[15-startX];
				mergeBuffer[13] = mergeBuffer[13] & bit16_clear[15-startX];
				mergeBuffer[14] = mergeBuffer[14] & bit16_clear[15-startX];
				mergeBuffer[15] = mergeBuffer[15] & bit16_clear[15-startX];
			}

			//bit shift left
			mergeBuffer[0] = mergeBuffer[0] << 1;
			mergeBuffer[1] = mergeBuffer[1] << 1;
			mergeBuffer[2] = mergeBuffer[2] << 1;
			mergeBuffer[3] = mergeBuffer[3] << 1;
			mergeBuffer[4] = mergeBuffer[4] << 1;
			mergeBuffer[5] = mergeBuffer[5] << 1;
			mergeBuffer[6] = mergeBuffer[6] << 1;
			mergeBuffer[7] = mergeBuffer[7] << 1;
			mergeBuffer[8] = mergeBuffer[8] << 1;
			mergeBuffer[9] = mergeBuffer[9] << 1;
			mergeBuffer[10] = mergeBuffer[10] << 1;
			mergeBuffer[11] = mergeBuffer[11] << 1;
			mergeBuffer[12] = mergeBuffer[12] << 1;
			mergeBuffer[13] = mergeBuffer[13] << 1;
			mergeBuffer[14] = mergeBuffer[14] << 1;
			mergeBuffer[15] = mergeBuffer[15] << 1;
			
			x = endX;
		} else {
			//Clear any extra shifted data from the mergeBuffer
			if(endX < 15){
				mergeBuffer[0] = mergeBuffer[0] & bit16_clear[15-endX];
				mergeBuffer[1] = mergeBuffer[1] & bit16_clear[15-endX];
				mergeBuffer[2] = mergeBuffer[2] & bit16_clear[15-endX];
				mergeBuffer[3] = mergeBuffer[3] & bit16_clear[15-endX];
				mergeBuffer[4] = mergeBuffer[4] & bit16_clear[15-endX];
				mergeBuffer[5] = mergeBuffer[5] & bit16_clear[15-endX];
				mergeBuffer[6] = mergeBuffer[6] & bit16_clear[15-endX];
				mergeBuffer[7] = mergeBuffer[7] & bit16_clear[15-endX];
				mergeBuffer[8] = mergeBuffer[8] & bit16_clear[15-endX];
				mergeBuffer[9] = mergeBuffer[9] & bit16_clear[15-endX];
				mergeBuffer[10] = mergeBuffer[10] & bit16_clear[15-endX];
				mergeBuffer[11] = mergeBuffer[11] & bit16_clear[15-endX];
				mergeBuffer[12] = mergeBuffer[12] & bit16_clear[15-endX];
				mergeBuffer[13] = mergeBuffer[13] & bit16_clear[15-endX];
				mergeBuffer[14] = mergeBuffer[14] & bit16_clear[15-endX];
				mergeBuffer[15] = mergeBuffer[15] & bit16_clear[15-endX];
			}
			//bit shift right
			mergeBuffer[0] = mergeBuffer[0] >> 1;
			mergeBuffer[1] = mergeBuffer[1] >> 1;
			mergeBuffer[2] = mergeBuffer[2] >> 1;
			mergeBuffer[3] = mergeBuffer[3] >> 1;
			mergeBuffer[4] = mergeBuffer[4] >> 1;
			mergeBuffer[5] = mergeBuffer[5] >> 1;
			mergeBuffer[6] = mergeBuffer[6] >> 1;
			mergeBuffer[7] = mergeBuffer[7] >> 1;
			mergeBuffer[8] = mergeBuffer[8] >> 1;
			mergeBuffer[9] = mergeBuffer[9] >> 1;
			mergeBuffer[10] = mergeBuffer[10] >> 1;
			mergeBuffer[11] = mergeBuffer[11] >> 1;
			mergeBuffer[12] = mergeBuffer[12] >> 1;
			mergeBuffer[13] = mergeBuffer[13] >> 1;
			mergeBuffer[14] = mergeBuffer[14] >> 1;
			mergeBuffer[15] = mergeBuffer[15] >> 1;

			x = startX;
		}
		
		currentCharacter = (textString[currentOffset/6]-32);
		int temp = 7-(currentOffset%6);
		
		if(font[currentCharacter][0] & bit8_set[temp]) mergeBuffer[startY] = mergeBuffer[startY] | bit16_set[15-x];
		if(font[currentCharacter][1] & bit8_set[temp]) mergeBuffer[startY+1] = mergeBuffer[startY+1] | bit16_set[15-x];
		if(font[currentCharacter][2] & bit8_set[temp]) mergeBuffer[startY+2] = mergeBuffer[startY+2] | bit16_set[15-x];
		if(font[currentCharacter][3] & bit8_set[temp]) mergeBuffer[startY+3] = mergeBuffer[startY+3] | bit16_set[15-x];
		if(font[currentCharacter][4] & bit8_set[temp]) mergeBuffer[startY+4] = mergeBuffer[startY+4] | bit16_set[15-x];
		if(font[currentCharacter][5] & bit8_set[temp]) mergeBuffer[startY+5] = mergeBuffer[startY+5] | bit16_set[15-x];
		if(font[currentCharacter][6] & bit8_set[temp]) mergeBuffer[startY+6] = mergeBuffer[startY+6] | bit16_set[15-x];
		
		// Update the mergeBuffer
		if(bounce){
			// Bouncing Text
			if(direction == 1){
				if(currentOffset >= (textLength * 6) - 1){
					direction = 2;
					currentOffset = currentOffset - (endX - startX + 1);
				} else {
					currentOffset++;
				}
			} else {
				if(currentOffset <= 0){
					direction = 1;
					currentOffset = endX - startX;
				} else {
					currentOffset--;
				}
			}
		} else {
			// Looping Text
			if(direction == 1){
				currentOffset++;
				if( (currentOffset) >= (textLength * 6) ){
					currentOffset = 0;
				}
			} else {
				if( currentOffset == 0){
					currentOffset = (textLength * 6)-1;
				} else {
					currentOffset--;
				}
			}
		}
	
		// Clear old data out
		badge->frameBuffer[startY][0] = badge->frameBuffer[startY][0] & clearBuffer;
		badge->frameBuffer[startY+1][0] = badge->frameBuffer[startY+1][0] & clearBuffer;
		badge->frameBuffer[startY+2][0] = badge->frameBuffer[startY+2][0] & clearBuffer;
		badge->frameBuffer[startY+3][0] = badge->frameBuffer[startY+3][0] & clearBuffer;
		badge->frameBuffer[startY+4][0] = badge->frameBuffer[startY+4][0] & clearBuffer;
		badge->frameBuffer[startY+5][0] = badge->frameBuffer[startY+5][0] & clearBuffer;
		badge->frameBuffer[startY+6][0] = badge->frameBuffer[startY+6][0] & clearBuffer;
		
		badge->frameBuffer[startY][1] = badge->frameBuffer[startY][1] & clearBuffer;
		badge->frameBuffer[startY+1][1] = badge->frameBuffer[startY+1][1] & clearBuffer;
		badge->frameBuffer[startY+2][1] = badge->frameBuffer[startY+2][1] & clearBuffer;
		badge->frameBuffer[startY+3][1] = badge->frameBuffer[startY+3][1] & clearBuffer;
		badge->frameBuffer[startY+4][1] = badge->frameBuffer[startY+4][1] & clearBuffer;
		badge->frameBuffer[startY+5][1] = badge->frameBuffer[startY+5][1] & clearBuffer;
		badge->frameBuffer[startY+6][1] = badge->frameBuffer[startY+6][1] & clearBuffer;
		
		// Merge the mergeBuffer into the frameBuffer
		if(brightness == 3 || brightness == 1){
			badge->frameBuffer[startY][0] = badge->frameBuffer[startY][0] | mergeBuffer[startY];
			badge->frameBuffer[startY+1][0] = badge->frameBuffer[startY+1][0] | mergeBuffer[startY+1];
			badge->frameBuffer[startY+2][0] = badge->frameBuffer[startY+2][0] | mergeBuffer[startY+2];
			badge->frameBuffer[startY+3][0] = badge->frameBuffer[startY+3][0] | mergeBuffer[startY+3];
			badge->frameBuffer[startY+4][0] = badge->frameBuffer[startY+4][0] | mergeBuffer[startY+4];
			badge->frameBuffer[startY+5][0] = badge->frameBuffer[startY+5][0] | mergeBuffer[startY+5];
			badge->frameBuffer[startY+6][0] = badge->frameBuffer[startY+6][0] | mergeBuffer[startY+6];
		}
		if(brightness == 3 || brightness == 2){
			badge->frameBuffer[startY][1] = badge->frameBuffer[startY][1] | mergeBuffer[startY];
			badge->frameBuffer[startY+1][1] = badge->frameBuffer[startY+1][1] | mergeBuffer[startY+1];
			badge->frameBuffer[startY+2][1] = badge->frameBuffer[startY+2][1] | mergeBuffer[startY+2];
			badge->frameBuffer[startY+3][1] = badge->frameBuffer[startY+3][1] | mergeBuffer[startY+3];
			badge->frameBuffer[startY+4][1] = badge->frameBuffer[startY+4][1] | mergeBuffer[startY+4];
			badge->frameBuffer[startY+5][1] = badge->frameBuffer[startY+5][1] | mergeBuffer[startY+5];
			badge->frameBuffer[startY+6][1] = badge->frameBuffer[startY+6][1] | mergeBuffer[startY+6];
		}
	}
}


// Set the diminsions for the frameBuffer area that will be used for the text
void DC21_text::setDiminsions(unsigned int newStartX, unsigned int newStartY, unsigned int newEndX) {
	if(newStartX > 15) startX = 15;
	else startX = newStartX;

	if(newEndX > 15) endX = 15;
	else endX = newEndX;

	if(newStartY > 8) startY = 8;
	else startY = newStartY;
	
	if(endX < startX) endX = startX;

	// Reset ALL the things!
	reset();
}


// Set the scrolling rate (in milliseconds) (closer to 0 is faster)
void DC21_text::setScrollRate(unsigned int newScrollRate) {
	if(newScrollRate > 9001){
		scrollRate = 9001;
	} else {
		scrollRate = newScrollRate; // 35 is the theoretical minimum
	}
}


// Set the brightness of the text
// 1 = low, 2 = medium, 3 = high
void DC21_text::setBrightness(unsigned int newBrightness) {
	if(newBrightness > 3){
		brightness = 3;
	} else if(newBrightness < 1) {
		brightness = 1;
	} else {
		brightness = newBrightness;
	}
}


// Set the direction of scrolling
void DC21_text::setDirection(unsigned int newDirection){
	if(newDirection == 2) direction = 2;
	else direction = 1;
}


// Set whether text "Bounces" or "Loops" (set scrollRate to 0, or don't update for static text)
void DC21_text::setBounce(bool newBounce){
	bounce = newBounce;

	// Reset ALL the things!
	reset();
}


// Set the number of times to repeat bounce / loop
void DC21_text::setTimesToRepeat(unsigned int newTimesToRepeat) {
	if(newTimesToRepeat > 9001){
		timesToRepeat = 9001;
	} else {
		timesToRepeat = newTimesToRepeat;
	}

	// Reset ALL the things!
	reset();
}
