# RS41-FOX - RS41 Amateur Radio Direction Finding (Foxhunting) Beacon
This codebase turns a Vaisala RS41 into a 70cm 'radio fox', suitable for use in amateur radio direction finding contests.

This is a fork of https://github.com/darksidelemm/RS41HUP, which is itself a fork of https://github.com/df8oe/RS41HUP.

Features:
* Morse Ident containing callsign & current battery voltage.
* Long CW beacon (user-defined length and repeats)
* Low-Voltage Cutout, to avoid destroying rechargable batteries.
* TODO: Put GPS into disable mode once lock is acquired. 
* TODO: Beacon GPS position when battery gets low.

# Configuration
Configuration settings are located in [config.h](./config.h). Modify as appropriate before compiling/programming.

# Compilation
(Currently only supporting compilation with GCC under Linux/OSX)

* Grab the latest GNU ARM Embedded toolchain from here: https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
* Extract the tarball to somewhere useful. In my case I'm using ~/opt/
* Within the RS41HUP directory:
  * Edit CMakeLists.txt and set the correct path to the un-tar'd directory.
  * `cmake .`
  * `make`

# Programming
Either:
* Use the ST Micro ST-LINK utility (Windows only), or
* [stlink](https://github.com/texane/stlink) under Linux/OSX (though I haven't had much success with this yet...), or
* Use `flash.sh` with a [Black Magic Probe](https://1bitsquared.com/products/black-magic-probe). You will need to modify the path to the debugger's serial interface. I haven't been able to do the initial erase/program via this method.

Refer to [this file](./docs/programming_header.md) for programming header pinouts.
