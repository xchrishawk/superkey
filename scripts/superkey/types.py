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

# ------------------------------------------------------ EXPORTS -------------------------------------------------------

__all__ = [
    'AutokeyFlag',
    'CodeElement',
    'IOPin',
    'IOPolarity',
    'IOType',
    'LED',
    'MessageID',
    'PaddleMode',
]

# ------------------------------------------------------- TYPES --------------------------------------------------------

# Autokey option flags (corresponds to keyer_autokey_flag_t)
AutokeyFlag = IntEnum(
    'AutokeyFlag',
    [
        'NO_LETTER_SPACE',
    ],
    start = 0
)

# Morse code elements (corresponds to wpm_element_t)
CodeElement = IntEnum(
    'CodeElement',
    [
        'DOT',
        'DASH',
        'ELEMENT_SPACE',
        'LETTER_SPACE',
        'WORD_SPACE'
    ],
    start = 0
)

# I/O pins (correspinds to io_pin_t)
IOPin = IntEnum(
    'IOPin',
    [
        'TRS_0_TIP',
        'TRS_0_RING',
        'TRS_1_TIP',
        'TRS_1_RING',
        'TRS_2_TIP',
        'TRS_2_RING',
        'TRS_3_TIP',
        'TRS_3_RING',
    ],
    start = 0
)

# I/O polarities (corresponds to io_polarity_t)
IOPolarity = IntEnum(
    'IOPolarity',
    [
        'ACTIVE_LOW',
        'ACTIVE_HIGH',
    ],
    start = 0
)

# I/O types (corresponds to io_type_t)
IOType = IntEnum(
    'IOType',
    [
        # Inputs
        'INPUT_STRAIGHT_KEY',
        'INPUT_PADDLE_LEFT',
        'INPUT_PADDLE_RIGHT',
        # Outputs
        'OUTPUT_KEYER',
    ],
    start = 0
)

# LED IDs (corresponds to led_t)
LED = IntEnum(
    'LED',
    [
        'STATUS',
        'KEY',
    ],
    start = 0
)

# Message IDs (corresponds to intf_message_t)
MessageID = IntEnum(
    'MessageID',
    [
        # Requests
        'REQUEST_AUTOKEY',
        'REQUEST_AUTOKEY_EX',
        'REQUEST_GET_BUZZER_ENABLED',
        'REQUEST_GET_BUZZER_FREQUENCY',
        'REQUEST_GET_INVERT_PADDLES',
        'REQUEST_GET_IO_POLARITY',
        'REQUEST_GET_IO_STATE',
        'REQUEST_GET_IO_STATE_FOR_TYPE',
        'REQUEST_GET_IO_TYPE',
        'REQUEST_GET_LED_ENABLED',
        'REQUEST_GET_PADDLE_MODE',
        'REQUEST_GET_TRAINER_MODE',
        'REQUEST_GET_WPM',
        'REQUEST_GET_WPM_SCALE',
        'REQUEST_PANIC',
        'REQUEST_PING',
        'REQUEST_RESTORE_DEFAULT_CONFIG',
        'REQUEST_SET_BUZZER_ENABLED',
        'REQUEST_SET_BUZZER_FREQUENCY',
        'REQUEST_SET_INVERT_PADDLES',
        'REQUEST_SET_IO_POLARITY',
        'REQUEST_SET_IO_TYPE',
        'REQUEST_SET_LED_ENABLED',
        'REQUEST_SET_PADDLE_MODE',
        'REQUEST_SET_TRAINER_MODE',
        'REQUEST_SET_WPM',
        'REQUEST_SET_WPM_SCALE',
        'REQUEST_VERSION',
        # Replies
        'REPLY_SUCCESS',
        'REPLY_INVALID_MESSAGE',
        'REPLY_INVALID_SIZE',
        'REPLY_INVALID_CRC',
        'REPLY_INVALID_PAYLOAD',
        'REPLY_INVALID_VALUE',
    ],
    start = 0
)

# Paddle modes (corresponds to keyer_paddle_mode_t)
PaddleMode = IntEnum(
    'PaddleMode',
    [
        'IAMBIC',
        'ULTIMATIC',
        'ULTIMATIC_ALTERNATE',
    ],
    start = 0
)
