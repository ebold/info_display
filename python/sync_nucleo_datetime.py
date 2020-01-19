#!/bin/env python

# Send the system time to the Nucleo board every hour via USB.

# Usage: python sync_nucleo_datetime.py

# Both the Beaglebone Black and Nucleo boards have been connected via USB and
# data communication is done with the virtual comport (VCP) mode.

# But there is an issue regarding VCP in STLink (refer to [1], [2]).
# In order to deal with this issue each byte in a transmitted data has been
# sent with a few millisecond delay between each other.
# [1] https://community.st.com/s/question/0D50X00009XkhUU/stlink-vcp-with-flow-control
# [2] https://electronics.stackexchange.com/questions/288187/is-it-possible-to-get-st-links-virtual-com-port-working-without-launching-any-p

import time
import serial
import datetime

DEVICE = '/dev/ttyACM0'  # serial device name
BAUDRATE = 115200        # default baudrate
INTERVAL_1H = 3600       # 1-hour interval
DELAY_MS = 0.001         # 1ms delay
FORMAT = '%H:%M:%S'      # time format as HH:MM:SS (date +%T)
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

# send a string to serial device with delay
def send_with_delay(string):
    if not DEBUG and ser == None:
        return
    for s in string:
        if DEBUG:
            print s
        else:
            ser.write(s)
        time.sleep(DELAY_MS) # some ms delay

with serial.Serial(DEVICE, BAUDRATE) as ser:
    # initial transmission of the system time at start
    now = datetime.datetime.now().time()
    print "Now:", now
    datetime_nucleo = now.strftime(FORMAT)
    send_with_delay(datetime_nucleo)
    wait = seconds_left_to_minute()
    if not VERBOSE:
        print wait, "seconds left until next full minute"
    time.sleep(wait)

    # now send the system time every hour
    wait = INTERVAL_1H
    while True:
        datetime_nucleo = datetime.datetime.now().time().strftime(FORMAT)
        if VERBOSE:
            print "datetime to nucleo: ", datetime_nucleo
        send_with_delay(datetime_nucleo)
        time.sleep(wait)
