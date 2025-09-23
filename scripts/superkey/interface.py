#
# @file     scripts/superkey/interface.py
# @brief    Module defining the Interface class.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-29
# @cpyrt    © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
#

# ------------------------------------------------------ IMPORTS -------------------------------------------------------

import serial
import struct
import time
from typing import Iterable, Optional, Tuple

from .constants import *
from .types import *

# ------------------------------------------------------ EXPORTS -------------------------------------------------------

__all__ = [
    'InterfaceError',
    'InvalidMessageError',
    'InvalidSizeError',
    'InvalidCRCError',
    'InvalidPayloadError',
    'InvalidValueError',
    'Interface',
    'InteractiveInterface',
]

# ----------------------------------------------------- CONSTANTS ------------------------------------------------------

# Header
HEADER_STRUCT_FORMAT = '<HxxHH'
HEADER_STRUCT_SIZE = struct.calcsize(HEADER_STRUCT_FORMAT)

# ------------------------------------------------------- TYPES --------------------------------------------------------

class InterfaceError(RuntimeError):
    """Base class for errors thrown by `Interface`."""
    def __init__(self, message: str):
        """Initializes a new error with the specified message."""
        super().__init__(message)


class InvalidMessageError(InterfaceError):
    """Exception indicating that a `REPLY_INVALID_MESSAGE` reply was received."""
    def __init__(self):
        """Initializes a new instance."""
        super().__init__("Reply: Invalid message.")


class InvalidSizeError(InterfaceError):
    """Exception indicating that a `REPLY_INVALID_SIZE` reply was received."""
    def __init__(self):
        """Initializes a new instance."""
        super().__init__("Reply: Invalid size.")


class InvalidCRCError(InterfaceError):
    """Exception indicating that a `REPLY_INVALID_CRC` reply was received."""
    def __init__(self):
        """Initializes a new instance."""
        super().__init__("Reply: Invalid CRC.")


class InvalidPayloadError(InterfaceError):
    """Exception indicating that a `REPLY_INVALID_PAYLOAD` reply was received."""
    def __init__(self):
        """Initializes a new instance."""
        super().__init__("Reply: Invalid payload.")


class InvalidValueError(InterfaceError):
    """Exception indicating that a `REPLY_INVALID_VALUE` reply was received."""
    def __init__(self):
        """Initializes a new instance."""
        super().__init__('Reply: Invalid value.')


class Interface:
    """
    Class encapsulating the serial interface provided by the SuperKey hardware.
    """
    def __init__(self,
                 port: str = SUPERKEY_DEFAULT_PORT,
                 baudrate: int = SUPERKEY_DEFAULT_BAUDRATE,
                 timeout: Optional[float] = SUPERKEY_DEFAULT_TIMEOUT):
        """
        Initializes a new instance with the specified serial port configuration.
        """
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.serial = None

    def __enter__(self):
        """
        Acquires the serial port.
        """
        self.serial = serial.Serial(port=self.port, baudrate=self.baudrate, timeout=self.timeout)
        if not self.serial.is_open:
            raise InterfaceError("Failed to open serial port!")
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        """
        Releases the serial port.
        """
        if self.serial is not None and self.serial.is_open:
            self.serial.close()

    def autokey(self, string: str, flags: Iterable[AutokeyFlag] = [], block: bool = False):
        """
        Sends the `REQUEST_AUTOKEY_EX` command. Queues the specified string to be automatically keyed.
        """
        # Get flag byte, setting each bit individually
        flag_byte = 0
        for flag in flags:
            flag_byte = flag_byte | (1 << int(flag))

        # Assemble payload
        payload = (struct.pack('<B', flag_byte) +           # First byte is flags
                   bytes(string, encoding='ascii') +        # Then the string
                   b'\x00')                                 # Null terminator needs to be added manually

        # Send packet and check reply
        self.__send_packet(MessageID.REQUEST_AUTOKEY_EX, payload)
        self.__check_reply_empty()

        # If blocking was selected, wait for autokey to complete
        if block:
            self.autokey_wait()

    def autokey_count(self) -> int:
        """
        Sends the `REQUEST_AUTOKEY_COUNT` command. Returns the number of Morse code elements in the autokey buffer.
        """
        self.__send_packet(MessageID.REQUEST_AUTOKEY_COUNT)
        return self.__check_reply('<H')[0]

    def autokey_quick_msg(self, index: int):
        """
        Sends the `REQUEST_AUTOKEY_QUICK_MSG` command. Keys a quick message.
        """
        self.__send_packet(MessageID.REQUEST_AUTOKEY_QUICK_MSG, struct.pack('<B', index))
        self.__check_reply_empty()

    def autokey_wait(self, delay: float = 0.1):
        """
        Waits until the autokey buffer is empty.
        NOTE: This is not an interface call - it periodically polls `autokey_count()`.
        """
        while self.autokey_count() != 0:
            time.sleep(delay)

    def get_buzzer_enabled(self) -> bool:
        """
        Sends the `REQUEST_GET_BUZZER_ENABLED` command. Returns whether the buzzer is enabled or not.
        """
        self.__send_packet(MessageID.REQUEST_GET_BUZZER_ENABLED)
        return self.__check_reply('<?')[0]

    def get_buzzer_frequency(self) -> int:
        """
        Sends the `REQUEST_GET_BUZZER_FREQUENCY` command. Returns the current buzzer frequency, in Hz.
        """
        self.__send_packet(MessageID.REQUEST_GET_BUZZER_FREQUENCY)
        return self.__check_reply('<H')[0]

    def get_humanizer_level(self) -> float:
        """
        Sends the `REQUEST_GET_HUMANIZER_LEVEL` command. Returns the current humanizer level as a fraction.
        """
        self.__send_packet(MessageID.REQUEST_GET_HUMANIZER_LEVEL)
        return self.__check_reply('<f')[0]

    def get_invert_paddles(self) -> bool:
        """
        Sends the `REQUEST_GET_INVERT_PADDLES` command. Returns whether or not the paddles are inverted.
        """
        self.__send_packet(MessageID.REQUEST_GET_INVERT_PADDLES)
        return self.__check_reply('<?')[0]

    def get_io_polarity(self, pin: IOPin) -> IOPolarity:
        """
        Sends the `REQUEST_GET_IO_POLARITY` command. Returns the polarity of the specified I/O pin.
        """
        self.__send_packet(MessageID.REQUEST_GET_IO_POLARITY, struct.pack('<B', pin))
        return IOPolarity(self.__check_reply('<B')[0])

    def get_io_state(self, pin: IOPin) -> IOPin:
        """
        Sends the `REQUEST_GET_IO_STATE` command. Returns `true` if the specified input / output pin is active.
        """
        self.__send_packet(MessageID.REQUEST_GET_IO_STATE, struct.pack('<B', pin))
        return self.__check_reply('<?')[0]

    def get_io_state_for_type(self, type: IOType) -> IOType:
        """
        Sends the `REQUEST_GET_IO_STATE_FOR_TYPE` command. Returns `true` if any I/O pin with the specified type is on.
        """
        self.__send_packet(MessageID.REQUEST_GET_IO_STATE_FOR_TYPE, struct.pack('<B', type))
        return self.__check_reply('<?')[0]

    def get_io_type(self, pin: IOPin) -> IOType:
        """
        Sends the `REQUEST_GET_IO_TYPE` command. Returns the type of the specified I/O pin.
        """
        self.__send_packet(MessageID.REQUEST_GET_IO_TYPE, struct.pack('<B', pin))
        return IOType(self.__check_reply('<B')[0])

    def get_led_enabled(self, led: LED) -> bool:
        """
        Sends the `REQUEST_GET_LED_ENABLED` command. Returns whether or not the specified LED is enabled.
        """
        self.__send_packet(MessageID.REQUEST_GET_LED_ENABLED, struct.pack('<b', led))
        return self.__check_reply('<?')[0]

    def get_paddle_mode(self) -> PaddleMode:
        """
        Sends the `REQUEST_GET_PADDLE_MODE` command. Returns the currently selected paddle mode.
        """
        self.__send_packet(MessageID.REQUEST_GET_PADDLE_MODE)
        return PaddleMode(self.__check_reply('<B')[0])

    def get_quick_msg(self, index: int) -> str:
        """
        Sends the `REQUEST_GET_QUICK_MSG` command. Returns the text of the specified quick message.
        """
        self.__send_packet(MessageID.REQUEST_GET_QUICK_MSG, struct.pack('<B', index))
        return str(self.__check_reply(), encoding='ascii')[:-1]

    def get_trainer_mode(self) -> bool:
        """
        Sends the `REQUEST_GET_TRAINER_MODE` command. Returns whether trainer mode is enabled or not.
        """
        self.__send_packet(MessageID.REQUEST_GET_TRAINER_MODE)
        return self.__check_reply('<?')[0]

    def get_wpm(self) -> float:
        """
        Sends the `REQUEST_GET_WPM` command. Returns the current WPM setting.
        """
        self.__send_packet(MessageID.REQUEST_GET_WPM)
        return self.__check_reply('<f')[0]

    def get_wpm_scale(self, element: CodeElement) -> float:
        """
        Sends the `REQUEST_GET_WPM_SCALE` command. Returns the current WPM scale for the specified code element.
        """
        self.__send_packet(MessageID.REQUEST_GET_WPM_SCALE, struct.pack('<B', element))
        return self.__check_reply('<f')[0]

    def invalidate_quick_msg(self, idx: int):
        """
        Sends the `REQUEST_INVALIDATE_QUICK_MSG` command. Deletes the specified quick message.
        """
        self.__send_packet(MessageID.REQUEST_INVALIDATE_QUICK_MSG, struct.pack('<B', idx))
        self.__check_reply_empty()

    def panic(self):
        """
        Sends the `REQUEST_PANIC` command. Immediately and unconditionally stops keying.
        """
        self.__send_packet(MessageID.REQUEST_PANIC)
        self.__check_reply_empty()

    def ping(self):
        """
        Sends the `REQUEST_PING` command. Keys a short test message.
        """
        self.__send_packet(MessageID.REQUEST_PING)
        self.__check_reply_empty()

    def restore_default_config(self):
        """
        Sends the `REQUEST_RESTORE_DEFAULT_CONFIG` command. Restores the device to its default configuration.
        """
        self.__send_packet(MessageID.REQUEST_RESTORE_DEFAULT_CONFIG)
        self.__check_reply_empty()

    def set_buzzer_enabled(self, enabled: bool):
        """
        Sends the `REQUEST_SET_BUZZER_ENABLED` command. Enables or disables the device's built-in buzzer.
        """
        self.__send_packet(MessageID.REQUEST_SET_BUZZER_ENABLED, struct.pack('<?', enabled))
        self.__check_reply_empty()

    def set_buzzer_frequency(self, frequency: int):
        """
        Sends the `REQUEST_SET_BUZZER_FREQUENCY` command. Sets the frequency (in Hz) of the device's built-in buzzer.
        """
        self.__send_packet(MessageID.REQUEST_SET_BUZZER_FREQUENCY, struct.pack('<H', frequency))
        self.__check_reply_empty()

    def set_humanizer_level(self, level: float):
        """
        Sends the `REQUEST_SET_HUMANIZER_LEVEL` command. Sets the humanizer level as a fraction.
        """
        self.__send_packet(MessageID.REQUEST_SET_HUMANIZER_LEVEL, struct.pack('<f', level))
        self.__check_reply_empty()

    def set_invert_paddles(self, inverted: bool):
        """
        Sends the `REQUEST_SET_INVERT_PADDLES` command. Sets whether the paddles are inverted.
        """
        self.__send_packet(MessageID.REQUEST_SET_INVERT_PADDLES, struct.pack('<?', inverted))
        self.__check_reply_empty()

    def set_io_polarity(self, pin: IOPin, polarity: IOPolarity):
        """
        Sends the `REQUEST_SET_IO_POLARITY` command. Sets the polarity of the specified I/O pin.
        """
        self.__send_packet(MessageID.REQUEST_SET_IO_POLARITY, struct.pack('<BB', pin, polarity))
        self.__check_reply_empty()

    def set_io_type(self, pin: IOPin, type: IOType):
        """
        Sends the `REQUEST_SET_IO_TYPE` command. Sets the type of the specified I/O pin.
        """
        self.__send_packet(MessageID.REQUEST_SET_IO_TYPE, struct.pack('<BB', pin, type))
        self.__check_reply_empty()

    def set_led_enabled(self, led: LED, enabled: bool):
        """
        Sends the `REQUEST_SET_LED_ENABLED` command. Sets whether the specified LED is enabled or not.
        """
        self.__send_packet(MessageID.REQUEST_SET_LED_ENABLED, struct.pack('<B?', led, enabled))
        self.__check_reply_empty()

    def set_paddle_mode(self, mode: PaddleMode):
        """
        Sends the `REQUEST_SET_PADDLE_MODE` command. Sets the current keyer paddle mode.
        """
        self.__send_packet(MessageID.REQUEST_SET_PADDLE_MODE, struct.pack('<B', mode))
        self.__check_reply_empty()

    def set_quick_msg(self, index: int, string: str):
        """
        Sends the `REQUEST_SET_QUICK_MSG` command. Sets the text of a quick message.
        """
        # Assemble payload
        payload = (struct.pack('<B', index) +               # First byte is index
                   bytes(string, encoding='ascii') +        # Then the string
                   b'\x00')                                 # Null terminator needs to be added manually

        # Send packet and check reply
        self.__send_packet(MessageID.REQUEST_SET_QUICK_MSG, payload)
        self.__check_reply_empty()

    def set_trainer_mode(self, enabled: bool):
        """
        Sends the `SET_TRAINER_MODE` command. Enables or disables trainer mode.
        """
        self.__send_packet(MessageID.REQUEST_SET_TRAINER_MODE, struct.pack('<?', enabled))
        self.__check_reply_empty()

    def set_wpm(self, wpm: float):
        """
        Sends the `REQUEST_SET_WPM` command. Sets the keyer's WPM setting.
        """
        self.__send_packet(MessageID.REQUEST_SET_WPM, struct.pack('<f', wpm))
        self.__check_reply_empty()

    def set_wpm_scale(self, element: CodeElement, scale: float):
        """
        Sends the `REQUEST_SET_WPM_SCALE` command. Sets the WPM scale for the specified code element.
        """
        self.__send_packet(MessageID.REQUEST_SET_WPM_SCALE, struct.pack('<Bf', element, scale))
        self.__check_reply_empty()

    def version(self) -> str:
        """
        Sends the `REQUEST_VERSION` command. Returns the device's version information.
        """
        header = self.__class__.__pack_header(MessageID.REQUEST_VERSION)
        self.__send(header)
        return self.__check_reply_str()

    def __validate_serial(self):
        """
        Raises an error if the serial port is not open.
        """
        if self.serial is None:
            raise InterfaceError("The serial port is not open.")

    def __send(self, buffer: bytes):
        """
        Transmits the specified buffer.
        """
        self.__validate_serial()
        self.serial.write(buffer)

    def __send_packet(self, message: MessageID, payload: Optional[bytes] = None):
        """
        Sends a packet with the specified message ID and payload.
        """
        # Get header
        size = 0
        crc = 0
        if payload is not None:
            size = len(payload)
            crc = self.__class__.__crc16(payload)
        header = self.__class__.__pack_header(message, size, crc)

        # Send data
        self.__send(header)
        if payload is not None:
            self.__send(payload)

    def __receive(self, size: int):
        """
        Receives the specified number of bytes.
        """
        self.__validate_serial()
        return self.serial.read(size=size)

    def __receive_header(self):
        """
        Attempts to receive a header from the serial port.
        """
        # Receive reply and verify we got enough data
        reply = self.__receive(HEADER_STRUCT_SIZE)
        if len(reply) != HEADER_STRUCT_SIZE:
            raise InterfaceError('No reply received.')

        # Unpack the header and verify the size and CRC are correct
        return self.__class__.__unpack_header(reply)

    def __check_reply(self, format: Optional[str] = None) -> Tuple[any, ...] | bytes:
        """
        Attempts to receive a reply with a payload from the device.
        """
        # Unpack header
        message, size, crc = self.__receive_header()

        # Receive payload
        if size != 0:
            payload = self.__receive(size)
            if len(payload) != size:
                raise InterfaceError('No payload received.')
        else:
            payload = None

        # Check the message ID
        self.__check_reply_message_id(message)

        # Check CRC
        if payload is not None and self.__class__.__crc16(payload) != crc:
            raise InterfaceError('Reply: Invalid CRC?')

        # Do we have a format string?
        if format is not None:

            # Check payload length
            if len(payload) != struct.calcsize(format):
                raise InterfaceError('Reply: Invalid payload?')

            # Unpack struct and return
            return struct.unpack(format, payload)

        else:

            # Return packed bytes
            return payload

    def __check_reply_empty(self):
        """
        Attempts to receive a generic empty reply from the device.
        """
        # Unpack message
        message, size, crc = self.__receive_header()
        if size != 0 or crc != 0:
            raise InterfaceError('Reply: Invalid size / CRC?')

        # Check the message ID
        self.__check_reply_message_id(message)

    def __check_reply_str(self) -> Optional[str]:
        """
        Attempts to receive a reply with a string payload from the device.
        """
        # Unpack header
        message, size, crc = self.__receive_header()

        # Receive payload
        if size != 0:
            payload = self.__receive(size)
            if len(payload) != size:
                raise InterfaceError('No payload received.')
        else:
            return None

        # Check the message ID
        self.__check_reply_message_id(message)

        # Check CRC
        if self.__class__.__crc16(payload) != crc:
            raise InterfaceError('Reply: Invalid CRC?')

        return str(payload, encoding='ascii')

    def __check_reply_message_id(self, message: MessageID):
        """
        Throws an exception if the specified message ID represent a failure.
        """
        if message == MessageID.REPLY_SUCCESS:
            return # no error
        elif message == MessageID.REPLY_INVALID_MESSAGE:
            raise InvalidMessageError()
        elif message == MessageID.REPLY_INVALID_SIZE:
            raise InvalidSizeError()
        elif message == MessageID.REPLY_INVALID_CRC:
            raise InvalidCRCError()
        elif message == MessageID.REPLY_INVALID_PAYLOAD:
            raise InvalidPayloadError()
        elif message == MessageID.REPLY_INVALID_VALUE:
            raise InvalidValueError()
        else:
            raise InterfaceError('Reply: Unknown reply?')

    @staticmethod
    def __crc16(buffer: bytes, seed: int = 0xFFFF):
        """
        Returns a 16-bit CRC for the specified buffer.
        """
        # Adapted from avrlibc _crc16_update implementation
        def update(crc: int, b: int) -> int:
            crc ^= b & 0xFF
            for _ in range(8):
                if crc & 1:
                    crc = (crc >> 1) ^ 0xA001
                else:
                    crc >>= 1
            return crc & 0xFFFF

        # Calculate and return CRC
        crc = seed
        for b in buffer:
            crc = update(crc, b)
        return crc

    @staticmethod
    def __pack_header(message: int, size: int = 0, crc: int = 0) -> bytes:
        """
        Returns a correctly formatted header struct with the specified values.
        """
        return struct.pack(HEADER_STRUCT_FORMAT, message, size, crc)

    @staticmethod
    def __unpack_header(buffer: bytes) -> Tuple[int, int, int]:
        """
        Unpacks the specified header struct and returns the parsed values.
        """
        return struct.unpack(HEADER_STRUCT_FORMAT, buffer)


class InteractiveInterface:
    """
    Class acting as a proxy for `Interface`, wrapping each function call in a serial port transaction.

    This is intended as a convenience to improve the syntax for **occasional** commands (e.g., in an interactive Python
    terminal). It allows sending commands to the SuperKey without having to wrap each command in a `with` block. It
    should not be used in normal scripts, as there is significant overhead in repeatedly opening and closing the serial
    port for every command.

    Example usage:
    ```
    # Note that no `with` statement is required
    intf = InteractiveInterface()
    intf.set_buzzer_frequency(800)
    intf.set_buzzer_enabled(True)
    ```
    """
    def __init__(self, *args, **kwargs):
        """
        Initializes a new instance with the specified serial port configuration.
        """
        self.__intf = Interface(*args, **kwargs)

    def __getattr__(self, name: str):
        """
        Gets the specified attribute.
        """
        attr = getattr(self.__intf, name)
        if callable(attr):
            def wrapper(*args, **kwargs):
                with self.__intf as _:
                    return attr(*args, **kwargs)
            wrapper.__doc__ = attr.__doc__
            return wrapper
        else:
            return attr
