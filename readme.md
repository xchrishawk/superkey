# `EXAKEY`

This will be a readme someday.

## Notes

### `avrdude` Upload Command

```
avrdude -c avrispmkii -P usb -p m1284p -U flash:w:exakey.ihex -D
```

### VS Code Setup

The following may be added to the `.vscode/c_cpp_properties.json` file to properly configure the C / C++ environment for
the embedded AVR compiler:

```json
{
    "configurations": [
        {
            "name": "AVR (ATMega1284P)",
            "includePath": [
                "${workspaceFolder}/lib",
                "${workspaceFolder}/src/exakey"
            ],
            "defines": [
                "BAUD=19200",
                "F_CPU=8000000",
                "EXAKEY_DFLT_WPM=200",
                "EXAKEY_DFLT_BUZZER_ENABLED=true",
                "EXAKEY_DFLT_BUZZER_FREQUENCY=700",
                "EXAKEY_DFLT_LED_STATUS_ENABLED=true",
                "EXAKEY_DFLT_LED_STATUS_ACTIVE_LO=false",
                "EXAKEY_DFLT_LED_KEY_ENABLED=true",
                "EXAKEY_DFLT_LED_KEY_ACTIVE_LO=false"
            ],
            "compilerPath": "/usr/bin/avr-gcc",
            "compilerArgs": [
                "-mmcu=atmega1284p",
                "-Wall",
                "-Wextra",
                "-Wpedantic",
                "-Os"
            ],
            "cStandard": "c11",
            "cppStandard": "gnu++14",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ]
}
```
