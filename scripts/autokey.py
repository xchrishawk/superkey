#
# @file     scripts/autokey.py
# @brief    Python file accepting text input which is automatically keyed by the keyer.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-09-06
# @cpyrt    © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
#

# ------------------------------------------------------ IMPORTS -------------------------------------------------------

import argparse
from msvcrt import getch
import re

from superkey import *

# ----------------------------------------------------- CONSTANTS ------------------------------------------------------

# Special ASCII codes
_ASCII_BACKSPACE = 0x08
_ASCII_NEWLINE = 0x0D
_ASCII_ESCAPE = 0x1B
_ASCII_BACKSLASH = 0x5C

# ----------------------------------------------------- PROCEDURES -----------------------------------------------------

def _parse_args():
    """
    Parses the command line arguments.
    """
    parser = argparse.ArgumentParser(description='SuperKey AutoKeyer')
    parser.add_argument('--port', type=str, default=SUPERKEY_DEFAULT_PORT, help='Serial port to connect to.')
    parser.add_argument('--baudrate', type=int, default=SUPERKEY_DEFAULT_BAUDRATE, help='Serial port baud rate.')
    parser.add_argument('--timeout', type=float, default=SUPERKEY_DEFAULT_TIMEOUT, help='Serial port timeout (s).')
    parser.add_argument('--immediate', action=argparse.BooleanOptionalAction, help='Use immediate mode?')
    parser.add_argument('--silent', action=argparse.BooleanOptionalAction, help='Disable output to console?')
    return parser.parse_args()


def _should_autokey(code: int) -> bool:
    """
    Returns `True` if the character with the specified ASCII code is a valid Morse code character.
    """
    return (code == 0x20 or                       # space
            code == 0x21 or                       # exclamation mark
            code == 0x22 or                       # double quote
            code == 0x27 or                       # single quote
            code == 0x2B or                       # plus
            code == 0x2C or                       # comma
            code == 0x2D or                       # dash
            code == 0x2E or                       # period
            code == 0x2F or                       # slash
            (code >= 0x30 and code <= 0x39) or    # digit
            code == 0x3D or                       # equals
            code == 0x3F or                       # question mark
            (code >= 0x41 and code <= 0x5A) or    # upper case letter
            code == 0x5F or                       # underscore
            (code >= 0x61 and code <= 0x7A))      # lower case letter


def _buffered_mode(port: str = SUPERKEY_DEFAULT_PORT,
                   baudrate: int = SUPERKEY_DEFAULT_BAUDRATE,
                   timeout: float = SUPERKEY_DEFAULT_TIMEOUT):
    """
    Runs the autokeyer in buffered mode.
    """
    with Interface(port=port, baudrate=baudrate, timeout=timeout) as intf:
        while True:
            try:

                # Get next input from user
                line = input('> ')

                # Helper functions
                def line_equals(x: str) -> bool:
                    return line.casefold() == x.casefold()
                def line_starts_with(x: str) -> bool:
                    return line.casefold().startswith(x.casefold())

                # Check for special commands
                if line_equals(':q') or line_equals(':quit') or line_equals(':exit'):
                    # Exit program
                    break

                elif line_equals(':!') or line_equals(':panic'):
                    # Panic
                    intf.panic()
                    continue

                elif line_equals(':wpm'):
                    # Print WPM status
                    print(f'WPM: {intf.get_wpm():.1f}')
                    continue

                elif line_starts_with(':wpm '):
                    # Set WPM
                    try:
                        intf.set_wpm(float(line[5:]))
                    except ValueError:
                        print('Invalid WPM?')
                    continue

                elif line_equals(':buzzer'):
                    # Print buzzer status
                    print(f'Buzzer: {'On' if intf.get_buzzer_enabled() else 'Off'} ({intf.get_buzzer_frequency()} Hz)')
                    continue

                elif line_equals(':buzzer off'):
                    # Turn buzzer off
                    intf.set_buzzer_enabled(False)
                    continue

                elif line_equals(':buzzer on'):
                    # Turn buzzer on
                    intf.set_buzzer_enabled(True)
                    continue

                elif line_starts_with(':buzzer frequency '):
                    # Set buzzer frequency
                    try:
                        intf.set_buzzer_frequency(int(line[18:]))
                    except ValueError:
                        print('Invalid frequency?')
                    continue

                elif line_equals(':paddle'):
                    # Print paddle mode
                    mode = intf.get_paddle_mode()
                    if mode == PaddleMode.IAMBIC:
                        print('Paddle mode: Iambic')
                    elif mode == PaddleMode.ULTIMATIC:
                        print('Paddle mode: Ultimatic')
                    elif mode == PaddleMode.ULTIMATIC_ALTERNATE:
                        print('Paddle mode: Ultimatic Alternate')
                    else:
                        print('Paddle mode: unknown?')
                    continue

                elif line_equals(':paddle iambic'):
                    # Set paddles to iambic mode
                    intf.set_paddle_mode(PaddleMode.IAMBIC)
                    continue

                elif line_equals(':paddle ultimatic'):
                    # Set paddles to ultimatic mode
                    intf.set_paddle_mode(PaddleMode.ULTIMATIC)
                    continue

                elif line_equals(':paddle ultimatic_alternate'):
                    # Set paddles to ultimatic alternate mode
                    intf.set_paddle_mode(PaddleMode.ULTIMATIC_ALTERNATE)
                    continue

                elif line_equals(':trainer'):
                    # Print trainer mode status
                    print(f'Trainer mode: {'On' if intf.get_trainer_mode() else 'Off'}')
                    continue

                elif line_equals(':trainer on'):
                    # Enable trainer mode
                    intf.set_trainer_mode(True)
                    continue

                elif line_equals(':trainer off'):
                    # Disable trainer mode
                    intf.set_trainer_mode(False)
                    continue

                elif line_starts_with(':qm'):
                    # Handle this with regex for easier processing
                    if match := re.match(r'^:qm get (\d+)$', line, re.IGNORECASE):
                        print(intf.get_quick_msg(int(match.group(1))))
                        continue
                    elif match := re.match(r'^:qm set (\d+) (.+)$', line, re.IGNORECASE):
                        intf.set_quick_msg(int(match.group(1)), match.group(2))
                        continue
                    elif match := re.match(r'^:qm del (\d+)$', line, re.IGNORECASE):
                        intf.invalidate_quick_msg(int(match.group(1)))
                        continue
                    elif match := re.match(r'^:qm (\d+)$', line, re.IGNORECASE):
                        intf.autokey_quick_msg(int(match.group(1)))
                        continue

                elif line_equals(':version'):
                    # Print version information
                    print(intf.version())
                    continue

                elif line_starts_with(':'):
                    # Unknown command?
                    print('Unknown command?')
                    continue

                # Split line into tokens and send in either normal or prosign mode
                tokens = line.split('\\')
                prosign = False
                for token in tokens:
                    if prosign and len(token) > 1:
                        intf.autokey(token[:-1], flags=[AutokeyFlag.NO_LETTER_SPACE])
                        intf.autokey(token[-1])
                    elif len(token) != 0:
                        intf.autokey(token)
                    prosign = not prosign

            # Ultra-graceful error handling
            except InvalidMessageError:
                print('SuperKey responds: invalid message!')
            except InvalidSizeError:
                print('SuperKey responds: invalid size!')
            except InvalidCRCError:
                print('SuperKey responds: invalid CRC!')
            except InvalidPayloadError:
                print('SuperKey responds: invalid payload!')
            except InvalidValueError:
                print('SuperKey responds: invalid value!')
            except InterfaceError:
                print('SuperKey responds: unknown error!')


def _immediate_mode(port: str = SUPERKEY_DEFAULT_PORT,
                    baudrate: int = SUPERKEY_DEFAULT_BAUDRATE,
                    timeout: float = SUPERKEY_DEFAULT_TIMEOUT,
                    echo: bool = True,
                    print_help: bool = False):
    """
    Runs the autokeyer in immediate mode. Any key the user presses is immediately sent to the keyer.
    """
    # Print help string
    if print_help:
        print('\n[SuperKey AutoKeyer] Immediate Mode')
        print('(Key presses are immediately sent to the keyer - press BACKSPACE to cancel input, ESCAPE to quit)')
        print('Listening...\n')

    # Open interface
    with Interface(port=port, baudrate=baudrate, timeout=timeout) as intf:

        # Tracking variables
        prosign_active = False
        prosign_string = ''

        # Loop until commanded to quit
        while True:

            # Get byte for user keypresse
            inpt = getch()
            assert(len(inpt) == 1)
            code = inpt[0]

            # Exit if user pressed escape key
            if code == _ASCII_ESCAPE:
                break

            # Panic if the user pressed backspace key
            if code == _ASCII_BACKSPACE:
                intf.panic()
                continue

            # Start a prosign if the user pressed backslash key
            if code == _ASCII_BACKSLASH:
                print(chr(_ASCII_BACKSLASH), end='', flush=True)
                prosign_active = not prosign_active
                if prosign_active:
                    prosign_string = ''
                    continue

            # Ignore unknown characters, but allow newlines to print
            should_autokey = _should_autokey(code)
            should_print = should_autokey or code == _ASCII_NEWLINE

            # Convert character code to string, fixing newlines
            if code != _ASCII_NEWLINE:
                char = chr(code)
            else:
                char = '\r\n'

            # Handle prosigns
            if prosign_active and should_autokey:
                prosign_string += char
            elif not prosign_active and len(prosign_string) != 0:
                assert code == _ASCII_BACKSLASH, 'Logic error!'
                if len(prosign_string) > 1:
                    intf.autokey(prosign_string[:-1], [AutokeyFlag.NO_LETTER_SPACE])
                    intf.autokey(prosign_string[-1])
                else:
                    intf.autokey(prosign_string)
                prosign_string = ''

            # Send character to keyer
            elif should_autokey:
                intf.autokey(char)

            # Send character to console
            if echo and should_print:
                print(char, end='', flush=True)

# --------------------------------------------------- MAIN PROCEDURE ---------------------------------------------------

if __name__ == '__main__':

    # Read arguments
    args = _parse_args()
    if args.immediate:
        _immediate_mode(port = args.port,
                        baudrate = args.baudrate,
                        timeout = args.timeout,
                        echo = not args.silent,
                        print_help = not args.silent)
    else:
        _buffered_mode(port=args.port,
                       baudrate=args.baudrate,
                       timeout=args.timeout)
