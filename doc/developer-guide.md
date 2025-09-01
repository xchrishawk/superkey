# SuperKey Developer's Guide

This file will contain developer-focused documentation for SuperKey's firmware.

## Pre-Requisites

The following tools and libraries are required in order to build firmware images for SuperKey:

- [CMake](https://cmake.org/)
- [avr-gcc](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio/gcc-compilers)
- [avr-libc](https://avrdudes.github.io/avr-libc/)
- [Doxygen](https://www.doxygen.nl/) (for documentation only)
- [AVRDUDE](https://avrdude.nongnu.org/) (for programming device only)

No third-party libraries are required, other than `avr-libc`'s C standard library.

## Quick Start

I recommend always using a Linux environment for developing SuperKey's software. I personally use WSL on Windows, with
VS Code as my primary editor / IDE.

Exact installation steps will vary depending on the Linux distribution used. On Ubuntu-based platforms (including WSL),
the following commands will install all required tools. (Note that the compiler package is confusingly named `gcc-avr`,
not `avr-gcc`).

```
sudo apt-get update
sudo apt-get install -y avr-libc binutils-avr cmake gcc-avr make
# for documentation only:
sudo apt-get install -y doxygen graphviz
# for programming device only:
sudo apt-get install -y avrdude
```

The build environment is configured and set up using CMake:

```
cd superkey
cmake -S . -B ./build/
```

You can then build the entire project using `make`:

```
cd build
make
```

## Firmware Upload

The executable is uploaded to the MCU using [`avrdude`](https://www.nongnu.org/avrdude/) and an
[AVRISP mkII programmer](https://a.co/d/59zSwb5). The AVRISP connects to the microcontroller via its SPI interface. This
requires connecting 5 wires to the device PCB, as shown in the list below. Headers are provided on the PCB to support
this.

- ISP ground to device ground
- ISP `RESET` to device `RESET` (pin 9)
- ISP `SCK` to device `SCK` (pin 8)
- ISP `MOSI` to device `MISO` (pin 7)
- ISP `MISO` to device `MOSI` (pin 6)

The `avrdude` command to upload the firmware is:

```
avrdude -c avrispmkii -P usb -p m1284p -U flash:w:superkey.ihex -D -V
```

- The `-c` and `-P` arguments may need to be updated if you're using a programmer other than the AVRISP mkII.
- The `-D` argument is optional, and disables the Flash erase cycle.
- The `-V` argument is optional, and disables the post-upload verification.

A future improvement is to support firmware uploads via the serial interface.
