# SuperKey Python Interfae

This README file will document SuperKey's Python interface (the `superkey` library). The intent of this library is to
provide a "reference implementation" for working with SuperKey's serial protocol, as well as to provide a way for users
to script SuperKey's functionality.

The root `scripts` directory also includes several handy stand-alone utility scripts:

- `autokey.py` allows controlling the keyer directly with your keyboard.
- `example.py` shows a demonstration of using the `superkey` library, as well as demonstrating some of SuperKey's
  hardware features.
- `interactive.py` creates an "interactive" environment for interfacing with SuperKey with a console-like REPL
  experience.

`superkey` has no dependencies other than the built-in Python standard library. A future improvement is to improve the
packaging for this library, to make it more consumable by other Python projects.

## AutoKeyer

The `autokey.py` script accepts user input from the keyboard, and immediately forwards it on to the keyer to be keyed.
Any key press which is not a valid Morse code character is ignored. The following special key presses are supported:

- **Backspace** - Immediately stops keying any string which was previously entered. This can be used to "cancel" input.
- **Escape** - Exits the tool.

The following command line arguments are supported:

- `--port` - Serial port name.
- `--baudrate` - Serial port baud rate. (This should always be 19200.)
- `--timeout` - Serial port timeout, in seconds.
- `--silent` - If specified, input will not be echoed back to the console.

## Interactive Python Environment

The `interactive.py` script can be used to quickly enter a REPL-like environment for interfacing with your SuperKey. It
supports the following command line arguments to set up the serial connection:

- `--port` - Serial port name.
- `--baudrate` - Serial port baud rate. (This should always be 19200.)
- `--timeout` - Serial port timeout, in seconds.

In most cases, the only required argument will be `--port`.

Once the interactive session has started, you can interact with your SuperKey by directly calling any public instance
function declared on the `Interface` class. No instance is required.

```
>>> set_buzzer_enabled(True)
>>> get_buzzer_frequency()
700
>>> set_buzzer_frequency(800)
>>> autokey('cq cq de n0vig n0vig k')
```

The `dir()` function may be used to get a list of available functions.
