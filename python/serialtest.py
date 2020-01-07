#!/bin/env python

# Send/print the current system time in given format when next minute fires.

# The UART transmission of datetime string to Nucleo is done with ms-delay
# between each byte in the string to overcome VCP issue in Stlink. [1][2]
# [1] https://community.st.com/s/question/0D50X00009XkhUU/stlink-vcp-with-flow-control
# [2] https://electronics.stackexchange.com/questions/288187/is-it-possible-to-get-st-links-virtual-com-port-working-without-launching-any-p

import time
import serial
import datetime

DEVICE = '/dev/ttyACM0'  # serial device name
BAUDRATE = 115200        # default baudrate
DELAY = 3                # default polling interval
DELAY_MS = 0.001         # 1ms delay
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
    # time at start
    now = datetime.datetime.now().time()
    last = now.strftime(FORMAT)
    print "Now:", now, "or", last
    send_with_delay(last)
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
            send_with_delay(last)  # send time to serial device
            wait = seconds_left_to_minute()
        time.sleep(wait)
