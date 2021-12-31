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

# get CPU model
cpu_model=$(cat /proc/device-tree/model)

# cpu_model can have:
# TI AM335x BeagleBone Black
# Raspberry Pi Model B Rev 2

# get OS
os_pretty_name=$(grep PRETTY_NAME /etc/os-release)

# os_pretty_name can have:
# PRETTY_NAME="Arch Linux ARM"
# PRETTY_NAME="Raspbian GNU/Linux 10 (buster)"

# turn off all LEDs (it's enough to set trigger mode to none)
# range in {0..3} is supported in bash v3.0+ (echo $BASH_VERSION)
# in older versions of bash use seq command (ie., $(seq 0 3) instead of {0..3}

if [[ "${cpu_model,,}" =~ "beaglebone" ]]; then
  if [[ "${os_pretty_name,,}" =~ "arch" ]]; then
    # if cpu_model is 'beaglebone' and OS is 'archlinux', then turn off LEDs
    for i in {0..3}; do
      led_trigger_ctl $i "none"
    done
  fi
fi

# start the user python script
python2 $user_python_script

# remove a file
unlock
