#!/bin/bash

# Requires the 'lockfile-progs' package (to install it in ArchLinuxARM,
# invoke 'pacman -S lockfile-progs')
# Inspired by: https://gist.github.com/4b8a3fa98587c6e4eb4e46ebd800526c.git

# This script ensures that only single instance of serial communication
# with the attached STM32 Nucleo board is established via USB

# Usage: specify this script in an udev rule with attribute RUN+="serialtest.sh"
# Example: refer to https://github.com/ebold/info_display/python/49-stlinkv2-1.rules

lock_file='/tmp/nucleo.lock'
lock_retry=0

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

# start the serial communication with the STM32 Nucleo board via USB
python2 /home/ebold/info_display/python/serialtest.py

# remove a file
unlock
