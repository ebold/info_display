#!/bin/env python

# Send/print the current system time in given format
# when next minute fires

import time
import serial
import datetime

DEVICE = '/dev/ttyACM0'  # serial device name
BAUDRATE = 115200        # default baudrate
DELAY = 3                # default polling interval
FORMAT = '%H:%M'         # time format in use
VERBOSE = False          # verbose flag (set to True to get output message)
DEBUG = False            # debug flag (set to True when serial device is not available)

# Return seconds remaining to next full minute
def seconds_left_to_minute():
    now = datetime.datetime.now()
    delta = datetime.timedelta(minutes=1)
    next_minute = (now + delta).replace(microsecond=0, second=0)
    seconds_left = (next_minute - now).seconds + 1
    if VERBOSE:
        print seconds_left, "seconds left until", next_minute.time().strftime(FORMAT)
    return seconds_left

with serial.Serial(DEVICE, BAUDRATE) as ser:
    # time at start
    now = datetime.datetime.now().time()
    last = now.strftime(FORMAT)
    print "Now:", now, "or", last
    if not DEBUG:
        ser.write(last)
    wait = seconds_left_to_minute()
    if not VERBOSE:
        print wait, "seconds left until next full minute"
    time.sleep(wait)

    # detect change in minute (at most in DELAY seconds)
    while True:
        wait = DELAY
        now = datetime.datetime.now().time()
        # send/print time only if next minute fires
        if (last != now.strftime(FORMAT)):
            last = now.strftime(FORMAT)
            if VERBOSE:
                print "Now:", now, "or", last
            if not DEBUG:
                ser.write(last)  # send time to serial device
            wait = seconds_left_to_minute()
        time.sleep(wait)
