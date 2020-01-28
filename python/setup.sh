#!/bin/bash

# Set up the serial communication between the BeagleBone Black board (running
# ArchLinuxARM) and the STM32 Nucleo board (with STLink embedded) via USB.
# The Nucleo board is powered only by USB.

cur_user=$(whoami)

if [ $cur_user != 'root' ]; then
  echo "Run it under root administration!"
  exit 1
fi

udev_rule_file='49-stlinkv2-1.rules'         # udev rule with signal to systemd on adding ttyACM0 device
udev_rule_dir='/etc/udev/rules.d/'           # standard location of the udev rules
nucleo_service_file='nucleo.service'         # user service (controlled by systemd)
systemd_services_dir='/etc/systemd/system/'  # standard location of the user service file
user_script_dir='/usr/local/bin/'            # standard location of the user scripts
user_shell_script='serialcomm_nucleo.sh'     # user shell script that starts the serial communication to Nucleo
user_python_script='sync_nucleo_datetime.py' # user python script that synchronize datetime in Nucleo

echo "Set up an environment needed to communicate with the Nucleo board via USB."

if [ ! -e $udev_rule_file ]; then
  echo "Error: cannot find $udev_rule_file"
  exit 1
fi

if [ ! -e $nucleo_service_file ]; then
  echo "Error: cannot find $nucleo_service_file"
  exit 1
fi

cp $udev_rule_file $udev_rule_dir
echo "Copied $udev_rule_file into $udev_rule_dir"

cp $nucleo_service_file $systemd_services_dir
echo "Copied $nucleo_service_file into $systemd_services_dir"

cp $user_shell_script $user_python_script $user_script_dir
echo "Copied $user_shell_script $user_python_script into $user_script_dir"

echo "Ready to communication with the Nucleo board!"

udevadm control --reload-rules
echo "udev rules were reloaded."
echo "Please verify that if STLink can be detected by st-util."
echo "Otherwise, please reboot to apply system changes!"
