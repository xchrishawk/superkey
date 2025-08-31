#
# @file     scripts/superkey/superkey_types.py
# @brief    Module defining common types for the SuperKey interface.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-30
# @cpyrt    © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
#

# ------------------------------------------------------ IMPORTS -------------------------------------------------------

from enum import IntEnum
import struct

# ------------------------------------------------------ EXPORTS -------------------------------------------------------

__all__ = [
    'HEADER_STRUCT_FORMAT',
    'HEADER_STRUCT_SIZE',
    'MessageID',
]

# ----------------------------------------------------- CONSTANTS ------------------------------------------------------

# Header
HEADER_STRUCT_FORMAT = '<HxxHH'
HEADER_STRUCT_SIZE = struct.calcsize(HEADER_STRUCT_FORMAT)

# ------------------------------------------------------- TYPES --------------------------------------------------------

# Message IDs (corresponds to intf_message_t)
MessageID = IntEnum(
    'MessageID',
    [
        # Requests
        'REQUEST_AUTOKEY',
        'REQUEST_PANIC',
        'REQUEST_PING',
        'REQUEST_RESTORE_DEFAULT_CONFIG',
        'REQUEST_SET_BUZZER_ENABLED',
        'REQUEST_SET_BUZZER_FREQUENCY',
        # Replies
        'REPLY_SUCCESS',
        'REPLY_INVALID_MESSAGE',
        'REPLY_INVALID_SIZE',
        'REPLY_INVALID_CRC',
        'REPLY_INVALID_PAYLOAD',
    ],
    start = 0)
