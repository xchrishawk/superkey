# `SUPERKEY`

**SuperKey** - the greatest CW keyer the world has ever seen.

This will be a README someday.

## Notes

### `avrdude` Upload Command

```
avrdude -c avrispmkii -P usb -p m1284p -U flash:w:superkey.ihex -D
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
                "${workspaceFolder}/build/autogen/superkey",
                "${workspaceFolder}/lib",
                "${workspaceFolder}/src/main"
            ],
            "defines": [
                "MCU=\"atmega1284p\"",
                "BAUD=19200",
                "F_CPU=8000000",
                "_DEBUG=1",
                "_BUILD_TYPE=\"Debug\"",
                "_CONFIG_DFLT_WPM=200",
                "_CONFIG_DFLT_BUZZER_ENABLED=true",
                "_CONFIG_DFLT_BUZZER_FREQUENCY=700",
                "_CONFIG_DFLT_LED_STATUS_ENABLED=true",
                "_CONFIG_DFLT_LED_KEY_ENABLED=true",
                "_CONFIG_DFLT_INPUT_TYPE_TRS_0_TIP=INPUT_TYPE_STRAIGHT_KEY",
                "_CONFIG_DFLT_INPUT_POLARITY_TRS_0_TIP=INPUT_POLARITY_ACTIVE_LOW",
                "_CONFIG_DFLT_INPUT_TYPE_TRS_0_RING=INPUT_TYPE_NONE",
                "_CONFIG_DFLT_INPUT_POLARITY_TRS_0_RING=INPUT_POLARITY_ACTIVE_LOW",
                "_CONFIG_DFLT_INPUT_TYPE_TRS_1_TIP=INPUT_TYPE_PADDLE_LEFT",
                "_CONFIG_DFLT_INPUT_POLARITY_TRS_1_TIP=INPUT_POLARITY_ACTIVE_LOW",
                "_CONFIG_DFLT_INPUT_TYPE_TRS_1_RING=INPUT_TYPE_PADDLE_RIGHT",
                "_CONFIG_DFLT_INPUT_POLARITY_TRS_1_RING=INPUT_POLARITY_ACTIVE_LOW",
                "_CONFIG_DFLT_INPUT_TYPE_TRS_2_TIP=INPUT_TYPE_NONE",
                "_CONFIG_DFLT_INPUT_POLARITY_TRS_2_TIP=INPUT_POLARITY_ACTIVE_LOW",
                "_CONFIG_DFLT_INPUT_TYPE_TRS_2_RING=INPUT_TYPE_NONE",
                "_CONFIG_DFLT_INPUT_POLARITY_TRS_2_RING=INPUT_POLARITY_ACTIVE_LOW",
                "_CONFIG_DFLT_KEYER_INVERT_PADDLES=false"
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
