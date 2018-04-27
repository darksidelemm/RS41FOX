#!/bin/bash
# Basic Black Magic Probe GDB automation script, from https://github.com/blacksphere/blackmagic/wiki/GDB-Automation
#
# Change the /dev/cu.<blah> path below to your SWD debugger.
#
# Also assumes arm-none-eabi-gdb is in your $PATH

arm-none-eabi-gdb -nx --batch \
  -ex 'target extended-remote /dev/cu.usbmodem7BAD9DB1' \
  -ex 'monitor swdp_scan' \
  -ex 'attach 1' \
  -ex 'load' \
  -ex 'compare-sections' \
  -ex 'kill' \
  RS41HUP.elf