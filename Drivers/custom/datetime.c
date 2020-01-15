/*
 * datetime.c
 *
 *  Created on: Jan 13, 2020
 *      Author: Enkhbold Ochirsuren
 */

#include "main.h"
#include "datetime.h"
#include "display.h"

/* datetime definitions */
#define DATETIME_HALF_SEC  5  // 0,5 s_mydatetime.second
#define DATETIME_SECOND    10 // 1 s_mydatetime.second
#define DATETIME_MINUTE    60 // 1 s_mydatetime.minute
#define DATETIME_HOUR      60 // 1 s_mydatetime.hour
#define DATETIME_DAY       24 // 1 day

struct mydatetime s_mydatetime;

/* Tick datetime
 * Must be called in period of 100ms */
void tickDateTime(void)
{
	s_mydatetime.msecond++;
	if (s_mydatetime.msecond == DATETIME_SECOND)
	{
		s_mydatetime.msecond = 0;
		s_mydatetime.second++;
		if (s_mydatetime.second == DATETIME_MINUTE)
		{
			event |= EVNT_DATETIME;
			s_mydatetime.second = 0;
			s_mydatetime.minute++;
			if (s_mydatetime.minute == DATETIME_HOUR)
			{
				event |= EVNT_DATETIME;
				s_mydatetime.minute = 0;
				s_mydatetime.hour++;
				if (s_mydatetime.hour == DATETIME_DAY)
				{
					s_mydatetime.hour = 0;
				}
			}
		}
		event |= EVNT_BLINK;
	}
	else if (s_mydatetime.msecond == DATETIME_HALF_SEC)
	{
		event |= EVNT_BLINK;
	}
}
