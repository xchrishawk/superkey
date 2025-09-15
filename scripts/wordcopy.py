#
# @file     scripts/wordcopy.py
# @brief    Trainer script for copying English words.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-09-10
# @cpyrt    © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
#

# ------------------------------------------------------ IMPORTS -------------------------------------------------------

import argparse
import time

from superkey import *
from utility import wordlist

# ----------------------------------------------------- CONSTANTS ------------------------------------------------------

DEFAULT_COUNT = 20
DEFAULT_DELAY = 3.
DEFAULT_WPM = 20.
DEFAULT_MINLEN = 2
DEFAULT_MAXLEN = 8

# ----------------------------------------------------- PROCEDURES -----------------------------------------------------

def _parse_args():
    """
    Parses the command line arguments.
    """
    parser = argparse.ArgumentParser(description='Word Copy Trainer')
    parser.add_argument('--port', type=str, default=SUPERKEY_DEFAULT_PORT, help='Serial port to connect to.')
    parser.add_argument('--baudrate', type=int, default=SUPERKEY_DEFAULT_BAUDRATE, help='Serial port baud rate.')
    parser.add_argument('--timeout', type=float, default=SUPERKEY_DEFAULT_TIMEOUT, help='Serial port timeout (s).')
    parser.add_argument('--count', type=int, default=DEFAULT_COUNT, help='Number of words to key.')
    parser.add_argument('--delay', type=float, default=DEFAULT_DELAY, help='Seconds to delay between each word.')
    parser.add_argument('--wpm', type=float, default=DEFAULT_WPM, help='Words per minute.')
    parser.add_argument('--minlen', type=int, default=DEFAULT_MINLEN, help='Minimum word length.')
    parser.add_argument('--maxlen', type=int, default=DEFAULT_MAXLEN, help='Maximum word length.')
    return parser.parse_args()

def _main(port: str,
          baudrate: int,
          timeout: float,
          count: int,
          delay: float,
          wpm: float,
          minlen: int,
          maxlen: int):
    """
    Runs the trainer.
    """
    # Build word list
    wl = wordlist.WordList()

    # Open SuperKey interface
    with Interface(port = port, baudrate = baudrate, timeout = timeout) as intf:

        try:

            # Get initial settings
            initial_wpm = intf.get_wpm()
            initial_trainer_mode = intf.get_trainer_mode()

            # Override settings
            intf.set_wpm(wpm)
            intf.set_trainer_mode(True)

            # Run as many times as commanded
            for _ in range(count):

                # Get a random word and key it
                word = wl.random(min_length=minlen, max_length=maxlen).upper()
                intf.autokey(word, block=True)

                # Print the word after a delay
                time.sleep(delay)
                print(word)
                time.sleep(delay * .5)

        except KeyboardInterrupt:

            # Cancel autokey
            intf.panic()

        finally:

            # Restore initial settings
            intf.set_wpm(initial_wpm)
            intf.set_trainer_mode(initial_trainer_mode)


# -------------------------------------------------- IMPORT PROCEDURE --------------------------------------------------

if __name__ == '__main__':

    # Parse arguments
    args = _parse_args()

    # Run main procedure
    _main(port = args.port,
          baudrate = args.baudrate,
          timeout = args.timeout,
          count = args.count,
          delay = args.delay,
          wpm = args.wpm,
          minlen = args.minlen,
          maxlen = args.maxlen)
