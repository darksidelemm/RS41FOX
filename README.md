# RS41-FOX - RS41 Amateur Radio Direction Finding (Foxhunting) Beacon
This codebase turns a Vaisala RS41 into a 70cm 'radio fox', suitable for use in amateur radio direction finding contests.

This is a fork of https://github.com/darksidelemm/RS41HUP, which is itself a fork of https://github.com/df8oe/RS41HUP.

Features:
* Morse Ident containing callsign & current battery voltage.
* Long CW beacon (user-defined length and number of repeats)
* Low-Voltage Cutout, to avoid destroying rechargable batteries.
* Beacon GPS position when battery is below a user-defined threshold.

## Configuration
Configuration settings are located in [config.h](./config.h). Modify as appropriate before compiling/programming.

## Compilation
(Currently only supporting compilation with GCC under Linux/OSX)

* Grab the latest GNU ARM Embedded toolchain from here: https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
* Extract the tarball to somewhere useful. In my case I'm using ~/opt/
* Within the RS41HUP directory:
  * Edit CMakeLists.txt and set the correct path to the un-tar'd directory.
  * `cmake .`
  * `make`

## Programming
Either:
* Use the ST Micro ST-LINK utility (Windows only), or
* [stlink](https://github.com/texane/stlink) under Linux/OSX (though I haven't had much success with this yet...), or
* Use `flash.sh` with a [Black Magic Probe](https://1bitsquared.com/products/black-magic-probe). You will need to modify the path to the debugger's serial interface. I haven't been able to do the initial erase/program via this method.

Refer to [this file](./docs/programming_header.md) for programming header pinouts. Note that you will need to power up the RS41 using the power button before you will be able to flash the chip.

## Usage
* Program as above.
* Once batteries are inserted, press the power button to start transmissions.
* Remove batteries to stop transmissions.

# Power Consumption Notes
Power consumption of the various peripherals is approximately:
* uC + other quiescent power usage: ~100 mW
* Radio: ~170 mW (20mW TX Power)
* GPS (Acquisition/Tracking Mode): ~210 mW

In the case of a stationary radio fox, there isn't much point enabling the GPS after the initial position fix, so we turn it off. This almost doubles our battery life...

## Runtime Estimate
Achieved battery life will depend on your battery chemistry and transmit duty cycle. As a pessimistic estimate (assume 100% duty cycle, GPS disabled), we can state that the power consumption of the RS41 is 270mW.

Using 2x [AA Lithium cells](http://data.energizer.com/pdfs/l91.pdf) (~3Ah @ 3V = 9 Wh), we have a runtime of approximately 33 hours.
Using 2x [Alkaline AAs](http://data.energizer.com/pdfs/e91.pdf), (~2.5Ah @ 3V = 7.5 Wh), the expected runtime is 27 hours (if operating continuously), though note that the discharge curve of the batteries is quite different to the Lithiums, and the low-voltage thresholds will need to be adjusted appropriately.

### Runtime Actuals
* 2x Energizer Industrial Alkaline AA - On = 10 seconds, Off = 10 seconds, Low Voltage Cutout = 0.5V - Runtime: 36 hours.
* 2x Energizer Lithium AA - On = 10 seconds, Off = 10 seconds, Low Voltage Cutout = 0.5V - Runtime: ??
