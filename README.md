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

### Building the firmware in a Linux environment

* Grab the latest GNU ARM Embedded toolchain from here: https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
* Extract the tarball to somewhere useful. In my case I'm using ~/opt/
* Within the RS41HUP directory:
  * Edit CMakeLists.txt and set the correct path to the un-tar'd directory.
  * Optionally enable a method of programming, in CMakeLists.txt.
  * `cmake .`
  * `make`

### Building the firmware with Docker

1. Install Docker if not already installed
2. Set the current directory to the RS41FOX source directory
3. Build the RS41fox compiler Docker image using the following command. It is necessary to build the Docker image only once.
    ```
    docker build -t rs41fox_compiler .
    ```
4. Build the firmware using the following command. If you need to rebuild the firmware, simply run the command again.
   On Linux/macOS, run:
    ```
    docker run --rm -it -v $(pwd):/usr/local/src/RS41FOX rs41fox_compiler
    ```
    On Windows CMD, run:
    ```
    docker run --rm -it -v %cd%:/usr/local/src/RS41FOX rs41fox_compiler
    ```
    On Windows PowerShell, run:
    ```
    docker run --rm -it -v ${pwd}:/usr/local/src/RS41FOX rs41fox_compiler
    ```
5. The firmware will be stored in file `build/RS41FOX.elf`

## Programming
Either:
* Use the ST Micro ST-LINK utility (Windows only), or
* [openocd](http://openocd.org) under Linux/OSX/Windows, or
* [stlink](https://github.com/texane/stlink) under Linux/OSX/Windows, or
* Use `flash.sh` with a [Black Magic Probe](https://1bitsquared.com/products/black-magic-probe). You will need to modify this script with the path to the debugger's serial interface.

Only the proprietary ST-LINK utility, or openocd, are known to support unlocking the device to remove the original manufacturer's firmware.

Refer to [this file](./docs/programming_header.md) for programming header pinouts. Note that you will need to power up the RS41 using the power button, before you will be able to interact with the MCU chip.

Please do not allow the RS41 to run the original manufacturer's firmware, as this will result in unlicensed radio transmissions. With the reset line connected to a debugger, it is possible to keep the MCU halted. However if the LED lights up after pressing the power button, then some firmware is definitely running - please erase the original firmware ASAP, or remove the batteries.

The build system supports a number of useful operations via openocd or stlink:
* `make erase` erases the flash, and if using openocd, unlocks a locked device
* `make halt` resets and halts the device
* `make run` resets the device and lets it run
* `make program-halt` programs the device and halts it
* `make program` programs the device and lets it run

Additionally `make program` is supported with Black Magic Probe. Currently `halt` and `program-halt` are not supported with stlink. The `program` targets will first compile the firmware, if it is not up to date.

A typical sequence for programming a freshly acquired RS41, would be:
* openocd: `make erase program-halt`
* stlink: Unlock the device using the proprietary (Windows) version of ST-LINK, and clear the flash page protection bits. Then program using either the proprietary ST-LINK, or the free replacement.

An already unlocked RS41 does not require `make erase` before programming. Always remember to press the power button after inserting batteries, or the MCU will not be powered for programming.

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
* 2x Energizer Industrial Alkaline AA - 20mW TX, On = 10 seconds, Off = 10 seconds, Low Voltage Cutout = 0.5V - Runtime: 36 hours.
* 2x Energizer Lithium AA - 20mW TX, On = 10 seconds, Off = 10 seconds, Low Voltage Cutout = 0.5V - Runtime: ??
