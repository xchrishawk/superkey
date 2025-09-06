#
# @file     scripts/superkey/constants.py
# @brief    Defines special constants used by the SuperKey interface.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-09-06
# @cpyrt    © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
#

# ------------------------------------------------------ EXPORTS -------------------------------------------------------

__all__ = [
    'SUPERKEY_DEFAULT_PORT',
    'SUPERKEY_DEFAULT_BAUDRATE',
    'SUPERKEY_DEFAULT_TIMEOUT',
]

# ----------------------------------------------------- CONSTANTS ------------------------------------------------------

# Default COM port for serial interface
SUPERKEY_DEFAULT_PORT = 'COM1'

# Default baud rate for serial interface
SUPERKEY_DEFAULT_BAUDRATE = 19200

# Default timeout for serial interface (seconds)
SUPERKEY_DEFAULT_TIMEOUT = 1.
