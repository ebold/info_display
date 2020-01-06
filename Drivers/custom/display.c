/*
 * display.c
 *
 *  Created on: Jan 6, 2020
 *      Author: Enkhbold Ochirsuren
 */
#include "main.h"
#include "display.h"

extern const uint8_t charset6x8[96][BYTE_PAT_COL]; // 8x6 byte-pattern for ASCII characters
uint8_t bufDisplay[MAX_DISP_MODE][MAX_CHAR];       // display buffer for ASCII text
uint8_t bytePattern[MAX_DISP_MODE][MAX_CHAR * BYTE_PAT_COL]; // pattern buffer for 8x6 byte-patterns of text in display buffer
uint8_t next_char; // offset for pattern buffer

/* initialize display variables */
void initDisplay(void)
{
	pPattern = &bytePattern[0][0];
}
/* convert an ASCII character to an 8x6 byte-pattern */
int8_t charToPattern(uint8_t mode, uint8_t ascii_char)
{
	uint8_t i, j;
	if ((ascii_char < ' ') || (ascii_char > 0x7F))
	{
		return -1;
	}
	ascii_char -= ' ';
	j = next_char * BYTE_PAT_COL;
	for (i = 0; i < BYTE_PAT_COL; i++)
		bytePattern[mode][i+j] = charset6x8[ascii_char][i];
	return 0;
}
/* copy ASCII char to the byte-pattern buffer */
void writeChar(uint8_t mode, uint8_t *source)
{
	uint8_t i;
	for (i = 0; i < MAX_CHAR; i++)
	{
		charToPattern(mode, *(source + i));
		next_char++;
	}
	pPattern = &bytePattern[mode][0];
}

/* generate byte-pattern in the given buffer */
void generatePattern(uint8_t mode)
{
	next_char = 0;
	writeChar(mode, &bufDisplay[mode][0]);
}

/* display text */
int8_t displayText(uint8_t mode, uint8_t *text, uint8_t offset, uint8_t length)
{
	uint8_t i, len;
	if (offset + length > MAX_CHAR)
		return -1;
	if (mode > DISP_MODE_BLINK)
		return -2;
	len = 0;
	for (i = offset; len < length; i++, len++)
		bufDisplay[mode][i] = *(text + i);

	generatePattern(mode);

	return len;
}
