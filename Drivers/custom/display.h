/*
 * display.h
 *
 *  Created on: Jan 6, 2020
 *      Author: Enkhbold Ochirsuren
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#define MAX_CHAR      5  // number of characters on display
#define BYTE_PAT_COL  6  // number of columns per 8x6 byte-pattern

enum {
	DISP_MODE_STATIC = 0,
	DISP_MODE_BLINK,
	MAX_DISP_MODE
};

void initDisplay(void); // initialize display variables
int8_t displayText(uint8_t mode, uint8_t *text, uint8_t offset, uint8_t length); // display text

uint8_t *pPattern;     // pointer to the pattern-byte buffer

#endif /* DISPLAY_H_ */
