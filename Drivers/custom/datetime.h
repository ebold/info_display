/*
 * datetime.h
 *
 *  Created on: Jan 13, 2020
 *      Author: Enkhbold Ochirsuren
 */

#ifndef DATETIME_H_
#define DATETIME_H_

/* dimmer state */
#define DIMMER_ON	0x80 // dimmer is enabled
#define DIMMER_OFF	0x00 // dimmer is disabled
struct mydatetime {
	uint8_t msecond; // 1/10 second
	uint8_t second;  // second (0..59)
	uint8_t minute;  // minute (0..59)
	uint8_t hour;    // hour (0..23)
};
extern struct mydatetime s_mydatetime;

void tickDateTime(void);    // tick datetime

#endif /* DATETIME_H_ */
