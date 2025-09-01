# SuperKey Developer's Guide

This file will contain developer-focused documentation for SuperKey's firmware.

## Pre-Requisites

The following tools and libraries are required in order to build firmware images for SuperKey:

- [CMake](https://cmake.org/)
- [avr-gcc](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio/gcc-compilers)
- [avr-libc](https://avrdudes.github.io/avr-libc/)
- [Doxygen](https://www.doxygen.nl/) (for documentation only)

No third-party libraries are required, other than `avr-libc`'s C standard library.

## Environment Quick Start

I recommend always using a Linux environment for developing SuperKey's software. I personally use WSL on Windows, with
VS Code as my primary editor / IDE.

Exact installation steps will vary depending on the Linux distribution used. On Ubuntu-based platforms (including WSL),
the following commands will install all required tools. (Note that the compiler package is confusingly named `gcc-avr`,
not `avr-gcc`).

```
sudo apt-get update
sudo apt-get install -y avr-libc binutils-avr cmake gcc-avr make
# for documentation only:
sudp apt-get install -y doxygen graphviz
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
