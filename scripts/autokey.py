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

from superkey import *

# ----------------------------------------------------- CONSTANTS ------------------------------------------------------

# Special ASCII codes
_ASCII_BACKSPACE = 0x08
_ASCII_NEWLINE = 0x0D
_ASCII_ESCAPE = 0x1B

# ----------------------------------------------------- PROCEDURES -----------------------------------------------------

def _parse_args():
    """
    Parses the command line arguments.
    """
    parser = argparse.ArgumentParser(description='SuperKey AutoKeyer')
    parser.add_argument('--port', type=str, default=SUPERKEY_DEFAULT_PORT, help='Serial port to connect to.')
    parser.add_argument('--baudrate', type=int, default=SUPERKEY_DEFAULT_BAUDRATE, help='Serial port baud rate.')
    parser.add_argument('--timeout', type=float, default=SUPERKEY_DEFAULT_TIMEOUT, help='Serial port timeout (s).')
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

            # Ignore unknown characters, but allow newlines to print
            should_autokey = _should_autokey(code)
            should_print = should_autokey or code == _ASCII_NEWLINE

            # Bail if there's nothing to do
            if not should_autokey and not should_print:
                continue

            # Convert character code to string, fixing newlines
            if code != _ASCII_NEWLINE:
                char = chr(code)
            else:
                char = '\r\n'

            # Send character to keyer
            if should_autokey:
                intf.autokey(char)
            if echo and should_print:
                print(char, end='', flush=True)

# --------------------------------------------------- MAIN PROCEDURE ---------------------------------------------------

if __name__ == '__main__':

    # Read arguments
    args = _parse_args()
    _immediate_mode(port = args.port,
                    baudrate = args.baudrate,
                    timeout = args.timeout,
                    echo = not args.silent,
                    print_help = not args.silent)
