# `SUPERKEY`

**SuperKey** - the greatest CW keyer the world has ever seen.

This will be a README someday.

## Build Status

[![Build - AVR ATmega1284P (Debug)](https://github.com/xchrishawk/superkey/actions/workflows/build-atmega1284p-debug.yaml/badge.svg)](https://github.com/xchrishawk/superkey/actions/workflows/build-atmega1284p-debug.yaml)  
[![Build - AVR ATmega1284P (Release)](https://github.com/xchrishawk/superkey/actions/workflows/build-atmega1284p-release.yaml/badge.svg)](https://github.com/xchrishawk/superkey/actions/workflows/build-atmega1284p-release.yaml)

## Notes

### `avrdude` Upload Command

```
avrdude -c avrispmkii -P usb -p m1284p -U flash:w:superkey.ihex -D
```
