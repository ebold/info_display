#!/bin/bash

# This script starts an user python script that communicates with the STM32
# Nucleo board via USB and ensures that only single instance of it runs.

# Usage: specify this script in an udev rule with attribute RUN+="serialcomm_nucleo.sh"
# Example: refer to https://github.com/ebold/info_display/python/49-stlinkv2-1.rules

# Requires the 'lockfile-progs' package (invoke 'pacman -S lockfile-progs'
# to install it in ArchLinuxARM)
# Inspired by: https://gist.github.com/4b8a3fa98587c6e4eb4e46ebd800526c.git

lock_file='/tmp/nucleo'
lock_retry=0
user_python_script='/usr/local/bin/sync_nucleo_datetime.py'

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

# start the user python script
python2 $user_python_script

# remove a file
unlock
