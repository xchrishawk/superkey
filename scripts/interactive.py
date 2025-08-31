#
# @file     scripts/interactive.py
# @brief    Python file setting up an interactive session with the SuperKey.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-30
# @cpyrt    © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
#
# Running this Python module in an interactive interpreter creates a "terminal-like" environment for interacting with
# the SuperKey. This allows directly calling functions (like `set_buzzer_frequency(800)`) without having to explicitly
# create an interface instance.
#

# ------------------------------------------------------ IMPORTS -------------------------------------------------------

from superkey import Interface, InteractiveInterface

# ----------------------------------------------------- PROCEDURES -----------------------------------------------------

def parse_args():
    """
    Parses the command line arguments.
    """
    import argparse
    parser = argparse.ArgumentParser(description='SuperKey Interactive')
    parser.add_argument('--port', type=str, default='COM1', help='Serial port to connect to.')
    parser.add_argument('--baudrate', type=int, default=19200, help='Serial port baud rate.')
    parser.add_argument('--timeout', type=float, default=1., help='Serial port timeout (s).')
    return parser.parse_args()

# --------------------------------------------------- MAIN PROCEDURE ---------------------------------------------------

# Get arguments
args = parse_args()

# Build interactive interface
sk = InteractiveInterface(port=args.port, baudrate=args.baudrate, timeout=args.timeout)

# Inject all public callable methods into globals
for name in dir(sk._InteractiveInterface__intf):
    if not name.startswith('_'):
        attr = getattr(sk, name)
        if callable(attr):
            globals()[name] = attr

# Clean up namespace
del args, attr, name, parse_args, sk, Interface, InteractiveInterface

# Start a child Python REPL with the environment that we have oh-so-carefully curated
import code
l = dict(locals())
del l['code']
code.interact(local=l)
