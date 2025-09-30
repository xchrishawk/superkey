# SuperKey

**SuperKey** - the greatest CW keyer the world has ever seen. (Someday, hopefully).

SuperKey is a fully featured, open source CW / Morse code keyer for amateur radio operators. The goal of SuperKey is to
provide **all keyer functionality** required by the modern ham radio enthusiast, as simply and cheaply as possible.
SuperKey is **100% open source**, and always will be - you are free to build, modify, and tinker with your own SuperKey.

![Prototype](https://github.com/user-attachments/assets/562650b9-a3b6-4f34-8f54-a3d0bb142bc6) ![Python Interface](https://github.com/user-attachments/assets/ab01b046-c7d4-4f6a-a0b4-01f7c1304780)

## Features

SuperKey provides many useful features for hams:

- Supports **all types of CW keys** — straight key, paddles, bugs, sideswipers, etc.

- Supports **all common paddle modes** — iambic, ultimatic, etc.

- Supports **all types of radios** — if it has a keyer input, you can connect a SuperKey to it.

- Connects **multiple keys to a single radio** — no need to choose between straight key and paddles!

- Connects **multiple radios to a single key** — if that's something you need to do (🤨), SuperKey can do it for you.

- Allows **keyboard text entry and automatic Morse code generation** with autokeyer support.

- Supports speeds from **1 WPM to 100 WPM**, with **exact timing control** of all Morse code elements.

- Keyer output is **millisecond accurate**, verified by oscilloscope.

- Hardware **LED and buzzer output** — practice when you're away from your radio, even silently!

- Supports a **robust serial interface**, allowing tight integration with third-party software.

- All functionality is **programmable via a Python library**, allowing advanced use cases.

## Hardware

SuperKey is under active development as of August 2025. The prototype hardware is continuing to evolve, and new features
are being added continuously. Once a basic design has been finalized, I will upload a bill of materials and CAD files
for the PCB. The current plan is for SuperKey to be equipped with at least the following hardware:

- [Microchip ATmega1284P](https://www.microchip.com/en-us/product/ATmega1284p) microcontroller
- [FTDI FT232](https://www.digikey.com/en/resources/datasheets/ftdi/ft232r) USB-to-TTL adapter
- Four 1/8" TRS sockets, for a total of eight fully configurable I/O pins.
- USB-Mini connector providing power and a serial connection to the PC.
- Auxiliary UART for debugging.
- Two LEDs (unit status and keyer output).
- One piezo buzzer with configurable audio frequency.

Power is provided via the USB connector. I also plan to add a hardware user interface of some sort, to allow performing
basic functions without requiring a PC connection. The details for this are TBD.

![Hardware Layout](https://github.com/user-attachments/assets/48e51617-bd74-42ce-a9fa-693ca33a8610)

## Documentation

⚠️ _Documentation is incomplete and under construction!_

All documentation for SuperKey is located in the project's [GitHub wiki](https://github.com/xchrishawk/superkey/wiki).

- [User Guide](https://github.com/xchrishawk/superkey/wiki/User-Guide) - **NEW USERS START HERE!** User-focused
  instruction manual for the SuperKey.
- [Developer Guide](https://github.com/xchrishawk/superkey/wiki/Developer-Guide) - Documentation for developers who want
  to modify the SuperKey software.
- [Hardware Guide](https://github.com/xchrishawk/superkey/wiki/Hardware-Guide) - Description of SuperKey hardware and
  how to build it.

## Build Status

[![Build - AVR ATmega1284P (Debug)](https://github.com/xchrishawk/superkey/actions/workflows/build-atmega1284p-debug.yaml/badge.svg)](https://github.com/xchrishawk/superkey/actions/workflows/build-atmega1284p-debug.yaml)<br/>
[![Build - AVR ATmega1284P (Release)](https://github.com/xchrishawk/superkey/actions/workflows/build-atmega1284p-release.yaml/badge.svg)](https://github.com/xchrishawk/superkey/actions/workflows/build-atmega1284p-release.yaml)

The linked GitHub workflows may be used as a reference example of setting up a SuperKey development environment.

## Credits

SuperKey is designed and maintained by [Chris Vig](mailto:chris@invictus.so) ([`N0VIG`](https://www.qrz.com/db/N0VIG)).

## Copyright / License

SuperKey is licensed under the GPLv3 license. See the [LICENSE](LICENSE) file for details.

![License: GPLv3](https://img.shields.io/badge/License-GPLv3-blue.svg)

```
Copyright (C) 2025 Chris Vig (chris@invictus.so).

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
```
