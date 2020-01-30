#!/bin/bash

# This script starts an user python script that communicates with the STM32
# Nucleo board via USB and ensures that only single instance of it runs.
# All on-board LEDs are also turned off.

# Usage: specify this script in an udev rule with attribute RUN+="serialcomm_nucleo.sh"
# Example: refer to https://github.com/ebold/info_display/python/49-stlinkv2-1.rules

# Requires the 'lockfile-progs' package (invoke 'pacman -S lockfile-progs'
# to install it in ArchLinuxARM)
# Inspired by: https://gist.github.com/4b8a3fa98587c6e4eb4e46ebd800526c.git

lock_file='/tmp/nucleo'
lock_retry=0
user_python_script='/usr/local/bin/sync_nucleo_datetime.py'
path_to_leds="/sys/class/leds/beaglebone:green:usr"

# turn off LEDs
led_trigger_ctl() {
  if [ "$1" == "" ] || [ $1 -gt 3 ] || [ $1 -lt 0 ]; then
    exit
  fi
  if [ "$2" == "none" ] || [ "$2" == "heartbeat" ] || [ "$2" == "mmc0" ] || \
    [ "$2" == "mmc1" ]; then
    echo "$2" > "${path_to_leds}$1/trigger"
  fi
}

# create and lock a file, return 0 if fails
lock() {
  lockfile-create -r $lock_retry -p $lock_file && return 0
  echo "Error: cannot lock $lock_file"
  exit $?
}

# remove the lock file
unlock() {
  lockfile-remove $lock_file
}

# lock a file
lock

# turn off all LEDs (it's enough to set trigger mode to none)
# range in {0..3} is supported in bash v3.0+ (echo $BASH_VERSION)
# in older versions of bash use seq command (ie., $(seq 0 3) instead of {0..3}
for i in {0..3}; do
  led_trigger_ctl $i "none"
done

# start the user python script
python2 $user_python_script

# remove a file
unlock
