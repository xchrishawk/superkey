# SuperKey Python Interfae

This README file will document SuperKey's Python interface (the `superkey` package). The intent of this library is to
provide a "reference implementation" for working with SuperKey's serial protocol, as well as to provide a way for users
to script SuperKey's functionality.

The root `scripts` directory also includes several handy stand-alone utility scripts:

- `example.py` shows a demonstration of using the `superkey` package, as well as demonstrating some of SuperKey's
  hardware features.
- `interactive.py` creates an "interactive" environment for interfacing with SuperKey with a console-like REPL
  experience.

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
