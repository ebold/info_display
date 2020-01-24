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

/* time period for dimmer activation */
#define DATETIME_DIMMER_START	21 // activate dimmer at 21:00
#define DATETIME_DIMMER_END     6  // deactivate dimmer at 6:00

uint8_t dimmer = DIMMER_OFF; // dimmer deactivated
struct mydatetime s_mydatetime;
uint32_t stateTimeout = 0;   // timeout counter used by FSM to set datetime

// states of FSM to set datetime
#define DATETIME_SET_IDLE    0  // idle state
#define DATETIME_SET_START   1  // start to set datetime
#define DATETIME_SET_HOUR    2  // set hour
#define DATETIME_SET_MINUTE  3  // set minute
#define DATETIME_SET_END     4  // end of setting datetime

uint8_t curSetState = DATETIME_SET_IDLE;
uint8_t nextSetState = DATETIME_SET_IDLE;
button_t btnPrvState = BTN_RELEASED;

/* Update the dimmer flag */
void updateDimmer(void)
{
	if ((DATETIME_DIMMER_START <= s_mydatetime.hour) || (s_mydatetime.hour <= DATETIME_DIMMER_END))
	{
		dimmer = DIMMER_ON;
	}
	else
	{
		dimmer = DIMMER_OFF;
	}
}
/* Tick datetime
 * Must be called in period of 100ms */
void tickDateTime(void)
{
	if (isSetTimeActive()) // do no tick datetime if setting it is in progress
		return;

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
				updateDimmer();
			}
		}
		event |= EVNT_BLINK;
	}
	else if (s_mydatetime.msecond == DATETIME_HALF_SEC)
	{
		event |= EVNT_BLINK;
	}
}
/**
 * @brief	Adjust minute manually
 * @param	None
 * @retval	None
 */
void adjustMinute(void)
{
	s_mydatetime.second = 0;
	s_mydatetime.minute++;
	if (s_mydatetime.minute == DATETIME_HOUR)
	{
		s_mydatetime.minute = 0;
	}
	event |= EVNT_DATETIME;
}
/**
 * @brief	Adjust hour manually
 * @param	None
 * @retval	None
 */
void adjustHour(void)
{
	s_mydatetime.hour++;
	if (s_mydatetime.hour == DATETIME_DAY)
	{
		s_mydatetime.hour = 0;
	}
	updateDimmer();
	event |= EVNT_DATETIME;
}

/**
 *  @brief	Set datetime
 *  @param	button	user button to trigger FSM
 *  @param	*buf	display buffer
 *  @retval	None
 */
void setTime(uint8_t button, uint8_t *buf)
{
	/**
	 * States for setting datetime: idle, start (s), hour (h), minute (m), end (e)
	 * Transition between states is triggered by the button status:
	 * 1. idle -> start: button is longer pressed (> 3 sec)
	 * 2.1. start -> hour: button pressed again, now user can set hour by hitting button
	 * 2.2. start -> end: button is longer inactive (> 3 sec)
	 * 3. hour -> minute: button is longer inactive (> 3 sec), now user can set minute
	 * 4. minute -> end: button is longer inactive (> 3 sec)
	 * 5.1. end -> idle: button is longer inactive (> 5 sec)
	 * 5.2. end -> start: button is hit again within 3 sec
	 */
	stateTimeout++;

	switch (curSetState)
	{
	case DATETIME_SET_IDLE:
		if (button == BTN_PRESSED)
		{
			if (stateTimeout > PERIOD_WAIT_LONG) // if button is pressed longer than threshold, then switch to next state
			{
				nextSetState = DATETIME_SET_START;
				*(buf + 2) = 's';

				event |= EVNT_DATETIME; // display new state transition displayText(DISP_MODE_STATIC, buf, 2, 1); // display "XXsYY"
			}
		}
		else
			stateTimeout = 0;  // reset counter here because of no state transition

		break;

	case DATETIME_SET_START:
	{
		if ((button == BTN_PRESSED) && (button ^ btnPrvState))	// if button is toggled, then switch to next state
		{
			nextSetState = DATETIME_SET_HOUR;
			*(buf + 2) = 'h';
			event |= EVNT_DATETIME; // display new state transition, "XXhYY"
		}
		else if (stateTimeout > PERIOD_WAIT_SHORT)	// if button was idle longer than threshold, then switch to last state
		{
			nextSetState = DATETIME_SET_END;
		}
	}
	break;

	case DATETIME_SET_HOUR:
	{
		if ((button == BTN_PRESSED) && (button ^ btnPrvState)) // if button is toggled, then switch adjust hour
		{
			stateTimeout = 0;  // reset counter here because of no state transition
			adjustHour();      // adjust hour
		}
		else if (stateTimeout > PERIOD_WAIT_SHORT)	// if button was idle longer than threshold, then switch to next state
		{
			nextSetState = DATETIME_SET_MINUTE;
			*(buf + 2) = 'm';
			event |= EVNT_DATETIME; // display new state transition, "XXmYY"
		}
	}
	break;

	case DATETIME_SET_MINUTE:
	{
		if ((button == BTN_PRESSED) && (button ^ btnPrvState)) // if button is toggled, then adjust minute
		{
			stateTimeout = 0;  // reset counter here because of no state transition
			adjustMinute();    // adjust minute
		}
		if (stateTimeout > PERIOD_WAIT_SHORT)	// if button was idle longer than threshold, then switch to the last state
		{
			nextSetState = DATETIME_SET_END;
			*(buf + 2) = 'e';
			event |= EVNT_DATETIME; // display new state transition, "XXeYY"
		}
	}
	break;

	case DATETIME_SET_END:
	{
		if ((button == BTN_PRESSED) && (button ^ btnPrvState)) // if button is toggled, then switch to the start state
		{
			nextSetState = DATETIME_SET_START;
			*(buf + 2) = 's';
			event |= EVNT_DATETIME; // display new state transition, "XXsYY"
		}
		else if (stateTimeout > PERIOD_WAIT_LONG)	// if button was idle longer than threshold, then switch to the idle state
		{
			nextSetState = DATETIME_SET_IDLE;
		}
	}
	break;

	default:
		break;

	}

	if (curSetState ^ nextSetState)  // state transition
	{
		curSetState = nextSetState;
		stateTimeout = 0;
	}

	btnPrvState = button;  // notice button state
}

/**
 * @brief	Return the current state of datetime setting
 * @param	None
 * @retval	Non-zero value indicates that datetime setting is in progress
 */
uint8_t isSetTimeActive()
{
	return curSetState;
}
